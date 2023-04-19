#include <QtCore>
#include "rig.h"
#include <iostream>
using namespace std;

//=================================================================================================
// Конструктор РИГ
Rig::Rig() {
    name="RIG_WITH_NO_NAME";
    nameTable.resize(5);
}
Rig::Rig(QString newName) {
    name=newName;
    nameTable.resize(5);
}
//=================================================================================================
// Клонировщик
Rig* Rig::Clone()
{
    Rig* newRig=new Rig(name);
    for(int i=0;i<extRefList.size();i++)
        newRig->Append(new ExtRef(extRefList[i]->GetName()));
    for(int i=0;i<localConstList.size();i++)
        newRig->Append(localConstList[i]->Clone());
    for(int i=0;i<actorList.size();i++)
        newRig->Append(actorList[i]->Clone());
    return newRig;
}

//=================================================================================================
// Добавление внешней ссылки в РИГ.
MemLocation Rig::Append(ExtRef* r) {
    long found=-1;
    for(long i=0;i<extRefList.size();i++)
        if(extRefList[i]->GetName()==r->GetName())
        {
            found=i;
            break;
        }
    if(found==-1)
    {
        if(!extRefList.size())
            SetName(r->GetName());
        extRefList.append(r);
        return MemLocation(ext,extRefList.size()-1);
    }
    return MemLocation(ext,found);
}

//=================================================================================================
// Добавление локальной константы в РИГ.
MemLocation Rig::Append(Value* v) {
    localConstList.append(v);
    return MemLocation(loc,localConstList.size()-1);
}

//=================================================================================================
// Добавление в РИГ оператора ФПП программы.
MemLocation Rig::Append(Actor* a) {
    actorList.append(a);
    return MemLocation(act,actorList.size()-1);
}
//=================================================================================================
// Очиска содержимого класса от накопившихся данных
void Rig::Clear() {
    // Удаление элементов, динамически подключенных к векторам.
    // При использовании QObject это может делаться автоматически.
    // Так пока и сделал
    // ...
    // Очистка векторов
    extRefList.clear();
    localConstList.clear();
    actorList.clear();
}


//=================================================================================================
// Тестовый вывод списка внешних ссылок
void Rig::TestOutExtRef(QTextStream& out) {
    out << "List of External References:" << endl;
    if(extRefList.empty()) {
        out << "Is Empty" << endl;
        return;
    }
    int size = extRefList.size();
    out << "size = " << size << endl;
    for(int i = 0; i < size; ++i) {
        out <<  i << ": ";
        extRefList[i]->TestOut(out);
    }
}

//=================================================================================================
// Тестовый вывод списка локальных констант функции
void Rig::TestOutLocalConst(QTextStream& out) {
    out << "List of Local Constants:" << endl;
    //if(size == 0) {
    if(localConstList.empty()) {
        out << "Is Empty" << endl;
        return;
    }
    int size = localConstList.size();
    out << "size = " << size << endl;
    for(int i = 0; i < size; ++i) {
        out <<  i << ": ";
        localConstList[i]->TestOut(out);
    }
}

//=================================================================================================
// Тестовый вывод списка операций РИГ
void Rig::TestOutActor(QTextStream& out) {
    out << "List of Actors:" << endl;
    if(actorList.empty()) {
        out << "Is Empty" << endl;
        return;
    }
    int size = actorList.size();
    out << "size = " << size << endl;
    for(int i = 0; i < size; ++i) {
        out <<  i << ": ";
        actorList[i]->TestOut(out);
    }
}


//=================================================================================================
// Тестовый вывод всех составляющих РИГ
void Rig::TestOut(QTextStream& out)
{
    // Тестовый вывод списка внешних ссылок
    TestOutExtRef(out);
    // Тестовый вывод списка локальных констант функции
    TestOutLocalConst(out);
    // Тестовый вывод списка операций РИГ
    TestOutActor(out);
}

QString Rig::GetAsStrExtRef()
{
    QString rez="List of External References:\n";
    if(extRefList.empty())
    {
        rez+="Is Empty\n";
        return rez;
    }
    int size = extRefList.size();
    rez+=QString("size = %1\n").arg(size);
    for(int i = 0; i < size; ++i) {
        rez+=QString("%1").arg(i);
        rez+='\t';
        rez+=extRefList[i]->GetAsStr();
        rez+='\n';
    }
    return rez;
}
QString Rig::GetAsStrLocalConst()
{
    QString rez="List of Local Constants:\n";
    if(localConstList.empty())
    {
        rez+="Is Empty\n";
        return rez;
    }
    int size = localConstList.size();
    rez+=QString("size = %1\n").arg(size);
    for(int i = 0; i < size; ++i) {
        rez+=QString("%1").arg(i);
        rez+='\t';
        rez+=localConstList[i]->GetAsStr();
        rez+='\n';
    }
    return rez;
}
QString Rig::GetAsStrActor()
{
    QString rez="List of Actors:\n";
    if(actorList.empty())
    {
        rez+="Is Empty\n";
        return rez;
    }
    int size = actorList.size();
    rez+=QString("size = %1\n").arg(size);
    for(int i = 0; i < size; ++i) {
        rez+=QString("%1").arg(i);
        rez+='\t';
        rez+=actorList[i]->GetAsStr();
        rez+='\n';
    }
    return rez;
}
QString Rig::GetAsStr()
{
    return GetAsStrExtRef()+GetAsStrLocalConst()+GetAsStrActor();
}

