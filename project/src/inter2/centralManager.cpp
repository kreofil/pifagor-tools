#ifndef CENTRAL_MANAGER_CPP
#define CENTRAL_MANAGER_CPP

#include "centralManager.h"
#include "eventProcessor.h"
#include "../rigparser/rigparser.h"
#include "../cgparser/cgparser.h"





//конструктор:
CentralManager::CentralManager()
{
    //создаем уникальные экземпляры автоматов
    interpAutom=new InterpAutomat();
    dlistAutom=new DlistAutomat();
    plistAutom=new PlistAutomat();
    alistAutom=new AlistAutomat();
    argAutom=new ArgAutomat();
    retAutom=new RetAutomat();
}
CentralManager::~CentralManager()
{
    delete interpAutom;
    delete dlistAutom;
    delete plistAutom;
    delete alistAutom;
    delete argAutom;
    delete retAutom;

    delete finalRez;

    tasks.clear();                  //чистим очередь задач
    eventProcessors.clear();        //чистим список Событийных процессоров
    eventProcessorDatas.clear();    //чистим список данных событийных процессоров
}
//инициализация:
bool CentralManager::Init(char *startFunc, Value *arg, QString repositoryName)
{
    myLinker.setRepositoryName(repositoryName);
    //проводим линковку
    if(myLinker.Link(startFunc)==LINK_ERROR)
    {
            qDebug() << "Link failed!";
            return LINK_ERROR;
    }
    firstArg=arg;                                       //инициализируем Главный Аргумент (входной аргумент всея системы)
    finalRez=new DataListValue(QString("INITIAL NULL REZULT"));  //инициализируем Главный Результат (затычка текстом)
    InitInnerData();                                    //инициализируем внутренние данные Центрального Менеджера
    FormFirstTask();                                    //создаем первое задание (запуск первой функции с Главным Аргументом)
    return SUCCESS;
}
//инициализируем внутренние данные Центрального Менеджера:
void CentralManager::InitInnerData()
{
    tasks.clear();                  //чистим очередь задач
    for(int i=0;i<eventProcessors.size();i++)
        delete eventProcessors[i];
    eventProcessors.clear();        //чистим список Событийных процессоров

    eventProcessorDatas.clear();    //чистим список данных событийных процессоров
}





/*создаем Первое Задание
СП №"-1" (т.е., внешний мир) хочет получить результат выполнения нулевой функции с Главным Аргументом
 */
void CentralManager::FormFirstTask()    //TODO CM7 magic numbers. Некрасиво
{
    AddQuestionTask(-1,firstArg,0);
}

/*создаем задачу-вопрос
taskSender - вопрошающий СП
arg - аргумент к вопросу
funcID - id вызываемой функции
возвращаем номер вопроса в очереди задач
*/
int CentralManager::AddQuestionTask(int taskSender, Value* arg, int funcID)
{
    tasks.append(CentralManagerTask(QUESTION,tasks.size(),taskSender,arg,funcID));
    return tasks.size()-1;
}
/*создаем задачу-ответ
taskID - номер вопроса, на который отвечаем
answer - собственно, ответ
возвращаем номер вопроса(!) в очереди задач
*/
int CentralManager::AddAnswerTask(int taskID, Value* answer)
{
    tasks.append(CentralManagerTask(ANSWER,taskID,0,answer,0));
    return taskID;
}
/*создаем задачу-"откройте чужой задержанный список"
newArg - ярлычок открываемого списка
EPID - ID СП, которому принадлежит список (ЦМ не нужно знать, как устроен DelayValue, поэтому передаем отдельно
callerID - ID СП, который задал вопрос
возвращаем номер задачи в очереди задач
да, EPID содержится в newArg. Но он нужен ЦМ, а учить ЦМ разбирать Value - нехорошо
*/
int CentralManager::AddDelayOpenTask(Value* newArg, int EPID, int callerID)
{
    tasks.push_back(CentralManagerTask(DELAY_OPEN,tasks.size(),callerID,newArg,EPID));
    return tasks.size()-1;
}
/*создаем задачу-"чужой задержанный список открыт"
taskID - номер вопроса, на который отвечаем
answer - собственно ответ
возвращаем номер вопроса(!) в очереди задач
TODO CM8 а зачем мы вообще что-то возвращаем?
TODO CM9 а не переложить ли этот функционал на AddAnswerTask - больно уж они похожи
*/
int CentralManager::AddDelayResultTask(int taskID, Value* answer)
{
    CentralManagerTask question=tasks[taskID];
    tasks.push_back(CentralManagerTask(DELAY_RESULT,question.taskID,0,answer,0));
    return taskID;
}




