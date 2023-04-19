#ifndef _QUICKMSG_H_
#define _QUICKMSG_H_

#include "../repository/const.h"
//#include <QMessageBox>
//#include <QTextCodec>
#include <QTextStream>
#include <QString>

class QuickMsg
{
public:
    QuickMsg(enmMsg enmType, QString strName);                    // конструктор
    void init(QString strText);                                   // инициализация текущей функции, процедуры,..
    void write(QString strText, int intErr, int intErl);          // вывод сообщения об ошибке
    void write(QString strText);                                  // вывод информационного сообщения
    bool isNotNothing(QVariant qVar, int intTypeMsg,              // проверка на пустоту
        const char *strMsg);
    bool isNotNothing(QString strText, int intTypeMsg,
        const char *strMsg);
    bool isNotNothing(int intVar, int intTypeMsg,
        const char *strMsg);
    bool isNotNothing(double dblVar, int intTypeMsg,
        const char *strMsg);
    bool isNotNothing(bool blnVar, int intTypeMsg,
        const char *strMsg);
    bool isNotNothing(QString strText, int intTypeMsg,
        QString strMsg);
private:
    enmMsg enmType;                                               // тип вывода (WRITE_MSG, WRITE_MSG_SILENT, WRITE_PRINT)
    QString strMacro;                                             // название модуля (библиотеки, класса, кнопки,..)
    QString strMicro;                                             // название функции или процедуры
    QString replaceSubString(QString strIn, QString strBad,       // замена подстроки в строке
                             QString strGood);
    bool writeSys(QString strText, QString strSys);               // вывод сообщения
    int showDialog(enmButton enmButtonSet, QString strTitle,      // отображение диалогового окна, там где это допустимо
                   QString strText, QString strDetail);
    bool isOnServer();                                            // код выполняется на сервере (нет поддержки UI)?
};

#endif // _QUICKMSG_H_
