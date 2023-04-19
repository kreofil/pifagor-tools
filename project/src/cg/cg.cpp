#include <QtCore>
#include "cg.h"
//#include "../cg2dot/cg2dot.h"
//QString Cg2Dot (Cg& cg);
//=================================================================================================
// Конструктор УГ
Cg::Cg() {
    name="CG_WITH_NO_NAME";
}

//=================================================================================================
// Добавление в УГ управляющего узла ФПП программы.
void Cg::Append(CtrlNode* n) {
    ctrlNodeList.append(n);
}

//=================================================================================================
// Добавление синала в УГ.
void Cg::Append(Signal* s) {
    signalList.append(s);
}

//=================================================================================================
// Добавление динамической связи в УГ.
void Cg::Append(DynLink* d) {
    dynLinkList.append(d);
}

//=================================================================================================
// Очистка класса от динамических структур перед возможным повторным использованием
void Cg::Clear() {
    // Очистка списков
    ctrlNodeList.clear();
    signalList.clear();
    dynLinkList.clear();
    // Очистка имени управляющего графа
    name.clear();
}

//=================================================================================================
// Тестовый вывод списка узлов УГ
void Cg::TestOutCtrlNodeList(QTextStream& out) {
    out << "===== Control Graph Parameters =====" << endl;
    out << "List of Nodes:" << endl;
    if(ctrlNodeList.empty()) {
        out << "Is Empty" << endl;
        return;
    }
    int size = ctrlNodeList.size();
    out << "size = " << size << endl;
    for(int i = 0; i < size; ++i) {
        out <<  i << ": ";
        ctrlNodeList[i]->TestOut(out);
    }
}

//=================================================================================================
// Тестовый вывод списка сигналов
void Cg::TestOutSignalList(QTextStream& out) {
    out << "List of Signals:" << endl;
    if(signalList.empty()) {
        out << "Is Empty" << endl;
        return;
    }
    int size = signalList.size();
    out << "size = " << size << endl;
    for(int i = 0; i < size; ++i) {
        out <<  i << ": ";
        signalList[i]->TestOut(out);
    }
}

//=================================================================================================
// Тестовый вывод списка динамических связей
void Cg::TestOutDynLinkList(QTextStream& out) {
    out << "List of Dynamic Links:" << endl;
    if(dynLinkList.empty()) {
        out << "Is Empty" << endl;
        return;
    }
    int size = dynLinkList.size();
    out << "size = " << size << endl;
    for(int i = 0; i < size; ++i) {
        out <<  i << ": ";
        dynLinkList[i]->TestOut(out);
    }
}

//=================================================================================================
// Тестовый вывод всех составляющих УГ
void Cg::TestOut(QTextStream& out) {
    // Тестовый вывод имени
    out << "Name is: " << name << endl;
    // Тестовый вывод списка узлов УГ
    TestOutCtrlNodeList(out);
    // Тестовый вывод списка сигналов
    TestOutSignalList(out);
    // Тестовый вывод списка динамических связей
    TestOutDynLinkList(out);
}


QString Cg::GetAsStrCtrlNodeList()
{
    QString rez="List of Nodes:\n";
    if(ctrlNodeList.empty()) {
        rez+="Is Empty\n";
        return rez;
    }
    int size = ctrlNodeList.size();
    rez+=QString("size = %1\n").arg(size);
    for(int i = 0; i < size; ++i) {
        rez+=QString("%1:\t").arg(i);
        rez+=ctrlNodeList[i]->GetAsStr();
        rez+='\n';
    }
    return rez;
}
QString Cg::GetAsStrSignalList()
{
    QString rez="List of Signals:\n";
    if(signalList.empty()) {
        rez+="Is Empty\n";
        return rez;
    }
    int size = signalList.size();
    rez+=QString("size = %1\n").arg(size);
    for(int i = 0; i < size; ++i) {
        rez+=QString("%1:\t").arg(i);
        rez+=signalList[i]->GetAsStr();
        rez+='\n';
    }
    return rez;
}
QString Cg::GetAsStrDynLinkList()
{
    QString rez="List of Dynamic Links:\n";
    if(dynLinkList.empty()) {
        rez+="Is Empty\n";
        return rez;
    }
    int size = dynLinkList.size();
    rez+=QString("size = %1\n").arg(size);
    for(int i = 0; i < size; ++i) {
        rez+=QString("%1:\t").arg(i);
        rez+=dynLinkList[i]->GetAsStr();
        rez+='\n';
    }
    return rez;
}
QString Cg::GetAsStr()
{
    return GetAsStrCtrlNodeList()+GetAsStrSignalList()+GetAsStrDynLinkList();
}

