#ifndef ARGPARSER_H
#define ARGPARSER_H

#include "../rig/rig.h"
#include "../rigparser/rigparser.h"

/*минимодуль для выделения аргумента из bin/arg.rig
вырезания из него собственно аргумента
и возврата его клиентской части интерпретатора
ибо негоже этим ЦМ-у заниматься*/

class ArgParser
{
    static Rig* ParseArgumentRig();                            //парсим РИГ из специального аргументохранящего файла bin/arg.rig
    static Value* SliceArgumentValue(Rig* argRig);             //вырезаем из РИГ собственно аргумент, каковой являет собой последний имеющийся local-объект

public:
    ArgParser();
    static Value* FormFirstArgument();                         //формируем Главный Аргумент
};

#endif // ARGPARSER_H
