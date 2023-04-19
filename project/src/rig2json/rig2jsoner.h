#ifndef RIG2JSONER_H
#define RIG2JSONER_H
#include <QJsonObject>

#include "../rig/rig.h"


class Rig2Jsoner
{
    QJsonObject jsonRig;                        //основной json-объект
    QJsonArray FormNodes(Rig *rig);             //джейсоним список акторов
    QJsonArray FormLinks(Actor* actor);         //джейсоним список связей актора
    QJsonObject FormLink(MemLocation memLoc);   //джейсоним одну связь
    QJsonArray FormConstants(Rig* rig);         //джейсоним список констант

public:
    Rig2Jsoner();
    ~Rig2Jsoner();
    void Transform(Rig *rig);                   //главный джейсонер РИГа
    void Save(QString filename);                //сохраняем джейсона в файл
};

#endif // RIG2JSONER_H
