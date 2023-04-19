#ifndef DATAWORKER_CPP
#define DATAWORKER_CPP
#include "dataWorker.h"

DataWorker::DataWorker(EventProcessor* newEp, Rig* newRig,Value* newArg)
{

    myEp=newEp;
    myRig=newRig;
    myArg=newArg;
    dataTasks.clear();
    actorList=*(myRig->GetActorList());
    CopyLocalConstList();

    interpActorQueuesList=new InterpActorQueues[actorList.size()];
    doneTables=new DoneTable[actorList.size()];
    rezult=NULL;
}

void DataWorker::CopyLocalConstList()
{
    //localConstList=*(myRig->GetlocalConstList());
    QVector<Value*> rigLocalConstList;
    Value* temp;
    DelayValue* delayTemp;
    rigLocalConstList=*(myRig->GetlocalConstList());
    for(int i=0;i<rigLocalConstList.size();i++)
    {
        temp=rigLocalConstList[i]->Clone();
        if(temp->GetValueType()==DelayValueType)
        {
            delayTemp=(DelayValue*)temp;
            delayTemp->SetEPID(myEp->GetID());
        }
        localConstList.push_back(temp);
    }

}

//инициализируем DataWorker
void DataWorker::Init()
{
    InitActorDataLayer();
    qDebug() << "=========Initial actor data layer:=========";
    PrintActorDataLayer();
    FormSpecList();
}
//инициализируем слой данных
void DataWorker::InitActorDataLayer()
{
    for(int i=0;i<actorList.size();i++)
    {
        //для параллельного списка заранее резервируем число элементов.
        if (actorList[i]->GetActorType()==parlistType)
        {
            ParListValue* emptyParList = new ParListValue();
            ParListActor* parListActor = (ParListActor*)actorList[i];
            int linkSize=parListActor->memList.size();
            for(int i=1;i<=linkSize;i++)
                emptyParList->SetElement(new NullValue(),i);
            actorDataLayer.push_back(emptyParList);
        }
        else
            actorDataLayer.push_back(NULL);
    }
    if((actorList[0]->GetActorType())==argType)
    {
        SetArgumentIntoDataLayer();
    }
}
//устанавливаем аргумент в слой данных
void DataWorker::SetArgumentIntoDataLayer()
{
    qDebug() << "Found arg position";
    actorDataLayer[0]=myArg;
    if(myArg==NULL)
        return;
    if(myArg->GetValueType()==ParListValueType)
        SendExtraSignals((ParListValue*)myArg,0);
}
//досылаем дополнительные сигналы от константы
void DataWorker::SendExtraSignals(ParListValue* constant, int nodeNum)
{
    int count=constant->GetElementListSize();
    count--;                                //для одного элемента сигнал уже есть - сигнал самого ParList
    myEp->BroadcastAndExtendByRigNode(nodeNum,count);
}

//печатаем слой данных
void DataWorker::PrintActorDataLayer()
{
    qDebug() << "=========ActorDataLayer:=========";
    for(int i=0;i<actorDataLayer.size();i++)
    {
        if(actorDataLayer[i]==NULL)
            qDebug() << i << " NULL";
        else
            qDebug() << i << actorDataLayer[i]->GetAsStr();
    }
}
//формируем список специальных операций
void DataWorker::FormSpecList()
{
    specList=new SpecValue*[lastSpec];
    for(int i=0;i<lastSpec;i++)
        specList[i]=new SpecValue(SpecType(i));
}


