#ifndef EVENT_PROCESSOR_H
#define EVENT_PROCESSOR_H

#include <stdio.h>
#include "../rig/rig.h"
#include "../cg/cg.h"
#include "dataWorker.h"
#include "centralManager.h"
#include "alu.h"

class CentralManager;
class DataWorker;
struct ExternTaskData
{
    int nodeNum;
    int posIndex;
    ExternTaskData() {;}
    ExternTaskData(int newNodeNum, int newPosIndex): nodeNum(newNodeNum), posIndex(newPosIndex) {;}
};

class EventProcessor: public QObject
{
    CentralManager *myCm;                   //указатель на центральный менеджер
    int linkerMapPosition;                  //положение РИГ/УГ данного СП в карте компоновщика
    int myID;                               //идентификатор данного СП.
    int taskID;                             //идентификатор задачи данного СП.
    Rig* myRig;                             //указатель на рабочий РИГ
    Cg* myCg;                               //указатель на рабочий УГ
    Value *arg;                             //указатель на аргумент функции
    QQueue<Signal*> signalQueue;            //очередь сигналов
    QVector<AutomatState> automatStates;    //слой состояний автоматов
    QVector<Automat*> automatLayer;         //автоматный слой
    bool *delayStatus;                      //состояния задержанных списков (true - открыт, false - закрыт)
    QQueue<Signal*>* delayQueues;           //очереди сигналов задержанных списков
    QVector<Link*>* dynLinks;               //динамические связи
    QVector<CtrlNode*> controlNodes;        //узлы УГ
    QVector<int> savedSignals;              //вектора сигналов, которые успела породить данная вершина УГ
    QVector<DynLink*> dynamicLinks;         //динамические связи УГ
    bool *blockedOutputCgNodes;             //маркеры временной заблокированности УГ-узлов
                                            //(не распространять сигналы по раскрытию задержанного списка)
    int *blockedInputCgNodes;               //метки "пропустить Х входных сигналов"
    QMap<int,ExternTaskData> answersPlaces; //позиции на места хранения поступающих

    int curStep;                            //номер текущего шага

    void PrintSignalQueue();                //печать очереди сигналов-сообщений
    void PrintAutomatLayer();               //печать автоматного слоя
    void InitSignalQueue();                 //инициализация очереди сигналов-сообщений
    void InitDelayQueues();                 //инициализация очередей задержанных списков
    void InitDynamicLinks();                //инициализация динамических связей
    void InitSavedSignals();
    void FormAutomatLayer();                //формирование автоматного слоя
    void InitAutomatStates();               //инициализация автоматных состояний

    void BroadcastByCgNode(int cgNodeNum, int count);
    void ExtendByCgNode(int cgNodeNum, int count);

public:
    DataWorker *myDw;

    //для CentralManager
    EventProcessor();                           //пустой конструктор
    EventProcessor(CentralManager *newCm, int thisEPID,
                   Rig *newRig, Cg *newCg, Value* newArg);
    ~EventProcessor();
                                                //основной конструктор
    void Init();
    int Start();                                //запуск процессора
    void SetAnswer(Value* answer, int taskID);  //
    void OpenDelayList(Value *arg);
    void OpenDelayList(int delNum, int iNode);  //раскрытие задержанного списка
    Value* GetDelayResult(Value* arg);          //интерфейс получения результата задержанного списка для CM


    //для DataWorker
    int GetID();
    void EnlargeDataLists(int index);           //увеличение ожидаемых всех списков данных, непосредственной связанных с данным узлом
    void BroadcastAndExtendByRigNode(int rigNodeNum, int count);
    void BroadcastByRigNode(int rigNodeNum, int count);
    void ExtendByRigNode(int rigNodeNum, int count);
    void BlockOutputCGNode(int nodeNum, bool blockFlag=true);
    void BlockInputCGNode(int nodeNum);
    void CallExtern(int externId,  int externOwnerID, Value *arg, int nodeNum, int posIndex);
    void CallDelayOpen(Value *arg, int EPID, int nodeNum, int posIndex);
    int GetFuncUID(int EPID, int funcID);                               //получаем уникальный в рамках сессии ID функции
};


#endif

