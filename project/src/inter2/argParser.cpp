#include "argParser.h"

ArgParser::ArgParser()
{
}

//формируем Главный Аргумент
Value* ArgParser::FormFirstArgument()
{
    //парсим РИГ из специального аргументохранящего файла bin/arg.rig
    //вырезаем из этого РИГ собственно аргумент
    return SliceArgumentValue(ParseArgumentRig());
}

///парсим РИГ из специального аргументохранящего файла bin/arg.rig
//TODO CM4 нужен defensive coding. Неаккуратно.
Rig* ArgParser::ParseArgumentRig()
{
    RigParser *argParser;
    Rig* argRig;
    bool result;

    argRig=new Rig();
    argParser=new RigParser(argRig,QString("./arg.rig"));       //открываем файл
    result=argParser->Start();                                  //парсим РИГ
    delete argParser;

    qDebug() << "[AP]Result of argparsing is " << result;

    if(!result)
        return NULL;
    else
        return argRig;
}
//вырезаем из РИГ собственно аргумент, каковой являет собой последний имеющийся local-объект
Value* ArgParser::SliceArgumentValue(Rig* argRig)
{
    if(argRig==NULL)
        return NULL;
    else
    {
        QVector<Value*>* rigLocalConstList=argRig->GetlocalConstList();
        int argValueIndex=rigLocalConstList->size()-1;
        return (*rigLocalConstList)[argValueIndex];
    }
}
