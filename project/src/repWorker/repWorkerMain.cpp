/*==============================================================================
   File: trans2.cpp

   Description:
       Pifagor RIG translator 2.0

   Credits: AUTHORS

   License: GNU General Public License Version 3 or later
            http://www.gnu.org/licenses/gpl.html
==============================================================================*/

#include <cstdlib>
#include <string.h>
#include <wchar.h>
#include <locale.h>

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#include <direct.h>
#endif

using namespace std;

#include "repWorker.h"

/*========================================================
    Input arguments:
            <dirKey> <typeKey> <filename> <funcname>
    where <dirKey> is;
        -in for saving data in repository
        -out for getting data out of repository
    where <typeKey> is;
        -rig for work with RIG
        -cg for work with CG
========================================================*/

void repWorkerArgError()
{
    QuickMsg msg(WRITE_PRINT,"repWorkerArgError");
    MsgServer* msgServer=MsgServer::getMsgServer();
    msgServer->turnOne("repWorkerArgError",true);

    msg.write("Wrong input format. Correct format:");
    msg.write("\t<dirKey> <typeKey> <filename> <funcname>");
    msg.write("where <dirKey> is");
    msg.write("\t-in for saving data in repository");
    msg.write("\t-out for getting data out of repository");
    msg.write("where <typeKey> is");
    msg.write("\t-rig for work with RIG");
    msg.write("\t-cg for work with CG");
    msg.write("\t-pfg for work with PFG");
}

int main(int argc, char *argv[]) {
    MsgServer* msgServer=MsgServer::getMsgServer();
    msgServer->turnAll(false);
    msgServer->turnOne("repWorker",true);

    QuickMsg msg(WRITE_PRINT,"repWorker");


    if (argc!=5)
    {
        repWorkerArgError();
        return ARG_ERROR;
    }

    QString dirKey=argv[1];                 //направление работы с репозиторием (чтение/запись)
    QString typeKey=argv[2];                //тип обрабатываемых данных (РИГ/УГ)
    QString funcName=argv[3];               //имя обрабатываемой функции
    QString fileName=argv[4];               //файл для приема/отдачи данных

    RepWorker repWorker;
    int result = repWorker.ProcessQuery(dirKey,typeKey,funcName,fileName);
    if (result==ARG_ERROR)
    {
        repWorkerArgError();
    }
    return result;
}