//добавляем задачу в очередь
void DataWorker::AddTask(int taskNode, int taskInput)
{
    DataWorkerTask newTask(taskNode,taskInput);
    dataTasks.push_back(newTask);
}
//извлекаем и выполняем все имеющиеся в очереди задачи
void DataWorker::Work()
{
    DataWorkerTask curTask;
    while(dataTasks.size())
    {
        curTask=dataTasks.dequeue();
        ExecuteTask(curTask);
        PrintActorDataLayer();
    }
}
//извлекаем и выполняем одну задачу
void DataWorker::WorkOne()
{
    DataWorkerTask curTask;
    curTask=dataTasks.dequeue();
    ExecuteTask(curTask);
}
//выполняем конкретную задачу
bool DataWorker::ExecuteTask(DataWorkerTask curTask)
{
    ActorType curActorType=actorList[curTask.nodeNum]->GetActorType();
    switch(curActorType)
    {
        case argType:
            ExecuteArgument();
            break;
        case datalistType:
            ExecuteDataList(curTask);
            break;
        case parlistType:
            ExecuteParList(curTask);
            break;
        case aslistType:
            ExecuteAsyncList(curTask);
            break;
        case retType:
            ExecuteReturn(curTask);
            break;
        case interpType:
            ExecuteInter(curTask);
            break;
    }
    return true;
}
void DataWorker::ExecuteArgument()
{
    qDebug() << "Arg. Nothing to do here";
}
void DataWorker::ExecuteDataList(DataWorkerTask curTask)
{
    qDebug() << "Creating data list";
    actorDataLayer[curTask.nodeNum]=MakeDataList(curTask.nodeNum);

}
void DataWorker::ExecuteParList(DataWorkerTask curTask)
{
    qDebug() << "Creating par list";
    actorDataLayer[curTask.nodeNum]=AddToParList(curTask);
}
void DataWorker::ExecuteAsyncList(DataWorkerTask curTask)
{
    qDebug() << "Creating async list";

    Value* currentValue=actorDataLayer[curTask.nodeNum];
    if(currentValue==NULL)                                  //TODO DW1 defensive. Может там не пусто, но и не а-лист?
    {
        currentValue=new AsyncListValue();
        SetData(curTask.nodeNum,currentValue);
    }
    AsyncListValue* asyncList=(AsyncListValue*)currentValue;
    AsListActor* asyncActor=(AsListActor*)(actorList[curTask.nodeNum]);
    Value* newElement=GetFromLocation(asyncActor->memList[curTask.input-1]);
    asyncList->Append(newElement);
}
void DataWorker::ExecuteReturn(DataWorkerTask curTask)
{
    RetActor *tmpRetActor;
    //TODO это очень нехороший костыль. Но с параллельными списками надо думать что-то глобальное
    //да и вообще - разве в ret-вершину может быть больше одного входа?
    //эрго, каждый раз переписывать результат заново - не такое уж и зло.
    actorDataLayer[curTask.nodeNum]=NULL;
    qDebug() << "Copiing data pointer";
    tmpRetActor=(RetActor*)(actorList[curTask.nodeNum]);
    SetData(curTask.nodeNum,GetFromLocation(tmpRetActor->arg));
    qDebug() << "Stop machine!";
    rezult=actorDataLayer[curTask.nodeNum];
    qDebug() << "DataWorker's result is " << rezult->GetAsStr();
}
void DataWorker::ExecuteInter(DataWorkerTask curTask)
{
    InterpActor *tmpInterpActor;
    int argLen, funcLen;

    Value* arg;
    Value* func;

    qDebug() << "Interpretation time";

    tmpInterpActor=(InterpActor*)(actorList[curTask.nodeNum]);
    argLen=GetMemLocationMultiplicity(tmpInterpActor->arg);
    funcLen=GetMemLocationMultiplicity(tmpInterpActor->func);

    arg=GetFromLocation(tmpInterpActor->arg);
    func=GetFromLocation(tmpInterpActor->func);

    qDebug() << "Arg multiplicity is " << argLen;
    qDebug() << "Func multiplicity is " << funcLen;

    ReserveDoneTable(curTask.nodeNum, argLen,funcLen);
    ReserveInterpretAnswer(curTask.nodeNum,argLen*funcLen);

    int doneCount=0;
    //TODO да, тут по определению часть рассчетов будет выполняться многократно. довести до ума поддержку readyIndexes
    for(int argNum=0;argNum<argLen;argNum++)
        for(int funcNum=0;funcNum<funcLen;funcNum++)
            if(ProcessSinglePair(argNum,funcNum,curTask.nodeNum))
                doneCount++;

    if(!doneCount)                                  //ни единого задания - и одного сигнала не положено
        myEp->BlockOutputCGNode(curTask.nodeNum);
    if(doneCount>1)                                 //вагон сигналов - надо действовать
        myEp->BroadcastByRigNode(curTask.nodeNum,doneCount-1);

    //TODO грязно. В идеале, дальше список должен идти пофрагментно, а не куском.
    /*int extraSignalsCount=argLen*funcLen-1-nullCount;
    myEp->BroadcastByRigNode(curTask.nodeNum,extraSignalsCount);
    for(int index=0;index<extraSignalsCount;index++)
        myEp->BlockInputCGNode(curTask.nodeNum);*/

}
bool DataWorker::ProcessSinglePair(int argNum, int funcNum, int nodeNum)
{
    Value* arg;
    Value* func;
    Value* rez;
    int rezIndex;
    InterpActor *tmpInterpActor;
    int argLen;
    bool isFuncDataList;
    tmpInterpActor=(InterpActor*)(actorList[nodeNum]);
    arg=GetFromLocation(tmpInterpActor->arg);
    func=GetFromLocation(tmpInterpActor->func);

    if ((arg==NULL)||(func==NULL))
            return false;

    argLen=GetMemLocationMultiplicity(tmpInterpActor->arg);

    int argUID=arg->GetUID(argNum);
    int funcUID=func->GetUID(funcNum);

    if(CheckDoneTable(nodeNum,argUID,funcUID))
        return false;

    rezIndex=argLen*funcNum+argNum;
    rez=ALU::Interpret(func->ParGetElement(funcNum),arg->ParGetElement(argNum),this,nodeNum,rezIndex);
    isFuncDataList=(func->ParGetElement(funcNum)->GetValueType()==DataListValueType);
    qDebug() << "SinglePair: " << func->GetAsStr() << "(" << arg->GetAsStr() << ")=" << rez->GetAsStr();
    if(rez->GetValueType()==DelayValueType)
    {
        int delayNum;
        DelayValue* tmpDelay=(DelayValue*)rez;
        if(tmpDelay->GetDelayLink()==-1 && tmpDelay->GetDelayNumber()==-1)      //служебная фишка "раскрытие задержанного"
        {
            ValueType argType,funcType;
            DelayValue* delayArg;

            argType=arg->GetValueType();
            funcType=func->GetValueType();

            if(argType==DelayValueType)
                delayArg=(DelayValue*)arg;
            if(funcType==DelayValueType)
                delayArg=(DelayValue*)func;

            delayNum=delayArg->GetDelayNumber();
            if(delayArg->GetEPID()==myEp->GetID())
            {
                myEp->OpenDelayList(delayNum,nodeNum);
                if(argType==DelayValueType)
                    tmpInterpActor->arg=MemLocation(act,delayArg->GetDelayLink());
                if(funcType==DelayValueType)
                    tmpInterpActor->func=MemLocation(act,delayArg->GetDelayLink());
                return true;
            }
            else
            {
                qDebug() << "Alien DelayList detected!";
                rez=new MockValue();
                SetData(nodeNum,rez,rezIndex);
                myEp->CallDelayOpen(delayArg,delayArg->GetEPID(),nodeNum,rezIndex);
            }
        }
    }
    if(rez->GetValueType()==MockValueType)
    {
        SetDoneTable(nodeNum,argUID,funcUID);
        myEp->BlockOutputCGNode(nodeNum);
        return true;
    }
    if(rez->GetValueType()==NullValueType)
    {
        SetData(nodeNum,rez,rezIndex,isFuncDataList);
        return false;
    }
    bool setDataRezult=SetData(nodeNum,rez,rezIndex,isFuncDataList);   //вся эта булевость - костыль для разрешения синхронности в [x,y,z]:(.). TODO
    SetDoneTable(nodeNum,argUID,funcUID);
    myEp->BlockOutputCGNode(nodeNum,!setDataRezult);
    return true;
}
//резервируем место под "ответ" интерпретации
void DataWorker::ReserveInterpretAnswer(int nodeNum, int answerLen)
{
    if(actorDataLayer[nodeNum]!=NULL)
        return;                             //что-то там уже есть. Нечего возиться.
    if (answerLen==1)                       //один элемент? ты не парлист, ты просто ... не парлист
        return;
    actorDataLayer[nodeNum]=new ParListValue();
    for(int i=1;i<answerLen;i++)            //TODO DW2 функцию для ParListValue, которая будет делать это одним движением
        myEp->EnlargeDataLists(nodeNum);

}
bool DataWorker::CheckDoneTable(int nodeNum, int argUID, int funcUID)
{
    if((argUID<0)||(funcUID<0))
        return true;
    if(doneTables[nodeNum].size()<=argUID)
        return false;
    if(doneTables[nodeNum][argUID].size()<=funcUID)
        return false;
    return doneTables[nodeNum][argUID][funcUID];
}
void DataWorker::SetDoneTable(int nodeNum, int argUID, int funcUID)
{
    if(doneTables[nodeNum].size()<=argUID)
        doneTables[nodeNum].resize(argUID+1);
    if(doneTables[nodeNum][argUID].size()<=funcUID)
        doneTables[nodeNum][argUID].resize(funcUID+1);
    doneTables[nodeNum][argUID][funcUID]=true;
}

