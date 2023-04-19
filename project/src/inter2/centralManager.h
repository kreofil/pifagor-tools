#ifndef CENTRAL_MANAGER_H
#define CENTRAL_MANAGER_H

#include <stdio.h>
#include "eventProcessor.h"
#include "../linker/linker.h"



class EventProcessor;

/* два вида задач:
задача-вопрос (выполни функцию Х с аргументом Y)
задача-ответ (верни результат выполнения функции X - фрагмент данных Z)
*/
enum CentralManagerTaskType {QUESTION, ANSWER,DELAY_OPEN,DELAY_RESULT};


/*задача для ЦМ*/
//TODO CM19 - подумать, не разнести ли Question/Answer структуры
struct CentralManagerTask
{
    CentralManagerTaskType taskType;        //тип задачи (вопрос/ответ)
    int taskID;                             //(порядковый номер, если вопрос) / (номер вопроса, если ответ)
    int taskSender;                         //(отправитель задачи (-1 - Внешний Мир), если вопрос) / (0 если ответ)
    Value *arg;                             //(входной аргумент, если вопрос) / (результат выполнения, если ответ)
    int funcID;                             //(id функции в таблице компоновки, если вопрос)/(0 если ответ)

    //два конструктора - пустой и полный
    CentralManagerTask() {;}
    CentralManagerTask(CentralManagerTaskType newTaskType, int newTaskID, int newTaskSender, Value* newArg, int newFuncID):
        taskType(newTaskType), taskID(newTaskID), taskSender(newTaskSender), arg(newArg), funcID(newFuncID) {;}
};

/*Досье СП*/
struct EventProcessorData
{
    int linkerMapPosition;                  //положение РИГ/УГ данного СП в карте компоновщика
    int taskID;                             //идентификатор задачи данного СП.

    EventProcessorData() {;}
    EventProcessorData(int newLinkerMapPosition, int newTaskID):
        linkerMapPosition(newLinkerMapPosition), taskID(newTaskID){;}
};

/*Центральный Менеджер*/
class CentralManager
{
    Linker myLinker;                                    //компоновщик (и, по совместительству, хранитель РИГов и УГов)
    Value* firstArg;                                    //Главный Аргумент (приходит в ЦМ извне)
    Value* finalRez;                                    //Главный Ответ (то, что посчитается по итогам работы всея ЦМ) TODO CM20 инкапсуляция
    QQueue<CentralManagerTask> tasks;                   //очередь ЦМ-ных задач
    QVector<EventProcessor*> eventProcessors;           //список имеющихся СП
    QVector<EventProcessorData*> eventProcessorDatas;   //список досье на СП

    //уникальные экземпляры автоматов
    InterpAutomat   *interpAutom;                       //автомат интерпретации
    DlistAutomat    *dlistAutom;                        //автомат списка данных
    PlistAutomat    *plistAutom;                        //автомат параллельного списка
    AlistAutomat    *alistAutom;                        //автомат асинхронного списка
    ArgAutomat      *argAutom;                          //автомат аргумента
    RetAutomat      *retAutom;                          //автомат вершины return

    void InitInnerData();                               //инициализация внутренних данных ЦМ
    void FormFirstTask();                               //создание Первого Задания

    void ProcessTask(CentralManagerTask task);                          //обрабатываем одну задачу
    void ProcessQuestionTask(CentralManagerTask question);              //обрабатываем задачу-вопрос
    void ProcessAnswerTask(CentralManagerTask answer);                  //обрабатываем задачу-ответ
    void ProcessDelayOpen(CentralManagerTask delayOpenTask);                //обрабатываем задачу-открой-задержанный-список
    void ProcessDelayResult(CentralManagerTask delayResultTask);              //обрабатываем задачу-задержанный-список-открыт

    EventProcessor* FormNewEventProcessor(CentralManagerTask task);     //создаем новый СП  

    int AddQuestionTask(int taskSender, Value *arg, int funcID);        //добавляем задачу-вопрос
    int AddAnswerTask(int taskID, Value *answer);                       //добавляем задачу-ответ
    int AddDelayOpenTask(Value* newArg, int EPID, int callerID);        //добавляем задачу-открой-задержанный-список
    int AddDelayResultTask(int taskID, Value *answer);                                           //добавляем задачу-задержанный-список-открыт


public:
    //для клиента
    CentralManager();                                   //пустой конструктор    
    ~CentralManager();
    bool Init(char *startFunc, Value* arg, QString repositoryName="default");             //инициализация ЦМ
    void Start();                                       //запускаем ЦМ
    Value* GetFinalRez();                               //возвращаем Главный Ответ

    //Для EventProcessor
    Automat* GetAutomat(autoType curAutoType);                          //возвращаем указатель на уникальный автомат нужного типа
    int CallExtern(int EPID, int externId, int externOwnerID, Value *arg);                 //интерфейс для СП, желающих задать новый вопрос
    int CallDelayOpen(Value* newArg, int EPID, int callerID);           //интерфейс для СП, желающих раскрыть чужой задержанный список
    int GetFuncUID(int EPID, int funcID);                               //получаем уникальный в рамках сессии ID функции
};


#endif

