#include "cg2dot.h"

//-----------------------------------------------------------------------------
QString Cg2Dotter::CgDigraph(QString name) {
    return
            "digraph \"" +
            name +
            "\" {\n"
            ;
}

//-----------------------------------------------------------------------------
QString Cg2Dotter::CgNodeParam() {
    return
            "node [\n"
            "  fontsize = \"14\"\n"
            "  fontcolor = \"black\"\n"
            "  shape = \"box\"\n"
            "  style = \"solid\"\n"
            "]\n\n"
            ;
}

//-----------------------------------------------------------------------------
QString Cg2Dotter::CgEdgeParam() {
    return
            "edge [\n"
            "  fontsize = \"14\"\n"
            "  fontcolor = \"black\"\n"
            "  color = \"black\"\n"
            "  style = \"dotted\"\n"
            "]\n\n"
            ;
}

//-----------------------------------------------------------------------------
QString Cg2Dotter::NameNode(QString s1) {
    return
            "\"name\" [\n"
            "  fontsize = \"14\"\n"
            "  fontcolor = \"black\"\n"
            "  label = \""+s1+"\"\n"
            "  shape = \"none\"\n"
            "]\n\n"
            ;
}

//-----------------------------------------------------------------------------
QString Cg2Dotter::TerminalNode(QString s1) {
    return
            "\""+s1+"\" [\n"
            "  fontsize = \"14\"\n"
            "  fontcolor = \"black\"\n"
            "  label = \"\"\n"
            "  shape = \"invtriangle\"\n"
            "]\n\n"
            ;
}

//-----------------------------------------------------------------------------
QString Cg2Dotter::SignalNode(QString s1, QString s2) {
    return
            "\"signal"+s1+"\" [\n"
            "  fontsize = \"14\"\n"
            "  fontcolor = \"black\"\n"
            "  label = \""+s2+"\"\n"
            "  shape = \"ellipse\"\n"
            "]\n\n"
            ;
}

//-----------------------------------------------------------------------------
QString Cg2Dotter::ControlNode(QString s1, QString s2) {
    return
            "\"ctrl"+s1+"\" [\n"
            "  fontsize = \"14\"\n"
            "  fontcolor = \"black\"\n"
            "  label = \""+s1+"\\n"+s2+"\"\n"
            "  shape = \"box\"\n"
            "  style = \"rounded\"\n"
            "]\n\n"
            ;
}

//-----------------------------------------------------------------------------
QString Cg2Dotter::CtrlLink(QString s1, QString s2, QString s3) {
    return
            "\""+s1+"\" -> \""+s2+"\" [\n"
            "  fontsize = \"12\"\n"
            "  fontcolor = \"black\"\n"
            "  label = \""+s3+"\"\n"
            "  color = \"black\"\n"
            "  style = \"dotted\"\n"
            "]\n\n"
            ;
}

//-----------------------------------------------------------------------------
QString Cg2Dotter::InvisibleLink(QString s1, QString s2) {
    return
            "\""+s1+"\" -> \""+s2+"\" [\n"
            "  style = \"invis\"\n"
            "]\n\n"
            ;
}

//-----------------------------------------------------------------------------
QString Cg2Dotter::DelimeterLink(QString s1, QString s2) {
    return
            "\""+s1+"\" -> \""+s2+"\" [\n"
            "  fontsize = \"10\"\n"
            "  fontcolor = \"black\"\n"
            "  label = \"\"\n"
            "  arrowtail = \"dot\"\n"
            "  color = \"black\"\n"
            "  style = \"solid\"\n"
            "]\n\n"
            ;
}

//-----------------------------------------------------------------------------
QString Cg2Dotter::DelayStr(QString s1, QString s2) {
    return
            "subgraph \"cluster_delay_"+s1+"\" {\n"
            +"  label=\"Delay List # "+s1+"\"\n"
            +"  labelloc=b\n"
            +"  "+s2+"\n"
            +"}\n\n";
}

