#include <QMap>
#include <QString>

#ifndef MSGSERVER_H
#define MSGSERVER_H

// сервер обработки сообщений
class MsgServer
{
    QMap<QString,bool> listOfNicety;    //открытость/закрытость генераторов сообщений по именам
    bool defaultNicety;                 //открытость/закрытость по умолчанию
    MsgServer();
    static MsgServer* instance;         //инстанс синглтона
public:
    static MsgServer* getMsgServer();   //получаем инстанс
    void turnAll(bool on);              //массовый вкл/выкл
    void turnOne(QString name,bool on); //единичный вкл/выкл по имени
    bool checkNicety(QString name);     //проверка статуса конкретного генератора сообщений
};


#endif // MSGSERVER_H
