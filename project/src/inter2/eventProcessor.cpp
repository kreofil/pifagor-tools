#ifndef EVENT_PROCESSOR_CPP
#define EVENT_PROCESSOR_CPP

#include "eventProcessor.h"
#include <iostream>
using namespace std;

//пустой конструктор
EventProcessor::EventProcessor()
{
    myRig=NULL;
    myCg=NULL;
}
//полный конструктор
EventProcessor::EventProcessor(CentralManager *newCm, int thisEPID, Rig *newRig, Cg *newCg, Value *newArg)
{
    //клонируем РИГ, чтоб не портить оригинальный. Остальное тупо сохраняем.
    myRig=newRig->Clone();
    myCg=newCg;
    myCm=newCm;
    myID=thisEPID;
    arg=newArg;   

    //получаем списки узлов и динамик.связей УГ - чтоб проще обращаться.
    controlNodes=*(myCg->GetCtrlNodeList());
    dynamicLinks=*(myCg->GetDynLinkList());
    int cgCtrlNodesCnt=controlNodes.size();     //сохраняем для пущей простоты количество узлов в УГ
    blockedOutputCgNodes=new bool[cgCtrlNodesCnt];    //список блокированных узлов УГ.
    blockedInputCgNodes=new int[cgCtrlNodesCnt];
    for(int i=0;i<cgCtrlNodesCnt;i++)           //пока что блокировок нет
    {
        blockedOutputCgNodes[i]=false;
        blockedInputCgNodes[i]=0;
    }
    myDw=new DataWorker(this,myRig,arg);        //создаем ДатаВоркера
}
EventProcessor::~EventProcessor()
{
    delete myRig;
    delete[] blockedOutputCgNodes;
    delete[] blockedInputCgNodes;
    delete myDw;
}

//возвращаем ID СП
int EventProcessor::GetID()
{
    return myID;
}


//инициализация всея СП
void EventProcessor::Init()
{
    curStep=0;

    //предварительный вывод данных по СП (функция, аргумент), РИГ, УГ
    qDebug() << "=========Event processor #" << myID << " started=========";
    if (arg!=NULL)
        qDebug() << qPrintable(myRig->GetName()) << "(" << qPrintable(arg->GetAsStr()) << ")";
    else
        qDebug() << qPrintable(myRig->GetName()) << "()";
    qDebug() << "=========MY RIG:=========";
    qDebug() << qPrintable(myRig->GetAsStr());
    qDebug() << "=========MY CG:=========";
    qDebug() << qPrintable(myCg->GetAsStr());

    //подготовка автоматной части
    FormAutomatLayer();
    InitAutomatStates();
    qDebug() << "=========Initial automat layer:=========";
    PrintAutomatLayer();

    //подготовка сигнальных частей
    InitSignalQueue();
    qDebug() << "=========Initial signal queue:=========";
    PrintSignalQueue();
    InitDelayQueues();
    InitDynamicLinks();
    InitSavedSignals();


    //подготовка ДатаВоркера
    qDebug() << "Initializing data worker";
    myDw->Init();
    qDebug() << "=========DW-ed signal queue:=========";
    PrintSignalQueue();
}
void EventProcessor::InitSignalQueue()
{
    //базовая очередь сигналов
    signalQueue.clear();
    QVector<Signal*> cgBase=*(myCg->GetSignalList());
    if(controlNodes[0]->GetAutomatType()==AT_ARG)
        signalQueue.append(new Signal(0,1));
    for(int i=0;i<cgBase.size();i++)
        signalQueue.append(cgBase[i]);
}
void EventProcessor::InitDelayQueues()
{
    //очереди и статусы задержанных списков
    int dynLinkCnt=dynamicLinks.size()+1;
    delayStatus=new bool[dynLinkCnt];
    delayStatus[0]=true;
    for(int i=1;i<dynLinkCnt;i++)
        delayStatus[i]=false;
    delayQueues=new QQueue<Signal*>[dynLinkCnt];
    for(int i=0;i<dynLinkCnt;i++)
        delayQueues[i].clear();
}
void EventProcessor::InitDynamicLinks()
{
    //вектор массивов динамических связей
    int cgCount=controlNodes.size();
    dynLinks=new QVector<Link*>[cgCount];
    for(int i=0;i<cgCount;i++)
        dynLinks[i].clear();
}
void EventProcessor::InitSavedSignals()
{
    int controlNodesCount=controlNodes.size();
    savedSignals.clear();
    savedSignals.resize(controlNodesCount);
    for(int i=0;i<controlNodesCount;i++)
        savedSignals[i]=0;
}