//=================================================================================================
// Преобразование РИГ в УГ
void Cg::Transform(Rig *rig)
{
    //BegActor* argActor = 0;
    InterpActor* interpActor = 0;
    DataListActor* dataListActor = 0;
    ParListActor* parListActor = 0;
    AsListActor* asListActor = 0;
    RetActor* retActor = 0;
    Automat *automat = 0;
    QVector<Trio> trioList; // Вектор, используемый для временного хранения управляющих связей
    Trio trio;  // Временная структура для формирования управляющих связей
    MemType dType, fType;
    int listSize;

    // Получение имени функции из РИГ (нулевая внешняя ссылка)
    QVector<ExtRef*>* pExtRefList = rig->GetExtRefList();
    ExtRef* pFirstExtRef = (*pExtRefList)[0];
    QString extRefName = pFirstExtRef->GetName();
    // Сохранение имени функции в УГ
    SetName(extRefName);

    // Формирование доступа к списку акторов
    QVector<Actor*>* pActorList = rig->GetActorList();
    int actorListSize = pActorList->size();
    AutomatState zState(0);
    // Использование акторов для формирования списка узлов УГ
    for(int i = 0; i < actorListSize; i++) {
        Actor* actor = (*pActorList)[i];
        // Определение типа актора
        ActorType actType = actor->GetActorType();
        switch(actType) {
        case argType:
            // Данный актор не имеет входных связей.
            //argActor = static_cast<BegActor*>(actor);
            automat = new ArgAutomat();
            break;
        case interpType:
            interpActor = static_cast<InterpActor*>(actor);
            // Проверка типа информационной связи для порождения элементов УГ
            dType = interpActor->arg.GetMemType();
            switch(dType) {
            case act:
                // Порождается связь в УГ
                trio.source = interpActor->arg.GetIndex();
                trio.reciver = i;
                trio.input = 1;
                trioList.append(trio);
                break;
            case ext:
            case loc:
            case spec:
                // Порождается сигнал в УГ
                Append(new Signal(i,1));
                break;
            }
            fType = interpActor->func.GetMemType();
            switch(fType) {
            case act:
                // Порождается связь в УГ
                trio.source = interpActor->func.GetIndex();
                trio.reciver = i;
                trio.input = 2;
                trioList.append(trio);
                break;
            case ext:
            case loc:
            case spec:
                // Порождается сигнал в УГ
                Append(new Signal(i,2));
                break;
            }
            automat = new InterpAutomat();
            break;
        case datalistType:
            dataListActor = static_cast<DataListActor*>(actor);
            // Для списков необходимо перебрать список связей memList
            listSize = dataListActor->memList.size(); // Число входов в списке
            for(int j = 0; j < listSize; j++) {
                dType = dataListActor->memList[j].GetMemType();
                switch(dType) {
                case act:
                    // Порождается связь в УГ
                    trio.source = dataListActor->memList[j].GetIndex();
                    trio.reciver = i;
                    trio.input = j+1;
                    trioList.append(trio);
                    break;
                case ext:
                case loc:
                case spec:
                    // Порождается сигнал в УГ
                    Append(new Signal(i,j+1));
                    break;
                }
            }
            automat = new DlistAutomat();
            break;
        case parlistType:
            parListActor = static_cast<ParListActor*>(actor);
            // Для списков необходимо перебрать список связей memList
            listSize = parListActor->memList.size(); // Число входов в списке
            for(int j = 0; j < listSize; j++) {
                dType = parListActor->memList[j].GetMemType();
                switch(dType) {
                case act:
                    // Порождается связь в УГ
                    trio.source = parListActor->memList[j].GetIndex();
                    trio.reciver = i;
                    trio.input = j+1;
                    trioList.append(trio);
                    break;
                case ext:
                case loc:
                case spec:
                    // Порождается сигнал в УГ
                    Append(new Signal(i,j+1));
                    break;
                }
            }
            automat = new PlistAutomat();
            break;
        case aslistType:
            asListActor = static_cast<AsListActor*>(actor);
            // Для списков необходимо перебрать список связей memList
            listSize = asListActor->memList.size(); // Число входов в списке
            for(int j = 0; j < listSize; j++) {
                dType = asListActor->memList[j].GetMemType();
                switch(dType) {
                case act:
                    // Порождается связь в УГ
                    trio.source = asListActor->memList[j].GetIndex();
                    trio.reciver = i;
                    trio.input = j+1;
                    trioList.append(trio);
                    break;
                case ext:
                case loc:
                case spec:
                    // Порождается сигнал в УГ
                    Append(new Signal(i,j+1));
                    break;
                }
            }
            automat = new AlistAutomat();
            break;
        case retType:
            retActor = static_cast<RetActor*>(actor);
            dType = retActor->arg.GetMemType();
            switch(dType) {
            case act:
                // Порождается связь в УГ
                trio.source = retActor->arg.GetIndex();
                trio.reciver = i;
                trio.input = 1;
                trioList.append(trio);
                break;
            case ext:
            case loc:
            case spec:
                // Порождается сигнал в УГ
                Append(new Signal(i,1));
                break;
            }
            automat = new RetAutomat();
            break;
        default:
            qDebug() << "Incorrect Type of Actor";
            exit(1);
        }
        int delNode = actor->GetDelay();
        // Формирование узла УГ по данным, полученным из РИГ
        CtrlNode* ctrlNode = new CtrlNode(delNode, automat, zState, i);
        Append(ctrlNode);

    }

    // После формирования управляющих связей можно начать их приписывание узлам.
    QVector<CtrlNode*>* pCtrlNodeList = GetCtrlNodeList();
    int linkSize = trioList.size();
    for(int i = 0; i < linkSize; i++) {
        CtrlNode* ctrlNode = (*pCtrlNodeList)[trioList[i].source];
        ctrlNode->Append(trioList[i].reciver, trioList[i].input);
    }

    // Формирование динамических связей за счет обхода констант и выявления среди них задержанных
    QVector<Value*>* pLocalConstList = rig->GetlocalConstList();
    int localConstListSize = (*pLocalConstList).size();
    for(int i = 0; i < localConstListSize; i++) {
        Value* v = (*pLocalConstList)[i];
        if(v->GetValueType() == DelayValueType) {
            DelayValue* dV = static_cast<DelayValue*>(v);
            Append(new DynLink(dV->GetDelayNumber(), dV->GetDelayLink()));
        }
    }

    return;
}

