#include "rig2dot.h"
Rig2Dotter::Rig2Dotter()
{
    iSpecNum = 0;
    interpActor = 0;
    retActor = 0;
}
//-----------------------------------------------------------------------------
QString Rig2Dotter::Digraph(QString name) {
    return
            "digraph \"" +
            name +
            "\" {\n" +
            "rankdir=BT\n\n"
            ;
}

//-----------------------------------------------------------------------------
QString Rig2Dotter::NodeParam() {
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
QString Rig2Dotter::EdgeParam() {
    return
            "edge [\n"
            "  fontsize = \"14\"\n"
            "  fontcolor = \"black\"\n"
            "  color = \"black\"\n"
            "  style = \"solid\"\n"
            "]\n\n"
            ;
}

//-----------------------------------------------------------------------------
QString Rig2Dotter::ExtNode(QString s1, QString s2) {
    return
            "\"e"+s1+"\" [\n"
            "  fontsize = \"14\"\n"
            "  fontcolor = \"black\"\n"
            "  label = \""+s1+": "+s2+"\"\n"
            "  shape = \"hexagon\"\n"
            "]\n\n"
            ;
}

//-----------------------------------------------------------------------------
QString Rig2Dotter::ValueNode(QString s1, QString s2) {
    s1.replace("\"","\\\"");
    s2.replace("\"","\\\"");
    return
            "\"l"+s1+"\" [\n"
            "  fontsize = \"14\"\n"
            "  fontcolor = \"black\"\n"
            "  label = \""+s1+": "+s2+"\"\n"
            "  shape = \"box\"\n"
            "]\n\n"
            ;
}

//-----------------------------------------------------------------------------
QString Rig2Dotter::SpecNode(QString s1, QString s2) {
    return
            "\"s"+s1+"\" [\n"
            "  fontsize = \"14\"\n"
            "  fontcolor = \"black\"\n"
            "  label = \""+s2+"\"\n"
            "  shape = \"octagon\"\n"
            "]\n\n"
            ;
}

//-----------------------------------------------------------------------------
QString Rig2Dotter::ArgNode(QString s1) {
    return
            "\"n"+s1+"\" [\n"
            "  fontsize = \"14\"\n"
            "  fontcolor = \"black\"\n"
            "  label = \""+s1+"\"\n"
            "  shape = \"invhouse\"\n"
            "]\n\n"
            ;
}

//-----------------------------------------------------------------------------
QString Rig2Dotter::InterpNode(QString s1) {
    return
            "\"n"+s1+"\" [\n"
            "  fontsize = \"14\"\n"
            "  fontcolor = \"black\"\n"
            "  label = \""+s1+"\\n:\"\n"
            "  shape = \"circle\"\n"
            "]\n\n"
            ;
}

//-----------------------------------------------------------------------------
QString Rig2Dotter::DataListNode(QString s1) {
    return
            "\"n"+s1+"\" [\n"
            "  fontsize = \"14\"\n"
            "  fontcolor = \"black\"\n"
            "  label = \""+s1+"\\n(-----)\"\n"
            "  shape = \"box\"\n"
            "  style = \"rounded\"\n"
            "]\n\n"
            ;
}

//-----------------------------------------------------------------------------
QString Rig2Dotter::ParListNode(QString s1) {
    return
            "\"n"+s1+"\" [\n"
            "  fontsize = \"14\"\n"
            "  fontcolor = \"black\"\n"
            "  label = \""+s1+"\\n[-----]\"\n"
            "  shape = \"box\"\n"
            "  style = \"rounded\"\n"
            "]\n\n"
            ;
}

//-----------------------------------------------------------------------------
QString Rig2Dotter::AsynchListNode(QString s1) {
    return
            "\"n"+s1+"\" [\n"
            "  fontsize = \"14\"\n"
            "  fontcolor = \"black\"\n"
            "  label = \""+s1+"\\n<<(---)\"\n"
            "  shape = \"box\"\n"
            "  style = \"rounded\"\n"
            "]\n\n"
            ;
}

//-----------------------------------------------------------------------------
QString Rig2Dotter::ReturnNode(QString s1) {
    return
            "\"n"+s1+"\" [\n"
            "  fontsize = \"14\"\n"
            "  fontcolor = \"black\"\n"
            "  label = \""+s1+"\"\n"
            "  shape = \"invhouse\"\n"
            "]\n\n"
            ;
}

//-----------------------------------------------------------------------------
QString Rig2Dotter::StartLink() {
    return
            "\"n0\" -> \"e0\" [\n"
            "  style = \"invis\"\n"
            "]\n\n"
            ;
}

//-----------------------------------------------------------------------------
QString Rig2Dotter::Link(QString s1, QString s2, QString s3) {
    return
            "\"n"+s1+"\" -> \""+s2+"\" [\n"
            "  fontsize = \"10\"\n"
            "  fontcolor = \"black\"\n"
            "  label = \""+s3+"\"\n"
            "  color = \"black\"\n"
            "  style = \"solid\"\n"
            "]\n\n"
            ;
}

//-----------------------------------------------------------------------------
QString Rig2Dotter::DelayLink(QString s1, QString s2) {
    return
            "\"l"+s1+"\" -> \"n"+s2+"\" [\n"
            "  fontsize = \"10\"\n"
            "  fontcolor = \"black\"\n"
            "  label = \"Delayed\"\n"
            "  color = \"black\"\n"
            "  style = \"dotted\"\n"
            "]\n\n"
            ;
}

//-----------------------------------------------------------------------------
QString Rig2Dotter::DelayStr(int key, QString tmpStr) {
    return "subgraph \"cluster_delay_" + QString::number(key) + "\" { " + tmpStr + "}\n";
}

//=================================================================================================
QString Rig2Dotter::SelectMemLocationNode(MemLocation& memLocation) {
    QString tmpStr;
    int tmpNum;
    int memType = memLocation.GetMemType();
    int memIndex = memLocation.GetIndex();
    switch(memType) {
    case act: // акторы
        tmpStr = "n" + QString::number(memIndex);
        break;
    case ext: // внешние ссылки
        tmpStr = "e" + QString::number(memIndex);
        break;
    case loc: // локальные константы
        tmpStr = "l" + QString::number(memIndex);
        break;
    case spec: // специальные знаки
        // Проверка на наличие спецузла
        if(SpecDict.contains(memIndex)) { // Узел уже встречался
            // Формируется связь с существующим узлом
            tmpNum = SpecDict[memIndex];
        } else {
            // Создается новый спецузел
            tmpNum = iSpecNum++;
            SpecDict[memIndex] = tmpNum;
            specText += SpecNode(QString::number(tmpNum), ::GetAsStr(static_cast<SpecType>(memIndex)));
        }
        tmpStr = "s" + QString::number(tmpNum);
        break;
    }
    return tmpStr;
}

//=================================================================================================
void Rig2Dotter::CreateListLinks(ListActor* pListActor, int i) {
    int memListSize = pListActor->memList.size(); // Число входов в списке
    for(int j = 0; j < memListSize; j++) {
        MemLocation* memLocation = &(pListActor->memList[j]);
        QString tmpStr = SelectMemLocationNode(*memLocation);
        linkText += Link(QString::number(i), tmpStr, QString::number(j+1));
    }
}

//=================================================================================================
// Проверка на строку и символ для приведения их к отображаемому виду
QString Rig2Dotter::TransformStringOrChar(QString value) {
    QString tmpStr;
    if(value[0] == '\"' || value[0] == '\'') {
        int last = value.size()-1;
        value[last] = '\\';
        tmpStr = '\\' + value + value[0];
    } else {
        tmpStr = value;
    }
    return tmpStr;
}
//=================================================================================================
// Пре-очистка всего положенного
void Rig2Dotter::PreInit()
{
    DelayActorDict.clear();
    SpecDict.clear();
    dotText="";
    extText="";
    valueText="";
    specText="";
    actorText="";
    linkText="";
    tmpStr="";
    iSpecNum = 0;
    interpActor = 0;
    retActor = 0;
}

//=================================================================================================
QString Rig2Dotter::Rig2Dot (Rig* rig) {
    PreInit();
    ListActor* pListActor;

    // Формирование заголовка
    QVector<ExtRef*>* pExtRefList = rig->GetExtRefList();
    QString name = (*pExtRefList)[0]->GetName();
    dotText = Digraph(name) + NodeParam() + EdgeParam();

    // Создание нeвидимой связи от имени функции к нулевому узлу
    linkText = StartLink();

    // Формирование узлов для внешних ссылок
    int extRefSize = pExtRefList->size();
    for(int i = 0; i < extRefSize; i++) {
        ExtRef* pExtRef = (*pExtRefList)[i];
        QString name = pExtRef->GetName();
        extText += ExtNode(QString::number(i), name);
    }

    // Формирование узлов для локальных констант
    QVector<Value*>* pValueList = rig->GetlocalConstList();
    int valueSize = pValueList->size();
    for(int i = 0; i < valueSize; i++) {
        Value* pValue = (*pValueList)[i];

        // Проверка задержанный список с возможным формированием задержанной связи
        if(pValue->GetValueType() == DelayValueType) {
            DelayValue* pDelayValue = static_cast<DelayValue*>(pValue);
            //int delayNumber = pDelayValue->GetDelayNumber();
            int delayLink = pDelayValue->GetDelayLink();
            linkText += DelayLink(QString::number(i), QString::number(delayLink));
        }
        QString value = pValue->GetAsStr();

        // Проверка на строку и символ для приведения их к отображаемому виду
        value = TransformStringOrChar(value);

        valueText += ValueNode(QString::number(i), value);
    }

    // Подготовка к формированию узлов для специальных символов
    iSpecNum = 0;

    // Формирование узлов РИГ и связей между всеми элементами графа
    QVector<Actor*>* pActorList = rig->GetActorList();
    int ActorListSize = pActorList->size();
    for(int i = 0; i < ActorListSize; i++) {
        Actor* pActor = (*pActorList)[i];
        int delNum = pActor->GetDelay();
        if(delNum > 0) { // Актор находится в задержанном списке
            if(DelayActorDict.contains(delNum)) { // Задержанный список уже встречался
                QVector<int>* pVec = DelayActorDict[delNum];
                pVec->append(i);
            } else { // Этот задержанный список встретился впервые
                QVector<int>* pNewVector = new QVector<int>;
                pNewVector->append(i);
                DelayActorDict[delNum] = pNewVector;
            }
        }
        // Формирование узлов графа
        //argType, interpType, datalistType, parlistType, aslistType, retType
        MemLocation* memLocation;
        ActorType actorType = pActor->GetActorType();
        switch(actorType) {
        case argType:
            actorText += ArgNode("0");
            break;
        case interpType:
            actorText += InterpNode(QString::number(i));
            // Формирование связей
            interpActor = static_cast<InterpActor*>(pActor);
            memLocation = &(interpActor->arg);
            tmpStr = SelectMemLocationNode(*memLocation);
            linkText += Link(QString::number(i), tmpStr, "Data");
            memLocation = &(interpActor->func);
            tmpStr = SelectMemLocationNode(*memLocation);
            linkText += Link(QString::number(i), tmpStr, "Func");
            break;
        case datalistType:
            actorText += DataListNode(QString::number(i));
            // Формирование связей
            pListActor = static_cast<ListActor*>(pActor);
            CreateListLinks(pListActor, i);
            break;
        case parlistType:
            actorText += ParListNode(QString::number(i));
            // Формирование связей
            pListActor = static_cast<ListActor*>(pActor);
            CreateListLinks(pListActor, i);
            break;
            break;
        case aslistType:
            actorText += AsynchListNode(QString::number(i));
            break;
            // Формирование связей
            pListActor = static_cast<ListActor*>(pActor);
            CreateListLinks(pListActor, i);
            break;
        case retType:
            actorText += ReturnNode(QString::number(i));
            // Формирование связей
            retActor = static_cast<RetActor*>(pActor);
            memLocation = &(retActor->arg);
            tmpStr = SelectMemLocationNode(*memLocation);
            linkText += Link(QString::number(i), tmpStr, "Result");
            break;
        default:
            qDebug() << "Incorrect type of Actor!";
            exit(1);
        }

    }

    // Добавление связей
    dotText += specText + extText + valueText + actorText + linkText;

    // Обход словаря и формирование dot описаний для задержанных списков
    QMapIterator<int, QVector<int>* > iDict(DelayActorDict);
    while(iDict.hasNext()) {
        iDict.next();
        int key = iDict.key();
        QVector<int> vec = *DelayActorDict[key];
        QVectorIterator<int> it(vec);
        QString tmpStr;
        while (it.hasNext()) {
            tmpStr += "\"n" + QString::number(it.next()) + "\" ";
        }
        dotText += DelayStr(key, tmpStr);
    }
    dotText += "}\n";

    return dotText;
}
//=================================================================================================
// Генерация dot- представления
// TODO больше не нужна?
bool Rig2Dotter::SaveRigDot(Rig& rig, QString fileName)
{
    QString dotForm=Rig2Dot(&rig);
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        stream << dotForm << endl;
        file.close();
        return true;
    }
    return false;
}