void EventProcessor::FormAutomatLayer()
{
    automatLayer.clear();
    for(int i=0;i<controlNodes.size();i++)
        automatLayer.push_back(myCm->GetAutomat(controlNodes[i]->GetAutomatType()));
}
void EventProcessor::InitAutomatStates()
{
    AutomatState *curState;
    automatStates.clear();
    for(int i=0;i<controlNodes.size();i++)
    {
        curState=new AutomatState();
        if(controlNodes[i]->GetAutomatType()==AT_DLIST)
            curState->dataListLength=myDw->GetLinksCount(controlNodes[i]->GetInode());
        automatStates.push_back(*curState);
    }
}
void EventProcessor::PrintAutomatLayer()
{
    qDebug() << "=========AutomatLayer:=========";
    for(int i=0;i<automatLayer.size();i++)
    {
        qDebug() << i << ' ' << automatLayer[i]->GetName() << " in state " << automatStates[i].state << "; data list length=" << automatStates[i].dataListLength;
    }
}
void EventProcessor::PrintSignalQueue()
{
    for(int i=0;i<signalQueue.size();i++)
        qDebug() << signalQueue[i]->GetAsStr();
}







int EventProcessor::Start()
{
    qDebug() << "=========Queue processing started:=========";
    Signal* curSignal;
    AutomatState curState;
    AutomatResult curResult;
    QVector<Link*> curLinks;

    int curNode=0;
    bool stopProcessing=false;
    int delayNum;

    //while(signalQueue.size() && !stopProcessing)
    while(signalQueue.size())
    {
        curStep++;
        qDebug() << endl << endl << endl;
        qDebug() << "=========Step" << myID << ":" << curStep << "=========";
        //qDebug() << myRig->GetAsStr();
        PrintSignalQueue();
        curSignal=signalQueue.dequeue();
        curNode=curSignal->GetNode();

        qDebug() << "Step #" << curStep << ": processing signal " << curSignal->GetAsStr();
        curState=automatStates[curNode];
        curResult=automatLayer[curNode]->ProcessSignal(curState, curSignal->GetInput());
        automatStates[curNode]=curResult.retState;
        if(controlNodes[curNode]->GetAutomatType()==AT_INTER)
        {
            DataWorkerTask curTask;
            curTask.input=curSignal->GetInput();
            curTask.nodeNum=controlNodes[curNode]->GetInode();
            myDw->FillInterpretQueues(curTask);
        }
        if(blockedInputCgNodes[curNode])
        {
            blockedInputCgNodes[curNode]--;
            continue;
        }
        switch(curResult.resultCode)
        {
            case AR_NOTHING:
                qDebug() << "Nothing important happened today";
                break;
            case AR_EXECUTE:
                qDebug() << "Execution time!";
                myDw->AddTask(controlNodes[curNode]->GetInode(),curSignal->GetInput());
                myDw->Work();
                if(automatLayer[curNode]->GetAutomatType()==AT_RET)
                    stopProcessing=true;

                qDebug() << "Generating new signals!";
                if(blockedOutputCgNodes[curNode]==true)                       //раскрылся задержанный список.
                {
                    qDebug() << "Signal generation blocked; skipping";
                    blockedOutputCgNodes[curNode]=false;
                }
                else                                                    //порождаем сигналы по обычной схеме
                {
                    savedSignals[curNode]++;
                    curLinks=*(controlNodes[curNode]->GetLinkList());
                    for(int i=0;i<curLinks.size();i++)
                    {
                        qDebug() << "Signal to node " << curLinks[i]->GetNode() << ", input " << curLinks[i]->GetInput();
                        delayNum=controlNodes[curLinks[i]->GetNode()]->GetDelNum();
                        if(delayStatus[delayNum])
                            signalQueue.push_back(new Signal(curLinks[i]->GetNode(),curLinks[i]->GetInput()));
                        else
                            delayQueues[delayNum].push_back(new Signal(curLinks[i]->GetNode(),curLinks[i]->GetInput()));
                    }
                    for(int i=0;i<dynLinks[curNode].size();i++)
                    {
                        qDebug() << "Signal to node " << dynLinks[curNode][i]->GetNode() << ", input " << dynLinks[curNode][i]->GetInput();
                        delayNum=controlNodes[dynLinks[curNode][i]->GetNode()]->GetDelNum();
                        if(delayStatus[delayNum])
                            signalQueue.push_back(new Signal(dynLinks[curNode][i]->GetNode(),dynLinks[curNode][i]->GetInput()));
                        else
                            delayQueues[delayNum].push_back(new Signal(dynLinks[curNode][i]->GetNode(),dynLinks[curNode][i]->GetInput()));
                    }
                }
                break;
            case AR_ASYNC_ADD:
                qDebug() << "Asynchronous addition time!";
                myDw->AddTask(controlNodes[curNode]->GetInode(),curSignal->GetInput());
                myDw->Work();
                break;
            default:
                qDebug() << "ERROR! This result code is not happening";
                break;
        }
    }
    PrintAutomatLayer();
    return stopProcessing;
}
//раскрытие задержанного списка по маркеру
//TODO EP1 следует ли СП понимать, как устроен DelayValue?
void EventProcessor::OpenDelayList(Value *arg)
{
    DelayValue* delayToOpen=(DelayValue*)arg;
    OpenDelayList(delayToOpen->GetDelayNumber(),-1);
}
//раскрытие задержанного списка по "координатам"
void EventProcessor::OpenDelayList(int delNum, int iNode)
{
    //помечаем список открытым
    delayStatus[delNum]=true;

    //сливаем очереди сигналов
    int delQueueSize=delayQueues[delNum].size();
    for(int i=0;i<delQueueSize;i++)
    {
        signalQueue.push_back(delayQueues[delNum].front());
        delayQueues[delNum].pop_front();
    }

    if (iNode==-1) // какие еще динамические связи - вызов-то извне!
        return;

    //создаем динамическую связь
    //выясняем по списку динамических связей, где у искомого задержанного списка корень
    int rootNum=-1;
    for(int i=0;i<dynamicLinks.size();i++)
        if(dynamicLinks[i]->GetDelNum()==delNum)
        {
            rootNum=dynamicLinks[i]->GetNode();
            break;
        }

    //по переданному номеру узла РИГ находим связанные с ним узлы УГ и копируем их связи корню.
    for(int i=0;i<controlNodes.size();i++)
        if(controlNodes[i]->GetInode()==iNode)
        {
            //ERROR?
            dynLinks[rootNum].push_back(new Link(i,0));
            BlockOutputCGNode(i);
            for(int j=0;j<savedSignals[rootNum];j++)
                signalQueue.push_back(new Signal(i,0));
        }
}
Value* EventProcessor::GetDelayResult(Value* arg)
{
    return myDw->GetDelayResult(arg);
}