//=================================================================================================
QString Cg2Dotter::Cg2Dot (Cg& cg) {
    QMap<int, QVector<int>* > DelayCtrlNodeDict;
    QMap<QString, int> signalDict; // Словарь для слияния кратных сигналов
    QString dotText;    // Формирование dot файла
    QString signalText; // Формирование сигнальных узлов
    QString ctrlText;   // Формирование узлов УГ
    QString linkText;   // Формирование управляющих связей

    // Формирование заголовка
    QString name = cg.GetName();
    dotText = CgDigraph(name) + CgNodeParam() + CgEdgeParam();

    // Формирование узлов и связей для сигналов
    QVector<Signal*>* pSignalList = cg.GetSignalList();
    int signalSize = pSignalList->size();
    QVector<trio> selection;
    int selectionSize = 0;
    // На первом шаге формируются только связи для подсчета совпадений
    for(int i = 0; i < signalSize; i++) {
        Signal* pSignal = (*pSignalList)[i];
        int node = pSignal->GetNode();
        int input = pSignal->GetInput();
        // Отбор несовпадающих комбинаций сигналов и подсчет их встречаемости
        int j = 0;
        while(j < selectionSize) {
            if(node==selection[j].node && input==selection[j].input) {
                selection[j].count++;
                goto _next;
            }
            j++;
        }
        trio t;
        t.node=node; t.input=input; t.count=1;
        selection.append(t);
        selectionSize++;
        _next:;
    }
    // После сжатия начинается обход с формированием узлов и связей.
    for(int i = 0; i < selectionSize; i++) {
        // Формирование сигнального узла
        signalText += SignalNode(QString::number(i), QString::number(selection[i].count));
        // Формирование управляющей связи от сформированного сигнального узла
        linkText += CtrlLink(QString("signal")+QString::number(i),
                             QString("ctrl")+QString::number(selection[i].node),
                             QString::number(selection[i].input));
    }


    // Формирование узлов УГ и связей между всеми элементами графа
    ctrlText += NameNode(name) + TerminalNode(QString("begin"));
    linkText += InvisibleLink(QString("name"), QString("begin"))
             + DelimeterLink(QString("begin"), QString("ctrl0"));
    QVector<CtrlNode*>* pCtrlNodeList = cg.GetCtrlNodeList();
    int CtrlNodeListSize = pCtrlNodeList->size();
    for(int i = 0; i < CtrlNodeListSize; i++) {
        CtrlNode* pCtrlNode = (*pCtrlNodeList)[i];

        int delNum = pCtrlNode->GetDelNum();
        if(delNum > 0) { // Актор находится в задержанном списке
            if(DelayCtrlNodeDict.contains(delNum)) { // Задержанный список уже встречался
                QVector<int>* pVec = DelayCtrlNodeDict[delNum];
                pVec->append(i);
            } else { // Этот задержанный список встретился впервые
                QVector<int>* pNewVector = new QVector<int>;
                pNewVector->append(i);
                DelayCtrlNodeDict[delNum] = pNewVector;
            }
        }

        // Формирование узлов УГ
        QString automatName = pCtrlNode->GetAutomatName();
        int state = pCtrlNode->GetAutomatState();
        int iNode = pCtrlNode->GetInode();
        QString tmpStr = automatName + "#" + QString::number(state)
                                     + "=>" + QString::number(iNode);
        ctrlText += ControlNode(QString::number(i), tmpStr);

        // Формирование управляющих связей из узлов УГ
        QVector<Link*>* pLinkList = pCtrlNode->GetLinkList();
        int linkListSize = pLinkList->size();
        for(int j = 0; j < linkListSize; j++) {
            Link* pLink = (*pLinkList)[j];
            int node = pLink->GetNode();
            int input = pLink->GetInput();
            linkText += CtrlLink(QString("ctrl")+QString::number(i),
                                 QString("ctrl")+QString::number(node), QString::number(input));
        }
    }
    ctrlText += TerminalNode(QString("end"));
    linkText += DelimeterLink(QString("ctrl")+QString::number(CtrlNodeListSize-1),
                             QString("end"));

    // Добавление к выходным узлам задержанных списков связей,
    // динамически устанавливающих соединения при раскрытии задержанных списков
    QVector<DynLink*>* pDynLinkList = cg.GetDynLinkList();
    int dynLinkSize = pDynLinkList->size();
    for(int i = 0; i < dynLinkSize; i++) {
        DynLink* pDynLink = (*pDynLinkList)[i];
        int node = pDynLink->GetNode();
        int delNum = pDynLink->GetDelNum();
        // Формирование динамически подключаемого узла
        signalText += SignalNode(QString("-dyn")+QString::number(i), QString("?"));
        // Формирование динамической связи от заданного управляющего узла к сформированному
        linkText += CtrlLink(QString("ctrl")+QString::number(node),
                             QString("signal-dyn")+QString::number(i), QString::number(delNum));
    }

    // сборка узлов и связей
    dotText += signalText + ctrlText + linkText;

    // Обход словаря и формирование dot описаний для задержанных списков
    QMapIterator<int, QVector<int>* > iDict(DelayCtrlNodeDict);
    while(iDict.hasNext()) {
        iDict.next();
        int key = iDict.key();
        QVector<int> vec = *DelayCtrlNodeDict[key];
        QVectorIterator<int> it(vec);
        QString tmpStr;
        while (it.hasNext()) {
            tmpStr += "\"ctrl" + QString::number(it.next()) + "\" ";
        }
        dotText += DelayStr(QString::number(key), tmpStr);
    }
    dotText += "}\n";

    return dotText;
}

//=================================================================================================
// Генерация dot- представления
bool Cg2Dotter::SaveCgDot(Cg& cg, QString fileName)
{
    QString dotForm=Cg2Dot(cg);
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        stream << dotForm << endl;
        file.close();
        return true;
    }
    return false;
}