//запускаем ЦМ
void CentralManager::Start()
{
    for(int i=0;i<tasks.size();i++) //попросту обрабатываем задачи, пока они есть
        ProcessTask(tasks[i]);
}
//обработка задачи:
void CentralManager::ProcessTask(CentralManagerTask task)
{
    //для разных типов задач - свои функции обработки
    //TODO CM10 это многоifие - редкое позорище. Нужно нормальное определение функции по типу.
    if(task.taskType==QUESTION)
        ProcessQuestionTask(task);
    if(task.taskType==ANSWER)
        ProcessAnswerTask(task);
    if(task.taskType==DELAY_OPEN)
        ProcessDelayOpen(task);
    if(task.taskType==DELAY_RESULT)
        ProcessDelayResult(task);
}
//обработка вопроса
void CentralManager::ProcessQuestionTask(CentralManagerTask question)
{
    qDebug() << "[CM] Processing question";
    EventProcessor* currentEP;
    int workResult;

    currentEP=FormNewEventProcessor(question);  //создаем новый СП
    //запускаем СП. считываем результат его работы (каковой появится, когда СП по какой-то причине остановится)
    workResult=currentEP->Start();                                  //TODO CM11 омерзительная синхронность. Надо думать

    if(workResult==1)                                               //если СП доработал до конца
        AddAnswerTask(question.taskID, currentEP->myDw->rezult);    //создаем задачу-ответ с полученным из СП значением
}
//обрабатываем ответ
void CentralManager::ProcessAnswerTask(CentralManagerTask answer)
{
    qDebug() << "[CM] Processing answer";
    int workResult;
    CentralManagerTask question=tasks[answer.taskID];           //а какой вопрос-то был?
    if(question.taskSender==-1)                                 //а не внешний ли мир ставил вопрос?
        finalRez=answer.arg;                                    //тогда у нас есть Главный Ответ
    else
    {
        eventProcessors[question.taskSender]->SetAnswer(answer.arg,answer.taskID);      //забиваем вопрос в того, кто его задал
        workResult=eventProcessors[question.taskSender]->Start();                       //возвращаем ему поток управления
                                                                                        //TODO CM12 омерзительная синхронность. Надо думать

        if(workResult==1)                                                               //СП отработал полностью
            AddAnswerTask(eventProcessorDatas[question.taskSender]->taskID,             //тогда у нас есть новый ответ
                          eventProcessors[question.taskSender]->myDw->rezult);
    }
}
void CentralManager::ProcessDelayOpen(CentralManagerTask delayOpenTask)
{
    qDebug() << "[CM] Processing delay_open";
    //ищем СП
    EventProcessor* delayKeeper=eventProcessors[delayOpenTask.funcID];
    //даем ему команду открыть список
    delayKeeper->OpenDelayList(delayOpenTask.arg);
    //возвращаем ему же поток управления
    //синхронно - потому что тупые - ждем результата
    //TODO CM13 синхронность зло
    delayKeeper->Start();
    //создаем задачу-задержанный-список-открыт
    //TODO CM14 пока исходим из того, что не открыться он не может. Но так ли это?
    //TODO CM15 да и "главного ответа СП" из вызванного СП мы таким образом не получим. Так ведь?
    Value* result=delayKeeper->GetDelayResult(delayOpenTask.arg);
    AddDelayResultTask(delayOpenTask.taskID,result);
}
void CentralManager::ProcessDelayResult(CentralManagerTask delayResultTask)
{
    qDebug() << "[CM] Processing delay_result";
    int workResult;
    CentralManagerTask question=tasks[delayResultTask.taskID];          //а какой вопрос-то был?
    EventProcessor* asker=eventProcessors[question.taskSender];

    asker->SetAnswer(delayResultTask.arg,delayResultTask.taskID);       //забиваем вопрос в того, кто его задал
    workResult=asker->Start();                                          //возвращаем ему поток управления

    //TODO CM16 омерзительная синхронность. Надо думать
    if(workResult==1)                                                               //СП отработал полностью
        AddAnswerTask(eventProcessorDatas[question.taskSender]->taskID,             //тогда у нас есть новый ответ
                      eventProcessors[question.taskSender]->myDw->rezult);
}



