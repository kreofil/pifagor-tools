#ifndef REPFACADE_H
#define REPFACADE_H

#include <../repository/nameMaker.h>

class RepFacade
{
    // формирователь имен файлов-записей
    NameMaker nameMaker;
    QMap<QString, QString> reposPaths;

    bool mainSaver(QString fileName, QString text);
    QString mainLoader(QString fileName);

    void dirPrepare(QString fileName);
public:
    RepFacade();
    void setMainRepository(QString key);

    static QString getCodeFragment(QString fileName, Positions posit);

    void init();
    bool saveRig(QString funcName, QString rigText);
    bool saveRigDot(QString funcName, QString rigDotText);
    bool saveCg(QString funcName, QString cgText);
    bool saveCgDot(QString funcName, QString cgDotText);
    bool saveSymbolTable(QString funcName, QString symTabText);
    bool saveCode(QString fileName, Positions posit, QString funcName);
    bool saveCode(QString funcName, QString codeFragment);

    QString getRigStr(QString funcName);
    QString getCgStr(QString funcName);
    QString getCode(QString funcName);
};

#endif // REPFACADE_H
