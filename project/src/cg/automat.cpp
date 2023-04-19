#include "automat.h"

//=================================================================================================
// Конструкторы AutomatState
AutomatState::AutomatState()
{
    state=0;
    dataListLength=0;
}
AutomatState::AutomatState(int newState)
{
    state=newState;
    dataListLength=0;
}

//=================================================================================================
// Тестовый вывод базовой части автомата
void Automat::TestOut(QTextStream& out) {
    out << ";"; // << endl;
}
QString Automat::GetAsStr() {
    return QString(";");
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Автомат интерпретации
//=================================================================================================
// Тестовый вывод Interp-автомата
void InterpAutomat::TestOut(QTextStream& out) {
    out << ":, ";
    Automat::TestOut(out);
}
QString InterpAutomat::GetAsStr() {
    return ":, "+Automat::GetAsStr();
}
//Обработка входного сигнала
AutomatResult InterpAutomat::ProcessSignal(AutomatState curState, int input)
{
    qDebug() << "I'm " << GetName() << " in the state " << curState.state << " and I got signal " << input;
    autoResultCode newResultCode=AR_NOTHING;
    switch(curState.state)
    {
        case 0:                     //have nothing
            if (input==1)           //got arg
                curState.state=1;
            if (input==2)           //got func
                curState.state=2;
            break;
        case 1:                     //have arg
            if (input==1)           //more arg
                curState.state=1;
            if (input==2)           //got func
                curState.state=3;
            break;
        case 2:                     //have func
            if (input==1)           //got arg
                curState.state=3;
            if (input==2)           //more func
                curState.state=2;
            break;
        case 3:                     //have all
            if (input==1)           //got arg
                curState.state=3;
            if (input==2)           //more func
                curState.state=3;
            break;
    }
    if(curState.state==3)
    {
        newResultCode=AR_EXECUTE;
    }
    return AutomatResult(curState,newResultCode);
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// Автомат списка данных
//=================================================================================================
// Тестовый вывод Dlist-автомата
void DlistAutomat::TestOut(QTextStream& out) {
    out << "(---), ";
    Automat::TestOut(out);
}
QString DlistAutomat::GetAsStr() {
    return "(---), "+Automat::GetAsStr();
}
//Обработка входного сигнала
AutomatResult DlistAutomat::ProcessSignal(AutomatState curState, int input)
{
    qDebug() << "I'm " << GetName() << " in the state " << curState.state << "; my len is " << curState.dataListLength << " and I got signal " << input;
    //пока все просто и в лоб
    //параллельные списки, поступая в списки данных, автораскрываются и увеличивают длину списка данных
    curState.state++;
    if(curState.state>=curState.dataListLength)     //список заполнен
        return AutomatResult(curState,AR_EXECUTE);
    return AutomatResult(curState,AR_NOTHING);
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// Автомат параллельного списка
//=================================================================================================
// Тестовый вывод Plist-автомата
void PlistAutomat::TestOut(QTextStream& out) {
    out << "[---], ";
    Automat::TestOut(out);
}
QString PlistAutomat::GetAsStr() {
    return "[---], "+Automat::GetAsStr();
}
//Обработка входного сигнала
AutomatResult PlistAutomat::ProcessSignal(AutomatState curState, int input)
{
    qDebug() << "I'm " << GetName() << " in the state " << curState.state << " and I got signal " << input;
    return AutomatResult(curState,AR_EXECUTE);
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// Автомат асинхронного списка
//=================================================================================================
// Тестовый вывод Alist-автомата
void AlistAutomat::TestOut(QTextStream& out) {
    out << "<(--), ";
    Automat::TestOut(out);
}
QString AlistAutomat::GetAsStr() {
    return "<(--), "+Automat::GetAsStr();
}
//Обработка входного сигнала
AutomatResult AlistAutomat::ProcessSignal(AutomatState curState, int input)
{
    qDebug() << "I'm " << GetName() << " in the state " << curState.state << " and I got signal " << input;
    if(curState.state==0)
    {
        curState.state=1;
        return AutomatResult(curState,AR_EXECUTE);
    }
    return AutomatResult(curState,AR_ASYNC_ADD);
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// Автомат входного аргумента
//=================================================================================================
// Тестовый вывод Arg-автомата
void ArgAutomat::TestOut(QTextStream& out) {
    out << "arg, ";
    Automat::TestOut(out);
}
QString ArgAutomat::GetAsStr() {
    return "arg, "+Automat::GetAsStr();
}
//Обработка входного сигнала
AutomatResult ArgAutomat::ProcessSignal(AutomatState curState, int input)
{
    qDebug() << "I'm " << GetName() << " in the state " << curState.state << " and I got signal " << input;
    autoResultCode curResultCode=AR_NOTHING;
    switch(curState.state)
    {
        case 0:                     //автомат не активирован
            if (input==1)           //запускаем аргумент
            {
                curState.state=1;
                curResultCode=AR_EXECUTE;
            }
            break;
    }
    return AutomatResult(curState,curResultCode);
}



///////////////////////////////////////////////////////////////////////////////////////////////////
// Автомат Return-вершины
//=================================================================================================
// Тестовый вывод Ret-автомата
void RetAutomat::TestOut(QTextStream& out) {
    out << "return, ";
    Automat::TestOut(out);
}
QString RetAutomat::GetAsStr() {
    return "return, "+Automat::GetAsStr();
}
//Обработка входного сигнала
AutomatResult RetAutomat::ProcessSignal(AutomatState curState, int input)
{
    qDebug() << "I'm " << GetName() << " in the state " << curState.state << " and I got signal " << input;
    autoResultCode curResultCode=AR_NOTHING;
    switch(curState.state)
    {
        case 0:                     //вершина возврата не срабатывала
            if (input==1)           //возвращаем результат
            {
                curState.state=1;
                curResultCode=AR_EXECUTE;
            }
            break;
        case 1:                     //вершина возврата уже срабатывала
            if (input==1)           //дополняем результат
            {
                curState.state=1;
                curResultCode=AR_EXECUTE;
            }
            break;
    }
    return AutomatResult(curState,curResultCode);
}

