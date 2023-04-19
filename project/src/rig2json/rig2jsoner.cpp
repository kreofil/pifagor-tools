#include "rig2jsoner.h"
#include <QJsonDocument>
#include <QFile>
#include <QJsonObject>

Rig2Jsoner::Rig2Jsoner(){}
Rig2Jsoner::~Rig2Jsoner(){}


//джейсоним одну связь
QJsonObject Rig2Jsoner::FormLink(MemLocation memLoc)
{
    QJsonObject result;
    result["type"]=memLoc.GetMemType();
    result["link"]=memLoc.GetIndex();
    return result;
}


//джейсоним список связей актора
QJsonArray Rig2Jsoner::FormLinks(Actor* actor)
{
    //{  datalistType, parlistType, aslistType, };
    QJsonArray result;
    ActorType type=actor->GetActorType();
    if(type==argType)
        ;
    if(type==interpType)
    {
        InterpActor* interpActor=(InterpActor*)actor;
        QJsonObject arg=FormLink(interpActor->arg);
        QJsonObject func=FormLink(interpActor->func);
        result.append(arg);
        result.append(func);
    }
    if(type==retType)
    {
        RetActor* retActor=(RetActor*)actor;
        QJsonObject arg=FormLink(retActor->arg);
        result.append(arg);
    }
    if ((type==datalistType)||(type==parlistType)||(type==aslistType))
    {
        ListActor* listActor=(ListActor*)actor;
        QList<MemLocation>::iterator iter;
        for(iter=listActor->memList.begin();iter!=listActor->memList.end();iter++)
        {
            QJsonObject elem=FormLink(*iter);
            result.append(elem);
        }
    }
    return result;
}


//джейсоним список акторов
QJsonArray Rig2Jsoner::FormNodes(Rig* rig)
{
    QJsonArray result;
    QJsonObject curActor;
    QVector<Actor*>* actors=rig->GetActorList();
    int cnt=actors->size();
    for(int i=0;i<cnt;i++)
    {
        curActor["id"]=i;
        curActor["type"]=(*actors)[i]->GetActorType();
        curActor["links"]=FormLinks((*actors)[i]);
        result.append(curActor);
    }
    return result;
}


//джейсоним список констант
QJsonArray Rig2Jsoner::FormConstants(Rig* rig)
{
    QJsonArray result;
    QJsonObject curValue;
    QVector<Value*>* values=rig->GetlocalConstList();
    int cnt=values->size();
    for(int i=0;i<cnt;i++)
    {
        curValue["id"]=i;
        curValue["type"]=(*values)[i]->GetValueType();
        curValue["value"]=(*values)[i]->GetAsStr();
        result.append(curValue);
    }
    return result;
}


//"коренная" функция джейсонинга РИГа
void Rig2Jsoner::Transform(Rig* rig)
{
    jsonRig["name"]=rig->GetName();
    jsonRig["constants"]=FormConstants(rig);
    jsonRig["nodes"]=FormNodes(rig);
}


//сохраняем джейсона в файл
void Rig2Jsoner::Save(QString filename)
{
    QFile saveFile(filename);
    if (!saveFile.open(QIODevice::WriteOnly)) {
           qWarning("Couldn't open save file.");
           return;
       }

    QJsonDocument saveDoc(jsonRig);
    saveFile.write(saveDoc.toJson());
}
