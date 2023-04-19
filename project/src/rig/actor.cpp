#include "value.h"
#include "actor.h"

//=================================================================================================
// Тестовый вывод позиций
void Positions::TestOut(QTextStream& out) {
    out << "pos=(" << pos[0] << "," << pos[1] << "),(" << pos[2] << "," << pos[3] << ")";
}


//=================================================================================================
// Конструктор базового класса
Actor::Actor() {


}

//=================================================================================================
// Получение текстового представления начального актора
QString BegActor::GetAsStr() {
    QString str;
    str = "arg\t\t\t\tpos";
    for(int i = 0; i < 4; i++) {
        str += " " + QString::number(pos.GetPos(i));
    }
    return str;
}

//=================================================================================================
// Тестовый вывод начального актора
void BegActor::TestOut(QTextStream& out) {
    out << "{" << GetDelay() << "} Beginning Actor    ";
    pos.TestOut(out);
    out << endl;
}
//=================================================================================================
// Клонировщик
Actor* BegActor::Clone()
{
    BegActor* newBegActor=new BegActor();
    newBegActor->SetDelay(GetDelay());
    newBegActor->pos=Positions(pos);
    return newBegActor;
}
//=================================================================================================
// Получение текстового представления памяти
QString MemLocation::GetAsStr() {
    QString str;

    switch(type) {
    case act:
        str = QString::number(index);
        break;
    case ext:
        str = "ext:" + QString::number(index);
        break;
    case loc:
        str = "loc:" + QString::number(index);
        break;
    case spec:
        str = ::GetAsStr(static_cast<SpecType>(index));
        break;
    default:
        str = "bad_type:";
        break;
    }
    return str;
}

//=================================================================================================
// Тестовый вывод параметров местоположения
void MemLocation::TestOut(QTextStream& out) {
    switch(type) {
    case act:
        out << "act:";
        break;
    case ext:
        out << "ext:";
        break;
    case loc:
        out << "loc:";
        break;
    case spec:
        //out << "spec:";
        //break;
        out << ::GetAsStr(static_cast<SpecType>(index));
        return;
    default:
        out << "bad type:";
        break;
    }
    out << index;
}
//=================================================================================================
// А не спек ли я пар-списка?
bool MemLocation::isSpecParList()
{
    if(type==spec)
        if((SpecType)index==palistSpec)
            return true;
    return false;
}

//=================================================================================================
//
InterpActor::InterpActor(MemType argType, int argIndex, MemType funcType, int funcIndex)
{	
    actorType = interpType;
    arg.Set(argType, argIndex);
    func.Set(funcType, funcIndex);
}
InterpActor::InterpActor(MemLocation argLoc, MemLocation funcLoc)
{ 
    actorType = interpType;
    arg.Set(argLoc.GetMemType(),argLoc.GetIndex());
    func.Set(funcLoc.GetMemType(),funcLoc.GetIndex());
}

//=================================================================================================
// Получение текстового представления операции интерпретации
QString InterpActor::GetAsStr() {
    QString str = ":\t\t" + arg.GetAsStr() + " " + func.GetAsStr() + "\t\tpos";
    for(int i = 0; i < 4; i++) {
        str += " " + QString::number(pos.GetPos(i));
    }
    return str;
}

//=================================================================================================
// Тестовый вывод операции интерпретации
void InterpActor::TestOut(QTextStream& out) {
    out << "{" << GetDelay() << "} Interpretating Actor. Arg location = ";
    arg.TestOut(out);
    out << " Func location = ";
    func.TestOut(out);
    out << "  ";
    pos.TestOut(out);
    out << endl;
}
//=================================================================================================
// Клонировщик
Actor* InterpActor::Clone()
{
    InterpActor* newInterpActor=new InterpActor(arg,func);
    newInterpActor->SetDelay(GetDelay());
    newInterpActor->pos=Positions(pos);
    return newInterpActor;
}
//=================================================================================================
// Преобразование в строку списка ссылок на аргументы
QString ListActor::RefListAsStr() {
    QString str;
    QList<MemLocation>::const_iterator memIter;
    int i = 1;
    for (memIter = memList.constBegin(); memIter != memList.constEnd(); ++memIter) {
        MemLocation mem = *memIter;
        str += mem.GetAsStr();
        if(i < memList.size()) str += " ";
        i++;
    }
    return str;
}
//=================================================================================================
// Добавление элемента в список
void ListActor::AddToList(MemType elemType, int elemIndex)
{
    MemLocation newLoc;
    newLoc.Set(elemType, elemIndex);
    memList.append(newLoc);
}
void ListActor::AddToList(MemLocation newEl)
{
    memList.append(MemLocation(newEl.GetMemType(),newEl.GetIndex()));
}

