#include "repWorker.h"



RepWorker::RepWorker()
{
    repFacade=new RepFacade;
    repFacade->init();
    msg=new QuickMsg(WRITE_PRINT,"repWorker");
}




QString RepWorker::GetFileContent(QString fileName)
{
    QString fileContent;
    QFile file(fileName);
    //считываем данные из входного файла
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        msg->write("Can't open input file! Error");
        return "";
    }
    QTextStream in(&file);
    fileContent=in.readAll();
    file.close();
    return fileContent;
}
int RepWorker::SetFileContent(QString fileName, QString fileContent)
{
    QFile file(fileName);
    //сохраняем полученные из репозитория файлы
    if(!file.open(QFile::WriteOnly | QFile::Truncate))
    {
        msg->write("Can't open output file! Error");
        return OUTPUTFILE_ERROR;
    }
    QTextStream out(&file);
    out << fileContent;
    file.close();
    return REPWORKER_SUCCESS;
}

int RepWorker::ProcessInRig(QString text, QString funcName)
{
    //пытаемся отпарсить считанное в РИГ
    Rig* rig;
    RigParser *rigParser;
    rig=new Rig;
    rigParser=new RigParser(rig,text,true);
    if(!(rigParser->Start()))
    {
        msg->write("RIG parse error!");
        return PARSE_ERROR;
    }

    //сравнение заявленного и реального имен функции
    if(funcName!=rig->GetCharName())
        msg->write("Argument funcname and name of func in file are not the same! Using argument func name.");
    repFacade->saveRig(funcName.toLocal8Bit().data(),text);
    return REPWORKER_SUCCESS;
}
int RepWorker::ProcessInCg(QString text, QString funcName)
{
    //пытаемся отпарсить считанное в УГ
    Cg* cg;
    CgParser *cgParser;
    cg=new Cg;
    cgParser=new CgParser(cg,text,true);
    if(!(cgParser->Start()))
    {
        msg->write("CG parse error!");
        return PARSE_ERROR;
    }

    //сравнение заявленного и реального имен функции
    if(funcName!=cg->GetCharName())
        msg->write("Argument funcname and name of func in file are not the same! Using argument func name.");
    repFacade->saveCg(funcName.toLocal8Bit().data(),text);
    return REPWORKER_SUCCESS;
}
int RepWorker::ProcessInPfg(QString text, QString funcName)
{
    //ничего не парсим, тупо сохраняем
    repFacade->saveCode(funcName.toLocal8Bit().data(),text);
    return REPWORKER_SUCCESS;
}
int RepWorker::ProcessInSymtab(QString text, QString funcName)
{
    repFacade->saveSymbolTable(funcName.toLocal8Bit().data(),text);
    return REPWORKER_SUCCESS;
}
int RepWorker::ProcessQuery(QString dirKey, QString typeKey, QString funcName, QString fileName)
{
    if(dirKey!="-in" && dirKey!="-out")     //непонятное направление
        return ARG_ERROR;
    if(typeKey!="-rig" && typeKey!="-cg" && typeKey!="-pfg" && typeKey!="-symtab")   //непонятный тип данных
        return ARG_ERROR;

    if(dirKey=="-in")
    {
        if (typeKey=="-rig")
            ProcessInRig(GetFileContent(fileName), funcName);
        if (typeKey=="-cg")
            ProcessInCg(GetFileContent(fileName), funcName);
        if (typeKey=="-pfg")
            ProcessInPfg(GetFileContent(fileName), funcName);
        if (typeKey=="-symtab")
            ProcessInSymtab(GetFileContent(fileName), funcName);
    }
    if(dirKey=="-out")
    {
        if (typeKey=="-rig")
            SetFileContent(fileName,repFacade->getRigStr(funcName.toLocal8Bit().data()));
        if (typeKey=="-cg")
            SetFileContent(fileName,repFacade->getCgStr(funcName.toLocal8Bit().data()));
        if (typeKey=="-pfg")
            SetFileContent(fileName,repFacade->getCode(funcName.toLocal8Bit().data()));
    }

    return REPWORKER_SUCCESS;
}
