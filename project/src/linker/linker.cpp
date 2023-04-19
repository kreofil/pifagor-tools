#include <iostream>
using namespace std;

#include "../repository/repFacade.h"
#include "../repository/quickMsg.h"
#include "../rigparser/rigparser.h"
#include "../cgparser/cgparser.h"
#include "linker.h"

/*/////////////
//Конструктор//
/////////////*/
Linker::Linker()
{
    rigsFound.clear();
    cgsFound.clear();
}
void Linker::setRepositoryName(QString newRepositoryName)
{
    repositoryName=newRepositoryName;
}

/*////////////////////////////
//Главная функция компоновки//
////////////////////////////*/
int Linker::Link(char* funcName)
{
    RepFacade* repFacade=new RepFacade();
    repFacade->setMainRepository(repositoryName);

    QuickMsg msg(WRITE_PRINT,"linkMain");

    Rig *rig;
    Cg *cg;
    RigParser *rigParser;
    CgParser *cgParser;
    int curPos=-1;
    int childPos;

    rig=new Rig;
    cg=new Cg;

    //Компоновщик уже обработал эту функцию?
    for(int i=0;i<rigsFound.size();i++)
        if(rigsFound[i]->GetName()==QString(funcName))
            return i;

    //Считываем РИГ
    QString rigText=repFacade->getRigStr(funcName);
    if(rigText=="")
    {
        msg.write("Can't get RIG of function "+QString(funcName));
        return LINK_ERROR;
    }
    rigParser=new RigParser(rig,rigText,true);

    if(!(rigParser->Start()))
    {
        msg.write("Link error during proccesing RIG of function "+QString(funcName));
        return LINK_ERROR;
    }
    curPos=rigsFound.size();
    rigsFound.append(rig);

    //Считываем УГ
    QString cgText=repFacade->getCgStr(funcName);
    if(cgText=="")
    {
        msg.write("Can't get CG of function "+QString(funcName));
        return LINK_ERROR;
    }
    cgParser=new CgParser(cg,cgText,true);

    if(!(cgParser->Start()))
    {
        qDebug() << "Link error during proccesing CG of function "+QString(funcName);
        return LINK_ERROR;
    }
    cgsFound.append(cg);

    QVector<ExtRef*> curExtRefVect=*(rig->GetExtRefList());
    for(int i=0;i<curExtRefVect.size();i++)
    {
        //Проделываем компоновку для всех внешних ссылок
        childPos=Link(curExtRefVect[i]->GetName().toUtf8().data());
        if(childPos==LINK_ERROR)
            return LINK_ERROR;
        //Формируем карту внешних ссылок
        AddMapNote(curPos,i,childPos);
    }

    return curPos;
}
/*//////////////////////////////////////////////
//Тестовая распечатка содержимого компоновщика//
//////////////////////////////////////////////*/
void Linker::PrintAll()
{
    //Печать РИГ
    for(int i=0;i<rigsFound.size();i++)
    {
        rigsFound[i]->SaveText(QString("./out/link/%1.rig").arg(i));
        cout << "Rig #" << i << " " << qPrintable(rigsFound[i]->GetName()) << "\n";
    }
    //Печать УГ
    for(int i=0;i<cgsFound.size();i++)
    {
        cgsFound[i]->SaveText(QString("./out/link/%1.cg").arg(i));
        cout << "Cg #" << i << " " << qPrintable(cgsFound[i]->GetName()) << "\n";
    }
    //Печать карты внешних ссылок
    for(int i=0;i<mapNotes.size();i++)
        cout << "Rig #"<< mapNotes[i].rigNum << "\'s ext#" << mapNotes[i].extNum << " is "  << mapNotes[i].mapPos << endl;
}
/*/////////////////////////////////////
//Новая запись в карту внешних ссылок//
/////////////////////////////////////*/
void Linker::AddMapNote(int rigNum, int extNum, int mapPos)
{
    mapNote curNote;
    curNote.rigNum=rigNum;
    curNote.extNum=extNum;
    curNote.mapPos=mapPos;
    mapNotes.append(curNote);
}
/////////////////////////////
// поиск конкретной записи //
/////////////////////////////
int Linker::GetMapNote(int rigNum, int extNum)  // TODO L1 сделать нормальную мапу
{
    for(int i=0;i<mapNotes.size();i++)
        if(mapNotes[i].rigNum==rigNum && mapNotes[i].extNum==extNum)
            return mapNotes[i].mapPos;
    return -1;
}