//=================================================================================================
//Получение текущей позиции
RigPos Rig::GetPos()
{
    RigPos result;
    result.externPos=extRefList.size();
    result.localPos=localConstList.size();
    result.actorPos=actorList.size();
    return result;
}

//=================================================================================================
// Откатка графа к сохраненной позиции
void Rig::Rollback(RigPos position)
{
    if(position.externPos>=0)   extRefList.erase(extRefList.begin()+position.externPos,extRefList.end());
    if(position.localPos>=0)    localConstList.erase(localConstList.begin()+position.localPos,localConstList.end());
    if(position.actorPos>=0)    actorList.erase(actorList.begin()+position.actorPos, actorList.end());
}
//=================================================================================================
// Геттер имени как char*
char* Rig::GetCharName()
{
    char *rez=new char[name.length()+100];
    QByteArray qb = name.toLocal8Bit();
    strcpy(rez,qb.data());
    return rez;
}
//=================================================================================================
// Генерация текстового представления
bool Rig::SaveText(QString fileName) {
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        // Формирование и вывод в файл внешних ссылок
        stream << "External" << endl;
        QVector<ExtRef*>* pExtRefList = GetExtRefList();
        for(int i = 0; i < pExtRefList->size(); i++) {
            stream << '\t' << i << '\t' << ((*pExtRefList)[i]->GetName()) << endl;
        }
        // Вывод в файл локальных констант
        stream << "Local" << endl;
        QVector<Value*>* pLocalConstList = GetlocalConstList();
        for(int i = 0; i < pLocalConstList->size(); i++) {
            stream << '\t' << i << '\t' << (*pLocalConstList)[i]->GetAsStr() << endl;
        }
        // Вывод узлов РИГ в файл
        stream << "id\tdelay\toperation\tlinks\t\tpositions" << endl;
        QVector<Actor*>* pActorList = GetActorList();
        for(int i = 0; i < pActorList->size(); i++) {
            Actor* pActor = (*pActorList)[i];
            stream << i << '\t' << pActor->GetDelay() << '\t' << pActor->GetAsStr() << endl;
        }
        file.close();
        return true;
    }
    return false;
}

//получаем РИГ одной строкой
void Rig::test1()
{
    name=extRefList[0]->GetName();
    cout << "\t\t\tName is " << qPrintable(name) << "\n";
    QString temp0=QString("\t%1\t%2\n").arg(0).arg(name);
    cout << "\t\t\ttemp0 = " << qPrintable(temp0);

}

QString Rig::GetTextRepresentation()
{
    QString result="";
    QString temp;
    QString name;
    result+="External\n";
    for(int i = 0; i < extRefList.size(); i++) {
        name=extRefList[i]->GetName();
        temp=QString("\t%1\t%2\n").arg(i).arg(name);
        result+=temp;
    }

    // Вывод в файл локальных констант
    result+="Local\n";
    QVector<Value*>* pLocalConstList = GetlocalConstList();
    for(int i = 0; i < pLocalConstList->size(); i++) {
        temp=QString("\t%1\t%2\n").arg(i).arg((*pLocalConstList)[i]->GetAsStr());
        result+=temp;
    }
    // Вывод узлов РИГ в файл
    result+="id\tdelay\toperation\tlinks\t\tpositions\n";
    QVector<Actor*>* pActorList = GetActorList();
    for(int i = 0; i < pActorList->size(); i++) {
        Actor* pActor = (*pActorList)[i];
        temp=QString("%1\t%2\t%3\n").arg(i).arg(pActor->GetDelay()).arg(pActor->GetAsStr());
        result+=temp;
    }
    return result;
}
// Добавление имени в таблицу имен РИГ
void Rig::AppendName(MemLocation id, QString name)
{
    int type=id.GetMemType();
    int index=id.GetIndex();
    if(nameTable[type].size()<=index)
        nameTable[type].resize(index+1);
    nameTable[type][index]=name;
}

//генерируем таблицу символов РИГ
QString Rig::GetSymbolTable()
{
    QString rezult="";

    rezult+="Extern\tName\n";
    int extCount=extRefList.size();
    if(nameTable[ext].size()<extCount)
        nameTable[ext].resize(extCount);
    for(int i=0;i<extCount;i++)
        rezult+=QString("%1\t%2\n").arg(i).arg(nameTable[ext][i]);

    rezult+="Local\tName\n";
    int localCount=localConstList.size();
    if(nameTable[loc].size()<localCount)
            nameTable[loc].resize(localCount);
    for(int i=0;i<localCount;i++)
        rezult+=QString("%1\t%2\n").arg(i).arg(nameTable[loc][i]);

    rezult+="Actor\tName\n";
    int actorCount=actorList.size();
    if(nameTable[act].size()<actorCount)
            nameTable[act].resize(actorCount);
    for(int i=0;i<actorCount;i++)
        rezult+=QString("%1\t%2\n").arg(i).arg(nameTable[act][i]);
    return rezult;
}
