#ifndef RIG2DOT_H
#define RIG2DOT_H

#include <QtCore>
#include <QVector>
#include <QMap>
#include "../rig/rig.h"

class Rig2Dotter
{
    QMap<int, QVector<int>* > DelayActorDict;
    QMap<int, int> SpecDict; // Тип спецузла, номер спецузла
    QString dotText;    // Формирование dot файла
    QString extText;    // Формирование узлов для внешних ссылок
    QString valueText;  // Формирование локальных констант
    QString specText;   // Формирование специальных операций
    QString actorText;  // Формирование узлов РИГ
    QString linkText;
    QString tmpStr;
    int iSpecNum;
    InterpActor* interpActor;
    RetActor* retActor;
    MemLocation elem;

    QString Digraph(QString name);
    QString NodeParam();
    QString EdgeParam();
    QString ExtNode(QString s1, QString s2);
    QString ValueNode(QString s1, QString s2);
    QString SpecNode(QString s1, QString s2);
    QString ArgNode(QString s1);
    QString InterpNode(QString s1);
    QString DataListNode(QString s1);
    QString ParListNode(QString s1);
    QString AsynchListNode(QString s1);
    QString ReturnNode(QString s1);
    QString StartLink();
    QString Link(QString s1, QString s2, QString s3);
    QString DelayLink(QString s1, QString s2);
    QString DelayStr(int key, QString tmpStr);
    QString SelectMemLocationNode(MemLocation& memLocation);
    void CreateListLinks(ListActor* pListActor, int i);
    QString TransformStringOrChar(QString value);
    void PreInit();

public:
    Rig2Dotter();
    QString Rig2Dot (Rig* rig);
    bool SaveRigDot(Rig& rig, QString fileName);

};

#endif // RIG2DOT_H
