#ifndef LINKER_H
#define LINKER_H

#include <QVector>
#include "../rig/rig.h"
#include "../cg/cg.h"
using namespace std;

/*/////////////////////////////////////////////////////////
//Структура для карты расположения функций в компоновщике//
/////////////////////////////////////////////////////////*/
struct mapNote
{
    int rigNum;     //номер РИГ
    int extNum;     //номер внешней ссылки РИГ
    int mapPos;     //местоположение связанной с этой ссылкой функции
};


/*/////////////
//Компоновщик//
/////////////*/
class Linker                                                //TODO L2 - больше map, меньше векторов
{
    QString repositoryName;
public:
    QVector<Rig*> rigsFound;                                //собранные РИГ
    QVector<Cg*>  cgsFound;                                 //собранные УГ
    QVector<mapNote> mapNotes;                              //карта внешних ссылок
    Linker();                                               //конструктор
    void setRepositoryName(QString newRepositoryName);
    int Link(char* name);                                   //главная функция компоновки
    void PrintAll();                                        //тестовая распечатка содержимого компоновщика
    void AddMapNote(int rigNum, int extNum, int mapPos);    //добавление записи в карту внешних ссылок
    int GetMapNote(int rigNum, int extNum);                 //поиск записи
};

#endif //LINKER_H