//=================================================================================================
// Генерация текстового представления
bool Cg::SaveText(QString fileName)
{
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        // Вывод в файл имени функции
        stream << GetName() << endl;
        // Формирование и вывод в файл Узлов УГ
        stream << "id\tdelay\tautomat\t\tinode\tlinks" << endl;
        QVector<CtrlNode*>* pCtrlNodeList = GetCtrlNodeList();
        for(int i = 0; i < pCtrlNodeList->size(); i++) {
            CtrlNode* pCtrlNode = (*pCtrlNodeList)[i];
            stream << i << '\t' << pCtrlNode->GetDelNum() << '\t'
                   << pCtrlNode->GetAutomatName() << ','
                   << pCtrlNode->GetAutomatState() << "\t\t"
                   << pCtrlNode->GetInode() << '\t';
            // Формирование и вывод списка связей
            QVector<Link*>* pLinkList = pCtrlNode->GetLinkList();
            int linkListSize = pLinkList->size();
            if(linkListSize > 0) {
                stream << "links:";
                for(int i = 0; i < linkListSize; i++) {
                    Link* pLink = (*pLinkList)[i];
                    stream << pLink->GetNode() << ',' << pLink->GetInput() << ';';
                }
            }
            stream << endl;
        }
        // Вывод в файл сигналов
        stream << "Signals: (Number, Node, Input)" << endl;
        QVector<Signal*>* pSignalList = GetSignalList();
        for(int i = 0; i < pSignalList->size(); i++) {
            Signal* pSignal = (*pSignalList)[i];
            stream << i << '\t' << pSignal->GetNode() << '\t'
                   << pSignal->GetInput() << endl;
        }
        // Вывод в файл динамических связей
        stream << "Dynamic links: (Number, Delay list, Node)" << endl;
        QVector<DynLink*>* pDynLinkList = GetDynLinkList();
        for(int i = 0; i < pDynLinkList->size(); i++) {
            DynLink* pDynLink = (*pDynLinkList)[i];
            stream << i << '\t' << pDynLink->GetDelNum() << '\t' << pDynLink->GetNode() << endl;
        }
        file.close();
        return true;
    }
    return false;
}

