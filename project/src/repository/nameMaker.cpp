/*==============================================================================
   File: pfg_parser.cpp

   Description: 
        Pifagor programming language parser(syntax analyzer).
        Input: array of lexema, debug information.
        Output: information graph, debug information.

   Credits: AUTHORS

   License: GNU General Public License Version 3 or later
            http://www.gnu.org/licenses/gpl.html
==============================================================================*/

#ifndef NAME_MAKER_CPP
#define NAME_MAKER_CPP

#include "nameMaker.h"

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#include <direct.h>
#endif

NameMaker::NameMaker()
{

}

void NameMaker::setMainRepository(QString newMainRepository)
{
    mainRepository=newMainRepository;
}
QString NameMaker::getFileName(QString funcName)
{
    QString strFuncName=funcName;
    QString basePath;
    QString startName = funcName;
    QString fileName;

    strFuncName=strFuncName.left(strFuncName.indexOf("."));

    startName = funcName;
    basePath = mainRepository;
    fileName = basePath+startName;

    return fileName;
}

///////////////
//Name makers//
////////////////
QString NameMaker::getIgTxtName(QString funcName)
{
    QString fileName;
    QString basicPath = formBasicPath(funcName);
    fileName = basicPath+"/1.rig";
    return fileName;
}
QString NameMaker::getCgTxtName(QString funcName)
{
    QString fileName;
    QString basicPath = formBasicPath(funcName);
    fileName = basicPath+"/1.cg";
    return fileName;
}
QString NameMaker::getIgDotName(QString funcName)
{   
    QString fileName;
    QString basicPath = formBasicPath(funcName);
    fileName = basicPath+"/1.rdot";
    return fileName;
}
QString NameMaker::getCgDotName(QString funcName)
{
    QString fileName;
    QString basicPath = formBasicPath(funcName);
    fileName = basicPath+"/1.cdot";
    return fileName;
}
QString NameMaker::getCodeName(QString funcName)
{
    QString fileName;
    QString basicPath = formBasicPath(funcName);
    fileName = basicPath+"/1.pfg";
    return fileName;
}
QString NameMaker::getSymtabName(QString funcName)
{
    QString fileName;
    QString basicPath = formBasicPath(funcName);
    fileName = basicPath+"/1.symtab";
    return fileName;
}

QString NameMaker::formBasicPath(QString funcName)
{
    QString fileName=getFileName(funcName);
    fileName.replace('.','/');
    fileName.replace("//","./");
    QString fileNameWithPriority = fileName + "/00.00/";
    return fileNameWithPriority;
}
#endif



