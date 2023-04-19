#include <QFile>
#include <QDir>


#include "repFacade.h"

RepFacade::RepFacade()
{
    init();
}

void RepFacade::init()
{
    QuickMsg msg(WRITE_PRINT,"linkMain");
    QString configName="./repository.ini";
    QFile config(configName);

    //считываем данные из входного файла
    if(!config.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        msg.write("Can't get repository.ini!");
        return;
    }
    QTextStream inConfig(&config);
    QString repName, repLink;
    while(!inConfig.atEnd())
    {
        inConfig >> repName >> repLink;
        reposPaths[repName]=repLink;
    }
    config.close();

    nameMaker.setMainRepository(reposPaths[QString("default")]);
}
void RepFacade::setMainRepository(QString key)
{
    QuickMsg msg(WRITE_PRINT,"linkMain");

    if (reposPaths.find(key)!=reposPaths.end())
    {
        nameMaker.setMainRepository(reposPaths[key]);
        msg.write("Repository " + key + " set successfully");
    }
    else
        msg.write("Can't set " + key + " repository");
}

void RepFacade::dirPrepare(QString fileName)
{
    QDir dir;
    QString curName;
    int slashIndex = fileName.lastIndexOf('/');       //TODO defensive
    curName = fileName.mid(2,slashIndex-2);
    dir.mkpath(curName);                              //TODO defensive
}

bool RepFacade::mainSaver(QString fileName, QString text)
{
    dirPrepare(fileName);
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out << text;
    file.close();
    return true;
}

QString RepFacade::mainLoader(QString fileName)
{   
    QString data;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return "";

    QTextStream in(&file);
    data=in.readAll();
    file.close();

    return data;
}
bool RepFacade::saveRig(QString funcName, QString rigText)
{
    return mainSaver(nameMaker.getIgTxtName(funcName),rigText);
}
bool RepFacade::saveRigDot(QString funcName, QString rigDotText)
{
    return mainSaver(nameMaker.getIgDotName(funcName),rigDotText);
}

bool RepFacade::saveCg(QString funcName, QString cgText)
{
    return mainSaver(nameMaker.getCgTxtName(funcName),cgText);
}
bool RepFacade::saveCgDot(QString funcName, QString cgDotText)
{
    return mainSaver(nameMaker.getCgDotName(funcName),cgDotText);
}

bool RepFacade::saveSymbolTable(QString funcName, QString symTabText)
{
    return mainSaver(nameMaker.getSymtabName(funcName),symTabText);
}
bool RepFacade::saveCode(QString fileName, Positions posit, QString funcName)
{
    QString codeFragment=getCodeFragment(fileName,posit);
    return mainSaver(nameMaker.getCodeName(funcName),codeFragment);
}
bool RepFacade::saveCode(QString funcName, QString codeFragment)
{
    return mainSaver(nameMaker.getCodeName(funcName),codeFragment);
}


QString RepFacade::getRigStr(QString funcName)
{
    return mainLoader(nameMaker.getIgTxtName(funcName));
}
QString RepFacade::getCgStr(QString funcName)
{
    return mainLoader(nameMaker.getCgTxtName(funcName));
}
QString RepFacade::getCode(QString funcName)
{
    return mainLoader(nameMaker.getCodeName(funcName));
}



QString RepFacade::getCodeFragment(QString fileName, Positions posit)
{
    //TODO lots of defensive
    int row=1, col=1;
    char tempChar;
    bool foundNotEmpty=false;
    QFile file(fileName);
    file.open(QIODevice::ReadOnly | QIODevice::Text);   //TODO defensive
    QTextStream fileStream(&file);

    QString rezult="";
    while(row<posit.GetBeginRow())
    {
        fileStream.readLine();
        row++;
    }
    while(col<posit.GetBeginCol())
    {
        fileStream >> tempChar;
        col++;
    }

    while(true)
    {
        fileStream >> tempChar;
        if ((tempChar!='\n')&&(tempChar!='\t')&&(tempChar!='\t')&&(tempChar!=' '))
            foundNotEmpty=true;
        if(foundNotEmpty)
            rezult+=tempChar;

        if(tempChar=='\n')
        {
            row++;
            col=1;
        }
        else
        {
            if(tempChar=='\t') col+=TAB_SIZE;
            col++;
        }
        if((row==posit.GetEndRow() && col>posit.GetEndCol())||row>posit.GetEndRow())
            break;
    }
    file.close();
    return rezult;
}