void EventProcessor::BlockOutputCGNode(int nodeNum, bool blockFlag)     //теперь узел nodeNum пропустит ход генерации сигнала
{
    blockedOutputCgNodes[nodeNum]=blockFlag;
}
void EventProcessor::BlockInputCGNode(int nodeNum)     //теперь узел nodeNum пропустит обработку одного входного сигнала
{
    blockedInputCgNodes[nodeNum]++;
}

void EventProcessor::EnlargeDataLists(int index)
{
    qDebug() << "Enlarging data lists from " << index << endl;
    int linkListSize=controlNodes[index]->GetLinkListSize();
    QVector<Link*> linkList=*(controlNodes[index]->GetLinkList());
    for(int i=0;i<linkListSize;i++)
        if(controlNodes[linkList[i]->GetNode()]->GetAutomatType()==AT_DLIST)
        {
            qDebug() << "AutomatState " << linkList[i]->GetNode() << " data list length was " <<  automatStates[linkList[i]->GetNode()].dataListLength;
            automatStates[linkList[i]->GetNode()].dataListLength++;
            qDebug() << "AutomatState " << linkList[i]->GetNode() << " data list length is " <<  automatStates[linkList[i]->GetNode()].dataListLength;
        }
}
void EventProcessor::BroadcastAndExtendByRigNode(int rigNodeNum, int count)
{
    BroadcastByRigNode(rigNodeNum, count);
    ExtendByRigNode(rigNodeNum, count);
}
void EventProcessor::BroadcastByRigNode(int rigNodeNum, int count)
{
    qDebug() << "Broadcasting by rig node " << rigNodeNum;
    for(int i=0;i<controlNodes.size();i++)
        if(controlNodes[i]->GetInode()==rigNodeNum)
            BroadcastByCgNode(i,count);
}
void EventProcessor::ExtendByRigNode(int rigNodeNum, int count)
{
    qDebug() << "Extending by rig node " << rigNodeNum;
    for(int i=0;i<controlNodes.size();i++)
        if(controlNodes[i]->GetInode()==rigNodeNum)
            ExtendByCgNode(i,count);
}

