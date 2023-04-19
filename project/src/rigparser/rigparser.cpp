#include "rigparser.h"
#include "time.h"
#include <cstdlib>
#include <iostream>
using namespace std;

//=================================================================================================
// Конструктор
RigParser::RigParser(Rig* _pRig, QString _inFile): pRig(_pRig), inFile(_inFile), flag(true)
{
    InitValues();
}

RigParser::RigParser(Rig* _pRig, QString rigText, bool uselessFlag): pRig(_pRig), inFile("rigparserTemp.txt"), flag(true)
{
    QFile file("rigparserTemp.txt");
    if(file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        stream << rigText;
        if(stream.status() != QTextStream::Ok) {
            qDebug() << "File writing error!!!";
        }
        file.close();
    }
    else
    {
        qDebug() << "Error while opening rigparserTemp.txt";
    }

    InitValues();
}


RigParser::~RigParser()
{

}

void RigParser::InitValues()
{
    elementaryValues["."] = dotSpec;        // "." - пустое или сигнальное значение
    elementaryValues["+"] = addSpec;        // "+" - плюс
    elementaryValues["-"] = minusSpec;      // "-" - минус
    elementaryValues["*"] = astSpec;        // "*" - звездочка
    elementaryValues["/"] = slashSpec;      // "/" - слэш
    elementaryValues["%"] = percentSpec;    // "%" - процент
    elementaryValues[".."] = dotsSpec;       // ".." - многоточие
    elementaryValues["dup"] = dupSpec;        // "dup" - дубликаты
    elementaryValues["<"] = ltSpec;         // "<" - меньше
    elementaryValues["<="] = leSpec;         // "<=" - меньше или равно
    elementaryValues["="] = eqSpec;         // "=" - равно
    elementaryValues["!="] = neSpec;         // "!=" - не равно
    elementaryValues[">="] = geSpec;         // ">=" - больше или равно
    elementaryValues[">"] = gtSpec;         // ">" - больше
    elementaryValues["|"] = vertSpec;       // "|" - вертикальная черта
    elementaryValues["?"] = questSpec;      // "?" - вопрос
    elementaryValues["#"] = sharpSpec;      // "#" - решетка
    elementaryValues["type"] = typeSpec;       // "type" - тип
    elementaryValues["value"] = valSpec;        // "value" - значение
    elementaryValues["bool"] = boolSpec;       // "bool" - булевый
    elementaryValues["char"] = charSpec;       // "char" - символьный
    elementaryValues["int"] = intSpec;        // "int" - целый
    elementaryValues["float"] = floatSpec;      // "float" - действительный
    elementaryValues["string"] = strSpec;        // "string" - строковый
    elementaryValues["()"] = dalistSpec;     // "()" - список данных
    elementaryValues["[]"] = palistSpec;     // "[]" - параллельный список
    elementaryValues["{}"] = delistSpec;     // "{}" - задержанный список
    elementaryValues["<()"] = aslistSpec;     // "<()" - асинхронный список
    elementaryValues["signal"] = signalSpec;     // "signal" - сигнал
    elementaryValues["error"] = errorSpec;       // "error" - ошибка
    endOfProgFlag = false;
}

//=================================================================================================
// Функция, запускающая парсер после всех начальных установок
bool RigParser::Start() {
    bool result=false;
    if(Load(inFile)) {
        result=true;
        TestOut();
        it = text.constBegin();
        line = column = 0;
        try {
            if(this->IsRig() && flag) {
                qDebug() << "OK";
            } else {
             Error("Invalid RIG");
             result=false;
            }
        }
        catch (EndOfProg) {
            qDebug() << "The end of RIG achived";
        }
    }
    return result;
}

//-------------------------------------------------------------------------------------------------
// Правила, описывающие синтаксис РИГ
//-------------------------------------------------------------------------------------------------

