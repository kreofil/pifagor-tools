#ifndef DATAWORKER_H
#define DATAWORKER_H
#include "../rig/rig.h"
#include "eventProcessor.h"
#include "alu.h"

#define DW_NOTHING      0
#define DW_DELAYOPEN    1

class EventProcessor;
struct DataWorkerTask
{
    int nodeNum;
    int input;
    DataWorkerTask(){;}
    DataWorkerTask(int newNodeNum, int newInput): nodeNum(newNodeNum),input(newInput){;}
};
struct InterpActorQueues
{
    QQueue<int> argQueue;
    QQueue<int> funcQueue;
};

typedef QVector< QVector<bool> > DoneTable;

class DataWorker
{
    EventProcessor *myEp;
    Rig *myRig;
    Value *myArg;
    QQueue<DataWorkerTask> dataTasks;                   //задания для обработчика данных
    QVector<Value*> actorDataLayer;                     //слой данных (акторы)

    QVector<Value*> localConstList;
    QVector<Actor*> actorList;                          //вектор узлов-акторов

    //коллекция указателей на спецоперации
    SpecValue** specList;
    InterpActorQueues* interpActorQueuesList;
    DoneTable* doneTables;

    void CopyLocalConstList();
    void InitActorDataLayer();                          //формирование слоя данных для акторов
    void SetArgumentIntoDataLayer();

    void FormSpecList();
    void WorkOne();
    bool ExecuteTask(DataWorkerTask curTask);
    void ExecuteArgument();
    void ExecuteDataList(DataWorkerTask curTask);
    void ExecuteParList(DataWorkerTask curTask);
    void ExecuteAsyncList(DataWorkerTask curTask);
    void ExecuteReturn(DataWorkerTask curTask);
    void ExecuteInter(DataWorkerTask curTask);
    bool ProcessSinglePair(int argNum, int funcNum, int nodeNum);
    void ReserveInterpretAnswer(int nodeNum, int answerLen);    //резервируем место под ответ сложной интерпретации
    void ReserveDoneTable(int nodeNum, int argLen, int funcLen);             //резервируем место под таблицу пометок параллельных интерпретаций


    void FillInterpretQueue(Value *obj, QQueue<int> *queue);
    Value* GetFromLocation(MemLocation curLocation);    //получаем блок данных из нужного слоя
    Value* MakeDataList(int index);                     //собираем уже готовый список данных
    Value* AddToParList(DataWorkerTask currentTask);
    SpecValue* GetSpecOp(int x);
    int GetMemLocationMultiplicity(MemLocation curMemLocation);
    int GetActorMultiplicity(int actorIndex);
    int GetLocalMultiplicity(int localIndex);
    int GetMemLocationSize(MemLocation curMemLocation);
    int GetValueSize(Value* currentValue);


    bool CheckDoneTable(int nodeNum, int argUID, int funcUID);
    void SetDoneTable(int nodeNum, int argUID, int funcUID);
public:
    Value *rezult;                                      //результат
    DataWorker() {;};
    DataWorker(EventProcessor* newEp, Rig* newRig, Value* newArg=NULL);
    void Init();
    int GetLinksCount(int nodeIndex);
    void FillInterpretQueues(DataWorkerTask task);
    void AddTask(int taskNode, int taskInput);
    void Work();
    Value* GetDelayResult(Value* arg);
    void PrintActorDataLayer();                         //печать слоя данных для акторов
    void SetData(int nodeNum, Value *newData);
    bool SetData(int nodeNum, Value *newData, int newDataIndex, bool isFuncDataList=false);

    void CallExtern(int externId, int externOwnerID, Value *arg, int nodeNum, int posIndex);
    void SendExtraSignals(ParListValue* constant, int nodeNum);
    int GetFuncUID(int EPID, int funcID);                               //получаем уникальный в рамках сессии ID функции


};

#endif // DATAWORKER_H