//резервируем место под таблицу уже исполненных параллельных пересекалок интерпретаций
void DataWorker::ReserveDoneTable(int nodeNum, int argLen, int funcLen)
{
    int argSize=doneTables[nodeNum].size();
    if(argSize<argLen)
    {
        doneTables[nodeNum].resize(argLen);
        argSize=argLen;
    }
    for(int i=0;i<argSize;i++)
        if(doneTables[nodeNum][i].size()<funcLen)
            doneTables[nodeNum][i].resize(funcLen);
}

void DataWorker::FillInterpretQueues(DataWorkerTask task)
{
    InterpActor* tmpInterpActor=(InterpActor*)(actorList[task.nodeNum]);
    Value* arg=GetFromLocation(tmpInterpActor->arg);
    Value* func=GetFromLocation(tmpInterpActor->func);

    int caller=task.input;
    if(caller==1)
        FillInterpretQueue(arg, &(interpActorQueuesList[task.nodeNum].argQueue));
    if(caller==2)
        FillInterpretQueue(func, &(interpActorQueuesList[task.nodeNum].funcQueue));
}
void DataWorker::FillInterpretQueue(Value *obj, QQueue<int> *queue)
{
    if(obj->GetValueType()!=ParListValueType)
    {
        queue->push_back(0);
        return;
    }
    ParListValue* parList=(ParListValue*)obj;
    int offsetReadyIndex=parList->GetReadyIndex(queue->size());
    queue->push_back(offsetReadyIndex);
}
//извлекаем объект по адресу в памяти
Value* DataWorker::GetFromLocation(MemLocation curLocation)
{
    switch(curLocation.GetMemType())
    {
        case act:
            return actorDataLayer[curLocation.GetIndex()];
            break;
        case ext:
            return new ExternValue(curLocation.GetIndex(),myEp->GetID());
            break;
        case loc:
            return localConstList[curLocation.GetIndex()];
            break;
        case spec:
            return GetSpecOp(curLocation.GetIndex());
            break;
        default:
            qDebug() << "Error! Getting MemLocation of unknown type";
            return NULL;
            break;
    }
}
//собираем список данных
Value* DataWorker::MakeDataList(int index)
{
    qDebug() << "Datalist creator activated";
    DataListValue *newDataList=new DataListValue();
    DataListActor* tmpDLActor;
    Value* newElement;
    QVector<Value*> pListElements;

    tmpDLActor=(DataListActor*)(actorList[index]);

    int linkListSize=tmpDLActor->memList.size();

    for(int i=0;i<linkListSize;i++)
    {
        newElement=GetFromLocation(tmpDLActor->memList[i]);

        if (newElement==NULL)
            newElement=new NullValue();
        //самораскрытие параллельного списка в списке данных
        if(newElement->GetValueType()==ParListValueType)
        {
            pListElements=*(((ParListValue*)newElement)->GetAllList());
            for(int j=0;j<pListElements.size();j++)
                newDataList->Append(pListElements[j]);
            continue;
        }
        //обычный элемент - обычное добавление
        newDataList->Append(newElement);
    }
    //сигналочистка
    newDataList->ClearSignals();

    qDebug() << "Datalist created: " << newDataList->GetAsStr();

    return newDataList;
}
//получаем спецоперацию
SpecValue* DataWorker::GetSpecOp(int x)
{
    if(x>lastSpec)
    {
        printf("Wrong (%d) spec-op called",x);
        return NULL;
    }
    return specList[x];
}
//добавляем элементы в параллельный список
Value* DataWorker::AddToParList(DataWorkerTask currentTask)
{
    ParListActor* currentParActor=(ParListActor*)actorList[currentTask.nodeNum];
    ParListValue* currentParList=(ParListValue*)actorDataLayer[currentTask.nodeNum];
    MemLocation newElementLocation=currentParActor->memList[currentTask.input-1];
    Value* newElement=GetFromLocation(newElementLocation);
    int linkListSize=currentParActor->memList.size();

    if (currentParList==NULL)
        currentParList=new ParListValue();
    if(newElement->GetValueType()==ParListValueType)    //парлист в парлист: раскрытие вложенности, блокировка входных сигналов в нужном числе
    {
        ParListValue* argParList=(ParListValue*)newElement;
        int argSize=argParList->GetElementListSize();
        int extraElements=-1;
        for(int i=0;i<argSize;i++)
        {
            Value* curEl=argParList->ParGetElement(i);
            if(curEl!=NULL)
            {
                currentParList->SetElement(curEl,currentTask.input);
                if(curEl->GetValueType()!=NullValueType)
                    if(curEl->GetValueType()!=MockValueType)
                        extraElements++;
            }
        }
        for(int i=0;i<extraElements;i++)
        {
            myEp->BlockInputCGNode(currentTask.nodeNum);
            qDebug() << "Signal to " << currentTask.nodeNum << " blocked";
        }
    }
    else                                                //непарлист в парлист: просто вкладываем
    {
        if (linkListSize==1)
            return newElement->Clone();
        currentParList->SetElement(newElement,currentTask.input);
    }
    return currentParList;
}
//добавляем фрагмент данных в заданную позицию слоя данных
void DataWorker::SetData(int nodeNum, Value *newData)
{
    if(actorDataLayer[nodeNum]==NULL)       //было пусто
    {
        actorDataLayer[nodeNum]=newData;
        return;
    }
    if(actorDataLayer[nodeNum]->GetValueType()==ParListValueType)   // был параллельный список
    {
        ((ParListValue*)actorDataLayer[nodeNum])->Append(newData);
        myEp->EnlargeDataLists(nodeNum);
        return;
    }
    ParListValue* newParList=new ParListValue();                //был, но не параллельный список
    newParList->Append(actorDataLayer[nodeNum]);
    newParList->Append(newData);
    actorDataLayer[nodeNum]=newParList;
    myEp->EnlargeDataLists(nodeNum);
}
// false вернется только при формировании даталиста
// и только если в нем остались NULL-ы
bool DataWorker::SetData(int nodeNum, Value *newData, int newDataIndex, bool isFuncDataList)
{
    if(actorDataLayer[nodeNum]==NULL)                               //было пусто
    {
        actorDataLayer[nodeNum]=newData;
        return true;
    }
    if(actorDataLayer[nodeNum]->GetValueType()==MockValueType)       //была заглушка
    {
        actorDataLayer[nodeNum]=newData;
        return true;
    }
    ValueType valueType=actorDataLayer[nodeNum]->GetValueType();
    if(!isFuncDataList)
    {
        if(valueType!=ParListValueType && !newDataIndex)
        {
            actorDataLayer[nodeNum]=newData;
            return true;
        }
        if(valueType!=ParListValueType && newDataIndex) //это не было парлистом, но теперь должно стать
        {
            ParListValue* parList = new ParListValue();
            parList->SetElement(actorDataLayer[nodeNum],actorDataLayer[nodeNum]->GetInput());
            parList->SetElement(newData,newDataIndex);
            actorDataLayer[nodeNum]=parList;
            //qDebug() << "Inserting in non-ParList actorDataUnit!";
            return true;
        }
        ((ParListValue*)actorDataLayer[nodeNum])->SetElement(newData,newDataIndex);
        return true;
    }
    else    // формируем результат блока вида [x,y,z]:(.)
    {
        if(valueType!=DataListValueType && !newDataIndex)
        {
            actorDataLayer[nodeNum]=newData;
            return true;
        }
        if(valueType!=DataListValueType && newDataIndex)
        {
            qDebug() << "Error! Inserting on non-0 position in non-DataList actorDataUnit!";
            return true;
        }
        qDebug() << actorDataLayer[nodeNum]->GetAsStr();

        DataListValue* newDataList=(DataListValue*)newData;
        DataListValue* rezDataList;
        Value* first;
        if(newData->GetValueType()==DataListValueType)
            first=(*(newDataList->GetElementList()))[0];
        else
            first=newData;
        rezDataList=((DataListValue*)actorDataLayer[nodeNum]);
        rezDataList->SetElement(first,newDataIndex);
        qDebug() << rezDataList->GetAsStr();

        int rezElemCount=rezDataList->GetElementListSize();         //В такой datalist может поступить не все. Тогда блочим
        for(int i=0;i<rezElemCount;i++)
            if ((*(rezDataList->GetElementList()))[i]->GetValueType()==NullValueType)
                return false;
        return true;
    }
}