//=================================================================================================
// Получение текстового представления операции группировки в список данных
QString DataListActor::GetAsStr() {
    QString str = "(---)\t\t" + RefListAsStr() + "\t\tpos";
    for(int i = 0; i < 4; i++) {
        str += " " + QString::number(pos.GetPos(i));
    }
    return str;
}

//=================================================================================================
// Тестовый вывод операции группировки в список данных
void DataListActor::TestOut(QTextStream& out) {
    out << "{" << GetDelay() << "} Data List Actor. Arguments = ";
    QList<MemLocation>::const_iterator memIter;
    for (memIter = memList.constBegin(); memIter != memList.constEnd(); ++memIter) {
        MemLocation mem = *memIter;
        mem.TestOut(out);
        out << "   ";
    }
    pos.TestOut(out);
    out << endl;
}
//=================================================================================================
// Клонировщик
Actor* DataListActor::Clone()
{
    DataListActor* newDataListActor = new DataListActor();
    for(int i=0;i<memList.size();i++)
        newDataListActor->AddToList(memList[i]);
    newDataListActor->SetDelay(GetDelay());
    newDataListActor->pos=Positions(pos);
    return newDataListActor;
}

//=================================================================================================
// Получение текстового представления операции группировки в параллельный список
QString ParListActor::GetAsStr() {
    QString str = "[---]\t\t" + RefListAsStr() + "\t\tpos";
    for(int i = 0; i < 4; i++) {
        str += " " + QString::number(pos.GetPos(i));
    }
    return str;
}

//=================================================================================================
// Тестовый вывод операции группировки в параллельный список
void ParListActor::TestOut(QTextStream& out) {
    out << "{" << GetDelay() << "} Parallel List Actor. Arguments = ";
    QList<MemLocation>::const_iterator memIter;
    for (memIter = memList.constBegin(); memIter != memList.constEnd(); ++memIter) {
        MemLocation mem = *memIter;
        mem.TestOut(out);
        out << "   ";
    }
    pos.TestOut(out);
    out << endl;
}
//=================================================================================================
// Клонировщик
Actor* ParListActor::Clone()
{
    ParListActor* newParListActor = new ParListActor();
    for(int i=0;i<memList.size();i++)
        newParListActor->AddToList(memList[i]);
    newParListActor->SetDelay(GetDelay());
    newParListActor->pos=Positions(pos);
    return newParListActor;
}
//=================================================================================================
// Получение текстового представления операции группировки в асинхронный список
QString AsListActor::GetAsStr() {
    QString str = "<(--)\t\t" + RefListAsStr() + "\t\tpos";
    for(int i = 0; i < 4; i++) {
        str += " " + QString::number(pos.GetPos(i));
    }
    return str;
}

//=================================================================================================
// Тестовый вывод операции группировки в асинхронный список
void AsListActor::TestOut(QTextStream& out) {
    out << "{" << GetDelay() << "} Asynchronous List Actor. Arguments = ";
    QList<MemLocation>::const_iterator memIter;
    for (memIter = memList.constBegin(); memIter != memList.constEnd(); ++memIter) {
        MemLocation mem = *memIter;
        mem.TestOut(out);
        out << "   ";
    }
    pos.TestOut(out);
    out << endl;
}
//=================================================================================================
// Клонировщик
Actor* AsListActor::Clone()
{
    AsListActor* newAsListActor = new AsListActor();
    for(int i=0;i<memList.size();i++)
        newAsListActor->AddToList(memList[i]);
    newAsListActor->SetDelay(GetDelay());
    newAsListActor->pos=Positions(pos);
    return newAsListActor;
}
//=================================================================================================
//
RetActor::RetActor(MemType argType, int argIndex) {
    actorType = retType;
    arg.Set(argType, argIndex);
    SetDelay(0);
}
//=================================================================================================
//
RetActor::RetActor(MemLocation newArg) {
    actorType = retType;
    arg.Set(newArg.GetMemType(), newArg.GetIndex());
    SetDelay(0);
}

//=================================================================================================
// Получение текстового представления операции возврата
QString RetActor::GetAsStr() {
    QString str = "return\t\t" + arg.GetAsStr() + "\t\tpos";
    for(int i = 0; i < 4; i++) {
        str += " " + QString::number(pos.GetPos(i));
    }
    return str;
}

//=================================================================================================
// Тестовый вывод операции возрата
void RetActor::TestOut(QTextStream& out) {
    out << "{" << GetDelay() << "} Return Actor. Arg location = ";
    arg.TestOut(out);
    out << "  ";
    pos.TestOut(out);
    out << endl;
}

//=================================================================================================
// Клонировщик
Actor* RetActor::Clone()
{
    RetActor* newRetActor=new RetActor(arg);
    newRetActor->SetDelay(GetDelay());
    newRetActor->pos=Positions(pos);
    return newRetActor;
}
