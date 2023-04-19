/*==============================================================================
   File: trans2.cpp

   Description:
       Pifagor RIG translator 2.0

   Credits: AUTHORS

   License: GNU General Public License Version 3 or later
            http://www.gnu.org/licenses/gpl.html
==============================================================================*/

#include <locale>
#include "centralManager.h"
#include "argParser.h"
#include "../repository/msgServer.h"

using namespace std;

void myMessageEater(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{

}

/*========================================================
    Input arguments:
        <funcname>
========================================================*/

void inter2_arg_error()
{
    printf("\nWrong input format. Correct format:\n");
    printf("\t<funcname> [<repositoryName>]\n");
}
int main(int argc, char *argv[])
{
    setlocale(LC_CTYPE,"en_US.UTF-8");

    if (argc<2)
    {
        inter2_arg_error();
        return ARGNUM_ERROR;
    };
    QString repositoryName="default";
    if (argc==3)
    {
        repositoryName=QString(argv[2]);
    }



    MsgServer* msgServer=MsgServer::getMsgServer();
    msgServer->turnAll(false);
    msgServer->turnOne("linkMain",true);

    CentralManager *cm;
    cm=new CentralManager();

    qInstallMessageHandler(myMessageEater);

    bool initResult=cm->Init(argv[1],ArgParser::FormFirstArgument(),repositoryName);
    if(initResult!=SUCCESS)
    {
        delete cm;
        return initResult;
    }

    qInstallMessageHandler(0);

    cm->Start();
    qDebug() << "============================================";
    qDebug() << "Final result is " << cm->GetFinalRez()->GetAsStr();
    qDebug() << "============================================";
    printf("Final result: %s\n", cm->GetFinalRez()->GetAsStr().toUtf8().data());
    printf("success\n");

    delete cm;


    return SUCCESS;
}

