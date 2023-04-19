/*==============================================================================
	File: repos.h

	Description: 

	Credits: 

	License: GNU General Public License Version 3 or later
			 http://www.gnu.org/licenses/gpl.html
==============================================================================*/

#ifndef NAME_MAKER_H
#define NAME_MAKER_H

#include <iostream>
#include <string>
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#else
  #include <sys/stat.h>
  #include <sys/types.h>
#endif

#include <QMap>

#include "../rig/rig.h"
#include "../trans2/pfg_scanner2.h"
#include "msgServer.h"
#include "quickMsg.h"

using namespace std;


/*------------------------------------------------------------------------------
  Class: NameMaker
        Пачка функций для формирования ключей объектов по именам функций
------------------------------------------------------------------------------*/
class NameMaker: public QObject
{
    QString formBasicPath(QString funcName);
    QString mainRepository;
public:
    NameMaker();
    QString getFileName(QString funcName);
    void setReposPaths(QMap<QString, QString>* reposPaths);
    void setMainRepository(QString newMainRepository);
    //void setReposPaths();
    QString getIgTxtName(QString funcName);
    QString getCgTxtName(QString funcName);
    QString getIgDotName(QString funcName);
    QString getCgDotName(QString funcName);
    QString getCodeName(QString funcName);
    QString getSymtabName(QString funcName);
};

#endif