void EventProcessor::BroadcastByCgNode(int cgNodeNum, int count)
{
    qDebug() << "Broadcasting from cg node " << cgNodeNum;
    if(blockedOutputCgNodes[cgNodeNum]==true)                       //раскрылся задержанный список.
    {
        qDebug() << "Signal generation blocked; skipping";
        //blockedOutputCgNodes[cgNodeNum]=false;
        return;
    }

    CtrlNode* cgNode=controlNodes[cgNodeNum];
    QVector<Link*> linkList=*(cgNode->GetLinkList());
    int linkListSize=linkList.size();
    for(int i=0;i<linkListSize;i++)
        for(int j=0;j<count;j++)
            signalQueue.append(new Signal(linkList[i]->GetNode(),linkList[i]->GetInput()));

    QVector<Link*> dynList=dynLinks[cgNodeNum];               //динамические связи
    int dynListSize=dynList.size();
    for(int i=0;i<dynListSize;i++)
        for(int j=0;j<count;j++)
            signalQueue.append(new Signal(dynList[i]->GetNode(),dynList[i]->GetInput()));
}
void EventProcessor::ExtendByCgNode(int cgNodeNum, int count)
{
    qDebug() << "Extending from cg node " << cgNodeNum;
    CtrlNode* cgNode=controlNodes[cgNodeNum];
    QVector<Link*> linkList=*(cgNode->GetLinkList());
    int linkListSize=linkList.size();
    int linkedNode;
    for(int i=0;i<linkListSize;i++)
    {
        linkedNode=linkList[i]->GetNode();
        if(controlNodes[linkedNode]->GetAutomatType()==AT_DLIST)
        {
            qDebug() << "Extending data list in " << linkedNode;
            (automatStates[linkedNode].dataListLength)+=count;
        }
    }
}


//вызываем внешнюю функцию (из ДВ в ЦМ)
void EventProcessor::CallExtern(int externId, int externOwnerID, Value *arg, int nodeNum, int posIndex)
{
    int taskID=myCm->CallExtern(myID,externId,externOwnerID, arg);
    answersPlaces[taskID]=ExternTaskData(nodeNum,posIndex);
}
//устанавливаем пришедший ответ
void EventProcessor::SetAnswer(Value *answer, int taskID)
{
    ExternTaskData externTaskData=answersPlaces[taskID];    //TODO EP2 - защиту от дураков
    qDebug() << "Setting answer. Before.";
    myDw->PrintActorDataLayer();
    myDw->SetData(externTaskData.nodeNum,answer,externTaskData.posIndex);
    qDebug() << "Setting answer. After";
    myDw->PrintActorDataLayer();
    for(int i=0;i<controlNodes.size();i++)                  //TODO EP3 исправить пересечение кода с BroadcastAndExtendByRigNode
        if(controlNodes[i]->GetInode()==externTaskData.nodeNum)
            BroadcastByCgNode(i,1);
    return;
}


//запрос на раскрытие чужого задержанного списка (из ДВ в ЦМ)
void EventProcessor::CallDelayOpen(Value* arg, int EPID, int nodeNum, int posIndex)
{
    int taskID=myCm->CallDelayOpen(arg,EPID,myID);
    answersPlaces[taskID]=ExternTaskData(nodeNum,posIndex);
}

//получаем уникальный в рамках сессии ID функции
//не очень аккуратная цепочка - и лишь для того, чтоб АЛУ не видело ЦМ напрямую
int EventProcessor::GetFuncUID(int EPID, int funcID)
{
    return myCm->GetFuncUID(EPID, funcID);
}
#endif