//создаем новый СП по входной задаче
EventProcessor* CentralManager::FormNewEventProcessor(CentralManagerTask task)
{
    Rig* currentRig;
    Value* currentArg;
    Cg* currentCg;
    EventProcessor* currentEP;
    EventProcessorData* currentEPData;

    currentRig=myLinker.rigsFound[task.funcID];     //ищем РИГ по иду функции в задаче
    currentCg=myLinker.cgsFound[task.funcID];       //ищем УГ по иду функции в задаче
    currentArg=task.arg;                            //аргумент и так в задаче
    currentEP=new EventProcessor(this,eventProcessors.size(),currentRig,currentCg,currentArg);  //создаем СП
    currentEP->Init();                                                                          //инициализируем СП
    eventProcessors.push_back(currentEP);                                                       //заносим СП в список СП

    currentEPData=new EventProcessorData(task.funcID,task.taskID);  //досье на СП - связанная функция, связанная задача
    eventProcessorDatas.push_back(currentEPData);                   //заносим досье в список досье

    return currentEP;                                               //возвращаем свежесозданный СП
}





//геттер автоматов. Такие вот у нас жестокие синглтоны
Automat* CentralManager::GetAutomat(autoType curAutoType)
{
    switch(curAutoType)
    {
        case AT_ARG:
            return argAutom;
            break;
        case AT_INTER:
            return interpAutom;
            break;
        case AT_DLIST:
            return dlistAutom;
            break;
        case AT_PLIST:
            return plistAutom;
            break;
        case AT_ALIST:
            return alistAutom;
            break;
        case AT_RET:
            return retAutom;
            break;
        default:
            return NULL;
            break;
    }
}





//интерфейс для СП, желающих задать новый вопрос
//TODO CM17 криво и синхронно. Надо думать.
int CentralManager::CallExtern(int EPID, int externId, int externOwnerID, Value *arg)
{
    qDebug() << "[CM] CallExtern - ownerID=" << externOwnerID << " funcID= " << externId;
    //ищем в таблице линковщика нужную функцию
    int linkerMapPosition=GetFuncUID(externOwnerID,externId);
    //собственно создаем вопрос. Вопрошавший получает номер созданной задачи-вопроса
    return AddQuestionTask(EPID,arg,linkerMapPosition);
}
//получаем уникальный в рамках сессии ID функции
int CentralManager::GetFuncUID(int EPID, int funcID)
{
    int linkerMapPosition=myLinker.GetMapNote(eventProcessorDatas[EPID]->linkerMapPosition,funcID);
    return linkerMapPosition;
}

//интерфейс для СП, желающих раскрыть чужой задержанный список
//TODO CM18 криво и синхронно. Надо думать.
int CentralManager::CallDelayOpen(Value* newArg, int EPID, int callerID)
{
    return AddDelayOpenTask(newArg,EPID,callerID);
}


//возвращаем Главный Ответ
Value* CentralManager::GetFinalRez()
{
    return finalRez;
}

#endif