//=================================================================================================
// Геттер имени как char*
char* Cg::GetCharName()
{
    char *rez=new char[name.length()+100];
    QByteArray qb = name.toLocal8Bit();
    strcpy(rez,qb.data());
    return rez;
}


// Генерация текстового представления
QString Cg::GetTextRepresentation()
{
    QString rez="";
    QString temp;

    // Вывод в файл имени функции
    rez+=GetName();
    rez+="\n";
    // Формирование и вывод в файл Узлов УГ
    rez+="id\tdelay\tautomat\t\tinode\tlinks\n";

    QVector<CtrlNode*>* pCtrlNodeList = GetCtrlNodeList();
    for(int i = 0; i < pCtrlNodeList->size(); i++)
    {
        CtrlNode* pCtrlNode = (*pCtrlNodeList)[i];
        temp=QString("%1\t%2\t%3,%4\t\t%5\t")
                .arg(i)
                .arg(pCtrlNode->GetDelNum())
                .arg(pCtrlNode->GetAutomatName())
                .arg(pCtrlNode->GetAutomatState())
                .arg(pCtrlNode->GetInode());
        rez+=temp;

        // Формирование и вывод списка связей
        QVector<Link*>* pLinkList = pCtrlNode->GetLinkList();
        int linkListSize = pLinkList->size();
        if(linkListSize > 0)
        {
            rez+="links:";
            for(int i = 0; i < linkListSize; i++)
            {
                Link* pLink = (*pLinkList)[i];
                temp=QString("%1,%2;")
                        .arg(pLink->GetNode())
                        .arg(pLink->GetInput());
                rez+=temp;
            }
        }
        rez+="\n";
    }

    // Вывод в файл сигналов
    rez+="Signals: (Number, Node, Input)\n";

    QVector<Signal*>* pSignalList = GetSignalList();
    for(int i = 0; i < pSignalList->size(); i++)
    {
        Signal* pSignal = (*pSignalList)[i];
        temp=QString("%1\t%2\t%3\n")
                .arg(i)
                .arg(pSignal->GetNode())
                .arg(pSignal->GetInput());
        rez+=temp;
    }
    // Вывод в файл динамических связей
    rez+="Dynamic links: (Number, Delay list, Node)\n";
    QVector<DynLink*>* pDynLinkList = GetDynLinkList();
    for(int i = 0; i < pDynLinkList->size(); i++)
    {
        DynLink* pDynLink = (*pDynLinkList)[i];
        temp=QString("%1\t%2\t%3\n")
                .arg(i)
                .arg(pDynLink->GetDelNum())
                .arg(pDynLink->GetNode());
        rez+=temp;
    }

    return rez;
}