//=================================================================================================
// Реверсивный информационный граф
bool RigParser::IsRig() {
//_0:
    if(Dcl()) {goto _1;}
    return false;
_1:
    if(Graph()) {goto _end;}
    Error("Invalid RIG");
    return false;
_end:
    qDebug() << "IsRig Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Описания внешних ссылок и локальных констант
bool RigParser::Dcl() {
//_0:
    if(IsExtRef()) { goto _1;}
    return false;
_1:
    if(LocalConst()) {goto _end;}
    Error("Invalid declaration");
    return false;
_end:
    qDebug() << "Dcl Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Внешние ссылки
bool RigParser::IsExtRef()
{
    QString str;
//_0:
    if(Word(QString("External"))) {goto _1;}
    flag = false;
    return false;
_1:
    if(EndOfLine()) {goto _2;}
    Error("End of line proposed");
    return false;
_2:
    if(Blank()) {goto _3;}
    if(Positive(str)) {goto _4;}
    Error("Blanks or Positive number proposed");
    return false;
_3:
    if(Positive(str)) {goto _4;}
    Error("Positive numer proposed");
    return false;
_4:
    if(Blank()) {goto _5;}
    Error("Blanks proposed");
    return false;
_5:
    if(Name(str)) {pRig->Append(new ExtRef(str)); goto _6;}
    Error("Name of External reference proposed");
    return false;
_6:
    if(EndOfLine()) {goto _7;}
    Error("End of line proposed");
    return false;
_7:
    if(Blank()) {goto _3;}
    if(Positive(str)) {goto _4;}
    goto _end;
_end:
    qDebug() << "IsExtRef Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Локальные константы
bool RigParser::LocalConst() {
    QString str;
//_0:
    if(Word(QString("Local"))) {goto _1;}
    return false;
_1:
    if(EndOfLine()) {goto _2;}
    Error("End of line proposed");
    return false;
_2:
    if(Blank()) {goto _3;}
    if(Positive(str)) {goto _4;}
    goto _end;
_3:
    if(Positive(str)) {goto _4;}
    Error("Positive numer proposed");
    return false;
_4:
    if(Blank()) {goto _5;}
    Error("Blanks proposed");
    return false;
_5:
    if(IsValue()) {goto _6;}
    Error("Value of local constant proposed");
    return false;
_6:
    if(EndOfLine()) {goto _2;}
    Error("End of line proposed");
    return false;
_end:
    qDebug() << "LocalConst Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Граф (сами операции РИГ)
bool RigParser::Graph() {
//_0:
    if(Title()) {goto _1;}
    return false;
_1:
    if(ActorList()) {goto _end;}
    Error("List of actors proposed");
    return false;
_end:
    qDebug() << "Graph Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Заголовок графа
bool RigParser::Title() {
//_0:
    if(Word(QString("id"))) {goto _1;}
    return false;
_1:
    if(Blank()) {goto _2;}
    Error("Blanks proposed");
    return false;
_2:
    if(Word(QString("delay"))) {goto _3;}
    Error("<delay> word proposed");
    return false;
_3:
    if(Blank()) {goto _4;}
    Error("Blanks proposed");
    return false;
_4:
    if(Word(QString("operation"))) {goto _5;}
    Error("<operation> word proposed");
    return false;
_5:
    if(Blank()) {goto _6;}
    Error("Blanks proposed");
    return false;
_6:
    if(Word(QString("links"))) {goto _7;}
    Error("<links> word proposed");
    return false;
_7:
    if(Blank()) {goto _8;}
    Error();
    return false;
_8:
    if(Word(QString("positions"))) {goto _9;}
    Error("<positions> word proposed");
    return false;
_9:
    if(EndOfLine()) {goto _end;}
    Error("End of line proposed");
    return false;
_end:
    qDebug() << "Title Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Список вершин графа
bool RigParser::ActorList() {
    Actor* pActor = 0;
//_0:
    if(IsBegActor(&pActor)) {
        goto _1;
    }
    return false;
_1:
    qDebug() << qPrintable(pActor->GetAsStr());
    pRig->Append(pActor);
    if(endOfProgFlag) {
        //throw EndOfProg();
        goto _end;
    }
    if(IsActor(&pActor)) {goto _1;}
    goto _end;
    //if(EndActor()) {goto _end;}
    //Error("Actor proposed");
    //return false;
_end:
    qDebug() << "ActorList Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Начальная вершина графа
bool RigParser::IsBegActor(Actor** ppActor) {
    Actor* pActor = 0;
    int p[4];
//_0:
    if(Word(QString("0"))) {goto _1;}
    return false;
_1:
    if(Blank()) {goto _2;}
    Error("Blanks proposed");
    return false;
_2:
    if(Word(QString("0"))) {goto _3;}
    Error("number 0 proposed");
    return false;
_3:
    if(Blank()) {goto _4;}
    Error("Blanks proposed");
    return false;
_4:
    if(Word(QString("arg"))) {goto _5;}
    Error("<arg> word proposed");
    return false;
_5:
    if(Blank()) {goto _6;}
    Error("Blanks proposed");
    return false;
_6:
    if(IsPositions(p)) {goto _7;}
    Error("Position list proposed");
    return false;
_7:
    if(EndOfLine()) {goto _end;}
    Error("End of line proposed");
    return false;
_end:
    pActor = new BegActor();
    pActor->SetDelay(0);
    pActor->pos.Set(p);
    //pActor->pos.Set(0,0,0,0);
    *ppActor = pActor;
    qDebug() << "BegActor Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Вершина графа
bool RigParser::IsActor(Actor** ppActor) {
    QString str1;
    QString str2;
    Actor* pActor = 0;
    int p[4];
//_0:
    if(Positive(str1)) {goto _1;}
    return false;
_1:
    if(Blank()) {goto _2;}
    Error("Blanks proposed");
    return false;
_2:
    if(Positive(str2)) {goto _3;}
    Error("Positive number proposed");
    return false;
_3:
    if(Blank()) {goto _4;}
    Error("Blanks proposed");
    return false;
_4:
    if(Operator(&pActor)) {goto _5;}
    Error("Operator or <return> proposed");
    return false;
_5:
    if(IsPositions(p)) {goto _6;}
    Error("Position list proposed");
    return false;
_6:
    if(EndOfLine()) {goto _end;}
    Error("End of line or file proposed");
    return false;
_end:
    pActor->SetDelay(str2.toInt());
    pActor->pos.Set(p);
    //pActor->pos.Set(10,20,30,40);
    *ppActor = pActor;
    qDebug() << "Actor Rool is successful in (" << line << "," << column << ")";
    return true;
}

/*
//=================================================================================================
// Конечная вершина графа
bool RigParser::EndActor() {
    QString str;
//_0:
    if(Positive(str)) {goto _1;}
    return false;
_1:
    if(Blank()) {goto _2;}
    Error("Blanks proposed");
    return false;
_2:
    if(Word(QString("0"))) {goto _3;}
    Error("Number 0 proposed");
    return false;
_3:
    if(Blank()) {goto _4;}
    Error("Blanks proposed");
    return false;
_4:
    if(Word(QString("return"))) {goto _5;}
    Error("<return> word proposed");
    return false;
_5:
    if(Blank()) {goto _6;}
    Error("Blanks proposed");
    return false;
_6:
    if(Positive(str)) {goto _7;}
    Error("Positive number proposed");
    return false;
_7:
    if(Blank()) {goto _8;}
    Error("Blanks proposed");
    return false;
_8:
    if(IsPositions()) {goto _9;}
    Error("Position list proposed");
    return false;
_9:
  if(EndOfLine()) {goto _end;}
    Error("End of line proposed");
    return false;
_end:
    qDebug() << "EndActor Rool is successful in (" << line << "," << column << ")";
    return true;
}
*/

//=================================================================================================
// Оператор
bool RigParser::Operator(Actor** ppActor) {
    Opc opc;
    MemType memType;    // Тип используемой памяти
    int addr;           // Адрес в соответствующей памяти
    Actor* pActor = 0;
    QList<MemType> memTypeList;
    QList<int> addrList;
    QList<MemType>::const_iterator memTypeIter;
    QList<int>::const_iterator addrIter;
    MemType mt, mt1, mt2;
    int a, a1, a2;
    MemLocation ml;
    ListActor* pLA;
//_0:
    if(Operation(opc)) {goto _1;}
    return false;
_1:
    if(Blank()) {goto _2;}
    Error("Blanks proposed");
    return false;
_2:
    if(Ref(memType, addr)) {
        memTypeList.append(memType);
        addrList.append(addr);
        goto _3;
    }
    Error("Reference proposed");
    return false;
_3:
    if(Blank()) {goto _4;}
    Error("Blanks proposed");
    return false;
_4:
    if(Ref(memType, addr)) {
        memTypeList.append(memType);
        addrList.append(addr);
        goto _3;
    }
    goto _end;
_end:
    qDebug() << "===Operator Debug: opc = " << opc;
    switch(opc) {
    case interp:
        if(memTypeList.size()!=2 || addrList.size()!=2) {
            Error("Incorrect arguments number of interpreting operation");
            return false;
        }
        mt1 = memTypeList.at(0);
        qDebug() << "===Operator interp Debug: mt1 = " << mt1;
        mt2 = memTypeList.at(1);
        qDebug() << "===Operator interp Debug: mt2 = " << mt2;
        a1 = addrList.at(0);
        qDebug() << "===Operator interp Debug: a1 = " << a1;
        a2 = addrList.at(1);
        qDebug() << "===Operator interp Debug: a2 = " << a2;
        pActor = new InterpActor(mt1, a1, mt2, a2);
        goto _finish;
        //break;
    case dlist:
        pLA = new DataListActor;
        goto _createMemTypeList;    // аргументы всех списков формируются одинаково
        //break;
    case plist:
        pLA = new ParListActor;
        goto _createMemTypeList;    // аргументы всех списков формируются одинаково
        //break;
    case alist:
        pLA = new AsListActor;
        goto _createMemTypeList;    // аргументы всех списков формируются одинаково
        //break;
    case ret:
        if(memTypeList.size()!=1 || addrList.size()!=1) {
            Error("Incorrect arguments number of return operation");
            return false;
        }
        mt = memTypeList.at(0);
        qDebug() << "===Operator return Debug: mt = " << mt;
        a = addrList.at(0);
        qDebug() << "===Operator return Debug: a = " << a;
        pActor = new RetActor(mt, a);
        goto _finish;
        //break;
    default:
        Error("Incorrect operation in Actor");
        return false;
    }
_createMemTypeList: // Общий для всех списков фрагмент формирования аргументов
    for (memTypeIter = memTypeList.constBegin(), addrIter = addrList.constBegin();
         memTypeIter != memTypeList.constEnd() && addrIter != addrList.constEnd();
         ++memTypeIter, ++addrIter)
    {
        mt = *memTypeIter;
        qDebug() << "===Operator list Debug: mt = " << mt;
        a = *addrIter;
        qDebug() << "===Operator list Debug: a = " << a;
        ml.Set(mt, a);
        pLA->memList.append(ml);
    }
    pActor = pLA;
    goto _finish;
_finish:
    *ppActor = pActor;
    qDebug() << "Operator Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Позиции
bool RigParser::IsPositions(int pos[]) {
    QString str;
    int count = 0;
//_0:
    if(Word(QString("pos"))) {goto _1;}
        return false;
_1:
    if(Blank()) {goto _2;}
    Error("Blanks proposed");
    return false;
_2:
    if(Positive(str)) {pos[count++]= str.toInt(); goto _3;}
    Error("Positive number proposed");
    return false;
_3:
    if(Blank()) {goto _2;}
    goto _4;
_4:
    if(count == 4) {goto _end;}
    Error("Fore (4) position\'s numbers proposed");
    return false;
_end:
    qDebug() << "Positions Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Ссылка на одну из областей памяти
bool RigParser::Ref(MemType& memType, int& addr) {
    QString str;
    SpecValue* val;     // Возращаемое при разборе значение
//_0:
    if(Word(QString("ext:"))) {memType = ext; goto _1;}
    if(Word(QString("loc:"))) {memType = loc; goto _1;}
    if(Positive(str)) {memType = act; addr = str.toInt(); goto _end;}
    if(Elementary(&val)) {memType = spec; addr = val->GetAddr(); goto _end;}
    return false;
_1:
    if(Positive(str)) {addr = str.toInt(); goto _end;}
    Error("Positive number proposed");
    return false;
_end:
    qDebug() << "Ref Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Операция
bool RigParser::Operation(Opc& opc) {
//_0:
    if(Word(QString(":"))) {opc = interp; goto _end;}
    if(Word(QString("(---)"))) {opc = dlist; goto _end;}
    if(Word(QString("[---]"))) {opc = plist; goto _end;}
    if(Word(QString("<(--)"))) {opc = alist; goto _end;}
    if(Word(QString("return"))) {opc = ret; goto _end;}
    return false;
_end:
    qDebug() << "Operation Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Значение
bool RigParser::IsValue() {
    QString str;
    Value* val = 0;     // Возращаемое при разборе значение
    DelayValue* delayVal = 0;
    SpecValue* specVal = 0;
    CharValue* charVal = 0;
    BoolValue* boolVal = 0;
    DataListValue* dListVal = 0;
    ParListValue* pListVal = 0;
    ErrorValue* errVal = 0;
//_0:
    if(IntReal(&val)) { lastValue=val; pRig->Append(val); goto _end;}
    if(Booler(&boolVal)) { lastValue=boolVal; pRig->Append(boolVal); goto _end;}
    if(IsError(&errVal)) { lastValue=errVal; pRig->Append(errVal); goto _end;}
    if(Delayed(&delayVal)) { lastValue=delayVal; pRig->Append(delayVal); goto _end;}
    if(Elementary(&specVal)) { lastValue=specVal; pRig->Append(specVal); goto _end;}
    if(IsChar(&charVal)) { lastValue=charVal; pRig->Append(charVal); goto _end;}
    if(IsStr(&dListVal)) { lastValue=dListVal; pRig->Append(dListVal); goto _end;}
    if(IsConstDataList(&dListVal)) { lastValue=dListVal; pRig->Append(dListVal); goto _end;}
    if(IsConstParList(&pListVal)) { lastValue=pListVal; pRig->Append(pListVal); goto _end;}
    //if(Name(str)) {goto _end;}
    return false;
_end:
    qDebug() << "Value = " << str << "Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Целое или действительное
bool RigParser::IntReal(Value** ppVal) {
    IntValue* pIntVal = 0;
    FloatValue* pFloatVal = 0;
    QString irStr;
    QString str;
    int i;
    int oldColumn=column;
    double d;
//_0:
    if(Word(QString("+"))) {irStr = ""; goto _1;}
    if(Word(QString("-"))) {irStr = "-"; goto _1;}
    if(Word(QString("."))) {irStr = "0."; goto _4;}
    if(Positive(str)) {irStr = str; goto _2;}
    goto _false;
_1:
    if(Word(QString("."))) {irStr += "0."; goto _4;}
    if(Positive(str)) {irStr += str; goto _2;}
    goto _false;
_2:
    if(Word(QString("."))) {irStr += "."; goto _3;}
    if(Word(QString("E"))) {irStr += "E"; goto _6;}
    if(Word(QString("e"))) {irStr += "E"; goto _6;}
    goto _int;
_3:
    if(Positive(str)) {irStr += str; goto _5;}
    goto _real;
_4:
    if(Positive(str)) {irStr += str; goto _5;}
    goto _false;
_5:
    if(Word(QString("E"))) {irStr += "E"; goto _6;}
    if(Word(QString("e"))) {irStr += "E"; goto _6;}
    goto _real;
_6:
    if(Word(QString("+"))) {goto _7;}
    if(Word(QString("-"))) {irStr += "-"; goto _7;}
    if(Positive(str)) {irStr += str; goto _real;}
    goto _false;
_7:
    if(Positive(str)) {irStr += str; goto _real;}
    goto _false;
_false:
    column=oldColumn;
    Error();
    return false;
_int:
    i = irStr.toInt();
    pIntVal = new IntValue(i);
    *ppVal = pIntVal;
    qDebug() << "Integer = " << i << "Rool is successful in (" << line << "," << column << ")";
    return true;
_real:
    d = irStr.toDouble();
    pFloatVal = new FloatValue(d);
    *ppVal = pFloatVal;
    qDebug() << irStr << " Real = " << d << "Rool is successful in (" << line << "," << column << ")";
    return true;
}
//
bool RigParser::Booler(BoolValue** boolVal)
{
    BoolValue* boolValue;
    if(Word(QString("true"))) {goto _true;}
    if(Word(QString("false"))) {goto _false;}
    return false;
_true:
    boolValue=new BoolValue(true);
    *boolVal=boolValue;
    qDebug() << "Bool Rool is successful in (" << line << "," << column << ")";
    return true;
_false:
    boolValue=new BoolValue(false);
    *boolVal=boolValue;
    qDebug() << "Bool Rool is successful in (" << line << "," << column << ")";
    return true;
}
//
bool RigParser::IsError(ErrorValue** errVal)
{
    ErrorValue* errorValue;
    ErrorType stErr;
    if(Word(QString("BASEFUNCERROR")))  { stErr=BASEFUNCERROR; goto _true;}
    if(Word(QString("TYPEERROR")))      { stErr=TYPEERROR; goto _true;}
    if(Word(QString("BOUNDERROR")))     { stErr=BOUNDERROR; goto _true;}
    if(Word(QString("INTERROR")))       { stErr=INTERROR; goto _true;}
    if(Word(QString("REALERROR")))      { stErr=REALERROR; goto _true;}
    if(Word(QString("ZERODIVIDE")))     { stErr=ZERODIVIDE; goto _true;}
    if(Word(QString("VALUEERROR")))     { stErr=VALUEERROR; goto _true;}
    return false;
_true:
    errorValue=new ErrorValue(stErr);
    *errVal=errorValue;
    qDebug() << "Error Rool is successful in (" << line << "," << column << ")";
    return true;
}
//=================================================================================================
// Задержанное
bool RigParser::Delayed(DelayValue** ppVal) {
    DelayValue* pDelayVal = 0;
    QString str1;
    QString str2;
//_0:
    if(Word(QString("{"))) {goto _1;}
    return false;
_1:
    if(Positive(str1)) {goto _2;}
    Error();
    return false;
_2:
    if(Word(QString("}"))) {goto _3;}
    Error();
    return false;
_3:
    if(Positive(str2)) {goto _end;}
    Error();
    return false;
_end:
    int dnum = str1.toInt();
    int link = str2.toInt();
    pDelayVal = new DelayValue(dnum,link);
    //pRig->Append(pIntVal);
    *ppVal = pDelayVal;
    qDebug() << "Delayed Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Элементарное
bool RigParser::Elementary(SpecValue** ppVal) {
    // Получение лексемы, определяющей элементарное значение (специальный тип)
    // Осуществляется набором значения до появления пробелов
    QString str;
    str.clear(); // Очистка буфера строки
    str="";
    int tmpCol = column;            // фиксация текущей позиции строки
    //QChar si = (*it)[tmpCol];       // берется очередной символ
    QChar si = GetSymbol(tmpCol);       // берется очередной символ
    // Проверяется достижение разделителей (табуляции, пробела)
    while(!(si == ' ' || si == '\t' || si == '\0'))
    {
        if(si==')' && str[str.length()-1]!='(')   //это скобка не от нас, а от даталиста, в котором мы входим
            break;
        // Накопление операнда
        str += si;
        tmpCol++;
        //si = (*it)[tmpCol];
        si = GetSymbol(tmpCol);
    }
    // Появился разделитель. Нужно сравнить набранное значение со специальными типами
    if(elementaryValues.contains(str)) {
        // Совпало. Это специальный тип. Фомируется соответствующее специальное значение.
        SpecValue* pSpecVal = new SpecValue(elementaryValues[str]);
        column = tmpCol;    // Сдвиг на следующую лексему
        *ppVal = pSpecVal;
        qDebug() << "Elementary (" << str << ") Rool is successful in (" << line << "," << column << ")";
        return true;
    }
    // В противном случае операнд может быть другого типа
    return false;
}

//=================================================================================================
// Имя
bool RigParser::Name(QString& str) {
//_0:
    QString str1;
    QString str2;
    if(Id(str1)) {goto _1;}
    return false;
_1:
    //if((*it)[column] == QChar('.')){column++; goto _2;}
    if(GetSymbol(column) == QChar('.')){column++; goto _2;}
    goto _end;
_2:
    if(Id(str2))
    {
        str1 += '.' + str2;
        goto _1;
    }
    Error();
    return false;
_end:
    str = str1;
    qDebug() << "Name = " << str << "Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Идентификатор
bool RigParser::Id(QString& str) {
    QChar sym;
//_0:
    if(Letter(sym)) {str = sym; goto _1;}
    //if((*it)[column] == QChar('_')){str = '_'; column++; goto _1;}
    if(GetSymbol(column) == QChar('_')){str = '_'; column++; goto _1;}
    return false;
_1:
    if(Letter(sym)) {str += sym; goto _1;}
    //if((*it)[column] == QChar('_')){str += '_'; column++; goto _1;}
    if(GetSymbol(column) == QChar('_')){str += '_'; column++; goto _1;}
    if(Digit(sym)) {str += sym;  goto _1;}
    goto _end;
_end:
    qDebug() << "Id = " << str << "Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Строка
bool RigParser::IsStr(DataListValue** ppVal) {
    QChar sym;
    QString str = "";
    QString erStr = "Incorrect string ";
    //sym = (*it)[column];
    int oldColumn = column;
    sym = GetSymbol(column);
//_0:
    if(Word(QString("("))) {goto _1;}
    goto _fail;
_1:
    if(Word(QString("\""))) {goto _2;}
    goto _fail;
_2:
    //sym = (*it)[column];
    sym = GetSymbol(column);
    if(sym == '\"') {column++; goto _3;}
    if(sym != '\'' && sym != '\\' && sym.isPrint()) {str += sym; column++; goto _2;}
    if(Word(QString("\\n"))) {str += "\n"; goto _2;}
    if(Word(QString("\\r"))) {str += '\r'; goto _2;}
    if(Word(QString("\\t"))) {str += '\t'; goto _2;}
    if(Word(QString("\\\'"))) {str += '\''; goto _2;}
    if(Word(QString("\\\""))) {str += '\"'; goto _2;}
    if(Word(QString("\\\\"))) {str += '\\'; goto _2;}
    erStr += str;
    Error(erStr);
    goto _fail;
_3:
    if(Word(QString(")"))) {goto _end;}
    goto _fail;
_fail:
    column=oldColumn;
    return false;
_end:
    DataListValue* pVal = new DataListValue(str);
    *ppVal = pVal;
    qDebug() << "Str = " << str << "Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Символ
bool RigParser::IsChar(CharValue** ppVal) {
    QChar sym;
    //int tmpCol = column;
//_0:
    if(Word(QString("\'"))) {goto _1;}
    return false;
_1:
    //sym = (*it)[column];
    sym = GetSymbol(column);
    if(sym != '\'' && sym != '\\' && sym.isPrint()) { column++; goto _2;}
    if(Word(QString("\\n"))) {sym = '\n'; goto _2;}
    if(Word(QString("\\r"))) {sym = '\r'; goto _2;}
    if(Word(QString("\\t"))) {sym = '\t'; goto _2;}
    if(Word(QString("\\\'"))) {sym = '\''; goto _2;}
    if(Word(QString("\\\""))) {sym = '\"'; goto _2;}
    if(Word(QString("\\\\"))) {sym = '\\'; goto _2;}
    Error();
    return false;
_2:
    if(Word(QString("\'"))) {goto _end;}
    Error();
    return false;
_end:
    CharValue* pVal = new CharValue(sym);
    *ppVal = pVal;
    qDebug() << "Char = " << sym << "Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Положительное
bool RigParser::Positive(QString& str) {
    QChar sym;
//_0:
    if(Digit(sym)) {str = sym; goto _1;}
    return false;
_1:
    if(Digit(sym)) {str += sym; goto _1;}
    goto _end;
_end:
    qDebug() << "Positive = "<< str << ". Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Буква
bool RigParser::Letter(QChar& sym) {

    //sym = (*it)[column];
    sym = GetSymbol(column);

//    int size = it->size();
    if(sym.isLetter()) {
//        if(column >= size) {
//            qDebug() << "Letter Rool is successful in (" << line << "," << column << ")";
//            return true;
//        }
        column++;
        qDebug() << "Letter=" << sym << " Rool is successful in (" << line << "," << column << ")";
        return true;
    }
    return false;
}

//=================================================================================================
// Цифра
bool RigParser::Digit(QChar& sym) {
    //QString digStr = "0123456789";
    //int i = digStr.indexOf((*it)[column]);   // Получение позиции первой встречи
    //if(i != -1) {
    //sym = (*it)[column];
    sym = GetSymbol(column);
    if(sym.isDigit()) {
        column++;
        qDebug() << "Digit Rool is successful in (" << line << "," << column << ")";
        return true;
    }
    return false;
}

//=================================================================================================
// Пропуск
bool RigParser::Blank() {
//_0:
    QChar sym;
    sym=GetSymbol(column);
    //if((*it)[column] == QChar(' ') || (*it)[column] == QChar('\t') ) {column++; goto _1;}
    if(sym == QChar(' ') || sym == QChar('\t') ) {column++; goto _1;}
    return false;
_1:
    sym=GetSymbol(column);
    //if((*it)[column] == QChar(' ') || (*it)[column] == QChar('\t') ) {column++; goto _1;}
    if(sym == QChar(' ') || sym == QChar('\t') ) {column++; goto _1;}
    goto _end;
_end:
    qDebug() << "Blank Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Заданное слово
bool RigParser::Word(QString str) {
    int tmpCol = column;                // фиксация текущей позиции строки
    int i = it->indexOf(str, tmpCol);   // Получение позиции первой встречи
    if(tmpCol == i) { // Нужное слово начинается с нужной позиции
        column += str.size();   // Перемещение курсора на позицию следующей лексемы
        qDebug() << "Word (" << str << ") Rool is successful in (" << line << "," << column << ")";
        return true;
    }
    // При несовпадении выдается ложь
    return false;
}

//=================================================================================================
// Конец строки
bool RigParser::EndOfLine() {
    //try {
        if(column >= it->size() && it < text.constEnd()) {
            NextLine();
            if(it == text.constEnd()) endOfProgFlag = true;
            qDebug() << "EndOfLine Rool is successful in (" << line << "," << column << ")";
            return true;
        }
        return false;
    //}
    //catch (EndOfProg) {
        //qDebug() << "The end of prog achived";
        //endOfProgFlag = true;
        //return true;
    //}
}

//=================================================================================================
// Переход на следующую строку
void RigParser::NextLine() {
    it++;
    //if(it == text.constEnd()) {
        //qDebug() << "End of RIG";
        //throw EndOfProg();
    //}
    line++;
    column = 0;
    return;
}

//=================================================================================================
void RigParser::Error(QString message) {
    QString tmpStr;
    //QString str = "Error (" + tmpStr.setNum(line) + ", " + tmpStr.setNum(column) + "): " + message;

    QString str = "Error (";
    str += tmpStr.setNum(line);
    str += ", ";
    str += tmpStr.setNum(column);
    str += "): " ;
    str += message;

    qDebug() << str;
    return;
}

//=================================================================================================
// Загрузка текста РИГ
bool RigParser::Load(QString fileName) {
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        QString str;
        while(!stream.atEnd()) {
            str = stream.readLine();
            text << str;
        }
        if(stream.status() != QTextStream::Ok) {
            qDebug() << "File reading error!!!";
            return false;
        }
        file.close();
        return true;
    }
    return false;
}

//=================================================================================================
// Тестовый вывод загруженного текста
void RigParser::TestOut() {
    QStringList::const_iterator it;
    for (it = text.constBegin(); it != text.constEnd(); ++it) {
        QString str = *it;
        qDebug() << str; // << "   len = " << str.size();
    }
}
//=================================================================================================
// Символ-геттер
QChar RigParser::GetSymbol(int col)
{
    if(col<it->size())
        return (*it)[col];
    else
        return QChar('\0');
}
//
bool RigParser::IsConstDataList(DataListValue** dataListValue)
{
    QChar sym;
    (*dataListValue)=new DataListValue();
    sym = GetSymbol(column);
    if(sym=='(')
    {
        column++;
        goto _1;
    }
    return false;
_1: if(IsValue())
    {
        (*dataListValue)->Append(lastValue);
        goto _2;
    }
    return false;
_2: sym = GetSymbol(column);
    if(sym==')')
    {
        column++;
        goto _end;
    }
    if(sym==',')
    {
        column++;
        goto _1;
    }
    return false;
_end:
    qDebug() << "DataList " << (*dataListValue)->GetAsStr() << " Rool is successful in (" << line << "," << column << ")";
    return true;
}
//
bool RigParser::IsConstParList(ParListValue** parListValue)
{
    QChar sym;
    (*parListValue)=new ParListValue();
    sym = GetSymbol(column);
    if(sym=='[')
    {
        column++;
        goto _1;
    }
    return false;
_1: if(IsValue())
    {
        (*parListValue)->Append(lastValue);
        goto _2;
    }
    return false;
_2: sym = GetSymbol(column);
    if(sym==']')
    {
        column++;
        goto _end;
    }
    if(sym==',')
    {
        column++;
        goto _1;
    }
    return false;
_end:
    qDebug() << "ParList " << (*parListValue)->GetAsStr() << " Rool is successful in (" << line << "," << column << ")";
    return true;
}
