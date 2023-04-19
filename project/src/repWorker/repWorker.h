#ifndef REPWORKER_H
#define REPWORKER_H

#include <cstdlib>
#include <string.h>
#include <wchar.h>
#include <locale.h>

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#include <direct.h>
#endif

using namespace std;

#include <../repository/repFacade.h>
#include <../repository/msgServer.h>
#include <../repository/quickMsg.h>
#include <../repository/repFacade.h>
#include <../rigparser/rigparser.h>
#include <../cgparser/cgparser.h>



#define REPWORKER_SUCCESS 0
#define ARG_ERROR 1
#define INPUTFILE_ERROR 2
#define OUTPUTFILE_ERROR 3
#define PARSE_ERROR 4


class RepWorker
{
    RepFacade* repFacade;
    QuickMsg* msg;

    int ProcessInRig(QString text, QString funcName);
    int ProcessInCg(QString text, QString funcName);
    int ProcessInPfg(QString text, QString funcName);
    int ProcessInSymtab(QString text, QString funcName);

    QString GetFileContent(QString fileName);
    int SetFileContent(QString fileName, QString fileContent);
public:
    RepWorker();
    int ProcessQuery(QString dirkey, QString typekey, QString funcname, QString filename);
};



#endif // REPWORKER_H