//проверяем кратность объекта с заданным положением в памяти
int DataWorker::GetMemLocationMultiplicity(MemLocation curMemLocation)
{
    //необходимость в вызове этой функции может возникнуть, только если
    //в узел РИГ с номеро rigNodeNum пришло все, что только могло
    switch(curMemLocation.GetMemType())
    {
        case act:
            return GetActorMultiplicity(curMemLocation.GetIndex());
            break;
        case ext:
            return 1;
            break;
        case loc:
            return GetLocalMultiplicity(curMemLocation.GetIndex());
            break;
        case spec:
            return 1;
            break;
        default:
            qDebug() << "Error! Unknown type of MemLocation in GetMultiplicity!";
            return 0;
            break;
    }
}

//проверяем кратность актора
int DataWorker::GetActorMultiplicity(int actorIndex)
{
    ActorType curActorType=actorList[actorIndex]->GetActorType();
    InterpActor* curInterpActor;
    int sumOfAllLinks=0;
    ParListActor* curParListActor;
    ValueType valueType;
    switch(curActorType)
    {
        case argType:
            valueType=actorDataLayer[0]->GetValueType();
            if(valueType!=ParListValueType)
                return 1;
            return ((ParListValue*)actorDataLayer[0])->GetElementListSize();
            break;
        case interpType:
            curInterpActor=(InterpActor*)(actorList[actorIndex]);
            if (actorDataLayer[actorIndex]!=NULL && actorDataLayer[actorIndex]->GetValueType()!=NullValueType)  //TODO в функцию
                if(actorDataLayer[actorIndex]->GetValueType()==ParListValueType)
                    return GetValueSize(actorDataLayer[actorIndex]);
            /*if(curInterpActor->func.isSpecParList())
                return GetMemLocationSize(curInterpActor->arg);*/
            return GetMemLocationMultiplicity(curInterpActor->func)*GetMemLocationMultiplicity(curInterpActor->arg);
            break;
        case datalistType:
            return 1;
            break;
        case parlistType:
            sumOfAllLinks=0;
            curParListActor=(ParListActor*)(actorList[actorIndex]);
            for(int i=0;i<curParListActor->memList.size();i++)
                sumOfAllLinks+=GetMemLocationMultiplicity(curParListActor->memList[i]);
            return sumOfAllLinks;
            break;
        case aslistType:
            return 1;       //TODO DW3 Так ли это?
            break;
        case retType:
            return 1;       //TODO DW4 Как тебя вообще сюда занесло, добрый хлопец?
            break;
        default:
            qDebug() << "Error! Unknown type of actor in GetActorMultiplicity!";
            return 0;
            break;
    }
}

