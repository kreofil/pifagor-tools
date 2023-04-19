#include "msgServer.h"

MsgServer* MsgServer::instance=NULL;
//получаем инстанс синглтона
MsgServer* MsgServer::getMsgServer()
{
    if(instance==NULL)
        instance=new MsgServer();
    return instance;
}

//конструктор
MsgServer::MsgServer()
{
    defaultNicety=true;
    listOfNicety.clear();
}

//включаем/выключаем генератор по имени
//name - имя включаемого/выключаемого
//on - включенность
void MsgServer::turnOne(QString name, bool on)
{
    listOfNicety[name]=on;
}

//включаем/выключаем все генераторы
//on - включенность
void MsgServer::turnAll(bool on)
{
    defaultNicety=on;
    QMap<QString,bool>::iterator iter;
    for(iter=listOfNicety.begin();iter!=listOfNicety.end();iter++)
        iter.value()=on;
}

//проверяем статус генератора по имени
//name - имя включаемого/выключаемого
bool MsgServer::checkNicety(QString name)
{
    if(listOfNicety.find(name)!=listOfNicety.end())
        return listOfNicety[name];
    return defaultNicety;
}