//проверяем кратность локальной константы
int DataWorker::GetLocalMultiplicity(int localIndex)
{
    Value* value=localConstList[localIndex];
    ValueType valueType=value->GetValueType();
    if(valueType!=ParListValueType)
        return 1;
    return ((ParListValue*)value)->GetElementListSize();
}


//проверяем размер актора
int DataWorker::GetMemLocationSize(MemLocation curMemLocation)
{
    switch(curMemLocation.GetMemType())
    {
        case act:
            return GetValueSize(actorDataLayer[curMemLocation.GetIndex()]);
            break;
        case ext:
            return 1;
            break;
        case loc:
            return GetValueSize(localConstList[curMemLocation.GetIndex()]);
            break;
        case spec:
            return 1;
            break;
        default:
            qDebug() << "Error! Unknown type of MemLocation in GetMemLocationSize!";
            return 0;
            break;
    }
}
int DataWorker::GetValueSize(Value* currentValue)
{
    if(currentValue->GetValueType()==DataListValueType)
        return ((DataListValue*)currentValue)->GetElementListSize();
    if(currentValue->GetValueType()==ParListValueType)
        return ((ParListValue*)currentValue)->GetElementListSize();
    return 1;
}

//считаем количество связей
int DataWorker::GetLinksCount(int nodeIndex)
{
    //попасть сюда можно только если этот узел - DataList
    //TODO DW5 - защита от дурака
    return ((DataListActor*)actorList[nodeIndex])->memList.size();
}

//вызываем внешнюю функцию (через СП в ЦМ)
void DataWorker::CallExtern(int externId, int externOwnerID, Value* arg, int nodeNum, int posIndex)
{
    myEp->CallExtern(externId, externOwnerID, arg, nodeNum, posIndex);
}

Value* DataWorker::GetDelayResult(Value* arg)
{
    DelayValue* delayToGet=(DelayValue*)arg;
    return actorDataLayer[delayToGet->GetDelayLink()];
}

//получаем уникальный в рамках сессии ID функции
//не очень аккуратная цепочка - и лишь для того, чтоб АЛУ не видело ЦМ напрямую
int DataWorker::GetFuncUID(int EPID, int funcID)
{
    return myEp->GetFuncUID(EPID, funcID);
}


#endif



