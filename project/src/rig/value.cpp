#include "value.h"

//=================================================================================================
//
Value::Value() {
}
Value* Value::ParGetElement(int elementNum)
{
    if(elementNum)
        return new ErrorValue(BOUNDERROR);
    return this;
}



//=================================================================================================
// Получение текстового представления логического значения
QString BoolValue::GetAsStr() {
    QString str;
    str = value ? "true" : "false";
    return str;
}
//=================================================================================================
// Тестовый вывод логического значения
void BoolValue::TestOut(QTextStream& out) {
    out << "type is bool, value = " << GetAsStr() << endl;
}
//=================================================================================================
// Клонировщик
Value* BoolValue::Clone()
{
    return new BoolValue(value);
}



// Получение текстового представления целочисленного значения
QString IntValue::GetAsStr()
{
    QString str="";
    str = QString("%1").arg(value);
    return str;
}
// Тестовый вывод целочисленного значения
void IntValue::TestOut(QTextStream& out)
{
    out << "type is int, value = " << GetAsStr() << endl;
}
// Клонировщик
Value* IntValue::Clone()
{
    return new IntValue(value);
}



// Получение текстового представления действительного значения
QString FloatValue::GetAsStr()
{
    QString str;
    str = QString::number(value,'f',12);

    //а так мы режем наши вещественные константы - текстовые их представления
    //чтоб и точность не терялась, и лишних хвостовых нулей не было
    int zeroPos=str.size()-1;
    while(zeroPos>0 && str[zeroPos]=='0' && str[zeroPos-1]=='0')
        zeroPos--;
    //исключение - после точки хоть один символ да надо, пусть даже и ноль
    //исключение 2 - мы даже не сдвинулись по строке, ни одного хвостонуля не было
    if(str[zeroPos-1]=='.' || zeroPos==str.size()-1)
        zeroPos++;    
    str.truncate(zeroPos);

    return str;
}
// Тестовый вывод действительного значения
void FloatValue::TestOut(QTextStream& out)
{
    out << "type is float, value = " << GetAsStr() << endl;
}
// Клонировщик
Value* FloatValue::Clone()
{
    return new FloatValue(value);
}



// Получение текстового представления символьного значения
QString CharValue::GetAsStr()
{
    QString str = QString("\'");
    if(value == '\n') str += "\\n";
    else if(value == '\r') str += "\\r";
    else if(value == '\t') str += "\\t";
    else if(value == '\'') str += "\\\'";
    else if(value == '\"') str += "\\\"";
    else if(value == '\\') str += "\\\\";
    else str += value;
    str += QString("\'");
    return str;
}
// Тестовый вывод символа
void CharValue::TestOut(QTextStream& out)
{
    out << "type is character, value = " << GetAsStr() << endl;
}
// Клонировщик
Value* CharValue::Clone()
{
    return new CharValue(value);
}


// Формирование текстового значения для специального обозначения
QString GetAsStr(SpecType value)
{
    QString str;
    switch(value) {
    case dotSpec:        str = ".";        break;  // - пустое или сигнальное значение
    case addSpec:        str = "+";        break;  // - плюс
    case minusSpec:      str = "-";        break;  // - минус
    case astSpec:        str = "*";        break;  // - звездочка
    case slashSpec:      str = "/";        break;  // - слэш
    case percentSpec:    str = "%";        break;  // - процент
    case dotsSpec:       str = "..";       break;  // - многоточие
    case dupSpec:        str = "dup";      break;  // - дубликаты
    case ltSpec:         str = "<";        break;  // - меньше
    case leSpec:         str = "<=";       break;  // - меньше или равно
    case eqSpec:         str = "=";        break;  // - равно
    case neSpec:         str = "!=";       break;  // - не равно
    case geSpec:         str = ">=";       break;  // - больше или равно
    case gtSpec:         str = ">";        break;  // - больше
    case vertSpec:       str = "|";        break;  // - вертикальная черта
    case questSpec:      str = "?";        break;  // - вопрос
    case sharpSpec:      str = "#";        break;  // - решетка
    case trueSpec:       str = "true";     break;  // - истина
    case falseSpec:      str = "false";    break;  // - ложь
    case typeSpec:       str = "type";     break;  // - тип
    case valSpec:        str = "value";    break;  // - значение
    case boolSpec:       str = "bool";     break;  // - булевый
    case charSpec:       str = "char";     break;  // - символьный
    case intSpec:        str = "int";      break;  // - целый
    case floatSpec:      str = "float";    break;  // - действительный
    case strSpec:        str = "string";   break;  // - строковый
    case dalistSpec:     str = "()";       break;  // - список данных
    case palistSpec:     str = "[]";       break;  // - параллельный список
    case delistSpec:     str = "{}";       break;  // - задержанный список
    case aslistSpec:     str = "<()";      break;  // - асинхронный список
    case signalSpec:     str = "signal";   break;  // - асинхронный список
    case errorSpec:      str = "error";    break;  // - ошибка
    default:             str = "Undefined spec type";
    }
    return str;
}
// Получение текстового представления специального значения
QString SpecValue::GetAsStr()
{
    return ::GetAsStr(value);
}
// Тестовый вывод специального обозначения
void SpecValue::TestOut(QTextStream& out)
{
    out << "type is spec, value = " << GetAsStr() << endl;
}
// Клонировщик
Value* SpecValue::Clone()
{
    return new SpecValue(value);
}



// Получение текстового представления задержанной константы
QString DelayValue::GetAsStr() {
    QString str;
    str = "{" + QString::number(number) + "}" + QString::number(link);
    return str;
}
// Тестовый вывод задержанной константы
void DelayValue::TestOut(QTextStream& out) {
    out << "type is delay const, value = {" << QString::number(number) << "}" << QString::number(link) << endl;
}
// Клонировщик
Value* DelayValue::Clone()
{
    return new DelayValue(number,link,EPID);
}



// Формирование текстового значения для ошибки
QString GetAsStr(ErrorType value)
{
    QString str;
    switch(value) {
    case BASEFUNCERROR: str="BASEFUNCERROR"; break;     // 0: общая ошибка
    case TYPEERROR:     str="TYPEERROR"; break;         // 1: ошибка получения типа
    case BOUNDERROR:    str="BOUNDERROR"; break;        // 2: выход за границы массива
    case INTERROR:      str="INTERROR"; break;          // 3: ошибка при выполнении целочисленной операции
    case REALERROR:     str="REALERROR"; break;         // 4: ошибка при выполнении операции с плавающей точкой
    case ZERODIVIDE:    str="ZERODIVIDE"; break;        // 5: деление на ноль
    case VALUEERROR:    str="VALUEERROR"; break;		// 6: ошибка функции value
    default:            str="Unknown error type";
    }
    return str;
}
// Получение текстового представления ошибки
QString ErrorValue::GetAsStr()
{
    return ::GetAsStr(value);
}
// Тестовый вывод ошибки
void ErrorValue::TestOut(QTextStream& out)
{
    out << "type is error, value = " << GetAsStr() << endl;
}
// Клонировщик
Value* ErrorValue::Clone()
{
    return new ErrorValue(value);
}



// Получение текстового представления внешней сущности
QString ExternValue::GetAsStr()
{
    QString str;
    str = QString::number(value);
    return str;
}
// Тестовый вывод внешней сущности
void ExternValue::TestOut(QTextStream& out)
{
    out << "type is extern value, externId = " << GetAsStr() << endl;
}
// Клонировщик
Value* ExternValue::Clone()
{
    return new ExternValue(value,EPID);
}



// Получение текстового представления заглушки
QString MockValue::GetAsStr() {
    return "mock";
}
// Тестовый вывод внешней сущности
void MockValue::TestOut(QTextStream& out) {
    out << "type is mock value" << endl;
}
// Клонировщик
Value* MockValue::Clone()
{
    return new MockValue();
}



// Получение текстового представления заглушки
QString NullValue::GetAsStr() {
    return "NULL";
}
// Тестовый вывод внешней сущности
void NullValue::TestOut(QTextStream& out) {
    out << "type is null value" << endl;
}
// Клонировщик
Value* NullValue::Clone()
{
    return new NullValue();
}



// Добавление элемента в список
void ListValue::Append(Value *x)
{
    elements.push_back(x);
}
void ListValue::SetElement(Value *x, int index)
{
    int oldSize=elements.size();
    if(oldSize<=index)
    {
        elements.resize(index+1);
        for(int i=oldSize;i<elements.size();i++)
            elements[i]=new NullValue();
    }
    elements[index]=x;
}


DataListValue::DataListValue(int base[])
{
    valueType = DataListValueType;
}
DataListValue::DataListValue(QString string)
{
    valueType = DataListValueType;
    int strLen = string.length();
    for(int i=0;i<strLen;i++)
        Append(new CharValue(string[i]));
}


// Получение текстового представления списка данных
QString DataListValue::GetAsStr()
{
    //строки нужно визуализировать несколько иначе, а то нечитаемо
    //но строка ли мы вообще?
    bool isString = true;
    int elementsSize = elements.size();
    for (int i=0;i<elementsSize;i++)
        if (elements[i]->GetValueType()!=CharValueType)
            isString = false;
    if (!elementsSize)
        isString = false;

    QString str="(";

    if (!isString)  //так мы визуализируем просто массивы
    {
        if(elementsSize)
            str+=elements[0]->GetAsStr();
        for(signed int i=1;i<elementsSize;i++)
        {
            str+=",";
            str+=elements[i]->GetAsStr();
        }

    }
    else        // а так - символьные строки
    {
        str+="\"";
        for(signed int i=0;i<elementsSize;i++)
            str+=((CharValue*)elements[i])->GetCharValue();
        str+="\"";
    }

    str+=")";
    return str;
}
// Тестовый вывод списка данных
void DataListValue::TestOut(QTextStream& out)
{
    out << "type is datalist, value = " << GetAsStr() << endl;
}
// Клонировщик
Value* DataListValue::Clone()
{
    DataListValue* newDataListValue=new DataListValue();
    for(int i=0;i<elements.size();i++)
        newDataListValue->Append(elements[i]->Clone());
    return newDataListValue;
}
// Удаление ненужных сигналов (.)
void DataListValue::ClearSignals()
{
    bool isNonSignals=false;        // есть ли не-сигналы?
    bool isSignals=false;           // есть ли сигналы?
    int count=elements.size();
    for(int i=0;i<count;i++)
    {
        if(elements[i]->GetValueType()==SpecValueType)
        {
            if(((SpecValue*)elements[i])->GetSpecValue()==dotSpec)
                isSignals=true;
            else
                isNonSignals=true;
        }
        else
            isNonSignals=true;
    }
    if(isNonSignals)        //не-сигналы есть
    {
        if(isSignals)       //и сигналы есть. Надо сигналы удалить
        {
            for(int i=count-1;i>=0;i--)
                if(elements[i]->GetValueType()==SpecValueType)
                    if(((SpecValue*)elements[i])->GetSpecValue()==dotSpec)
                        elements.erase(elements.begin()+i);
        }
        else                //а сигналов нет. Все норм, делать ничего не надо.
            ;
    }
    else                    //не-сигналов нет
    {
        if (isSignals)      //а сигналы есть. Должен остаться только один
            elements.resize(1);
        else                //и сигналов нет. Список-то пустой! Делать ничего не надо
            ;
    }

}



// Получение текстового представления параллельного списка
QString ParListValue::GetAsStr()
{
    QString str="[";
    if(elements.size())
        str+=elements[0]->GetAsStr();
    for(signed int i=1;i<elements.size();i++)
    {
        str+=",";
        str+=elements[i]->GetAsStr();
    }
    str+="]";
    return str;
}
// Тестовый вывод параллельного списка
void ParListValue::TestOut(QTextStream& out)
{
    out << "type is parlist, value = " << GetAsStr() << endl;
}
void ParListValue::Append(Value *x)
{
    elements.push_back(x);
    elementInputs.push_back(-1);
    readyIndexes.push_back(elements.size()-1);
    elementUIDS.push_back(curUID);
    curUID++;
}
void ParListValue::SetElement(Value *x, int input)
{
    if (x->GetValueType()==ParListValueType)    //парлисты в парлистах самораскрываются!
    {
        ParListValue* xParList=(ParListValue*)x;
        int xsize=xParList->GetElementListSize();
        for(int index=0;index<xsize;index++)
            SetElement(xParList->ParGetElement(index),input);
        return;
    }


    int oldSize=elements.size();    
    //первый проход по массиву
    //находим либо пустое место, либо место с которого начинаются гости с более старшего входа
    for (int index=0;index<oldSize;index++)
    {
        if(elements[index]->GetValueType()==NullValueType || elements[index]->GetValueType()==MockValueType)
        {
            if(elementInputs[index]==input)
            {
                elements[index]=x;
                elementInputs[index]=input;
                readyIndexes.push_back(index);
                elementUIDS[index]=curUID;
                curUID++;
                return;
            }
        }
        if (elementInputs[index]>input)
        {
            elements.insert(index,x);
            elementInputs.insert(index,input);
            for(int index2=0;index2<readyIndexes.size();index2++)
                if(readyIndexes[index2]>=index)
                    readyIndexes[index2]++;
            readyIndexes.push_back(index);
            elementUIDS.insert(index,curUID);
            curUID++;
            return;
        }
    }

    //массив мы прошли, места так и не нашли?
    //пушбечим
    elements.push_back(x);
    elementInputs.push_back(input);
    readyIndexes.push_back(elements.size()-1);
    elementUIDS.push_back(curUID);
    curUID++;
}

//возвращаем весь список
QVector<Value*>* ParListValue::GetAllList()
{
    return &elements;
}
//для константного (или просто передающегося готовым) параллельного списка
//порядок поступления элементов не важен
//посему предполагаем, что поступали они по порядку
void ParListValue::ConstantReadyFill()
{
    readyIndexes.clear();
    for(int i=0;i<elements.size();i++)
        readyIndexes.push_back(i);
}
Value* ParListValue::ParGetElement(int elementNum)
{
    if(elementNum>=elements.size())
        return new NullValue();
    return elements[elementNum];
}
int ParListValue::GetReadyIndex(int offset)
{
    if(offset>readyIndexes.size())
    {
        qDebug() << " Error! Offset for readyIndexes is too big!";
        return NULL;
    }
    int index=readyIndexes[offset];
    return index;
}


// Клонировщик
Value* ParListValue::Clone()
{
    ParListValue* newParListValue=new ParListValue();
    for(int i=0;i<elements.size();i++)
        newParListValue->Append(elements[i]->Clone());
    return newParListValue;
}
int ParListValue::GetUID(int elemNum)
{
    if(elementUIDS.size()>elemNum)
    {
        return elementUIDS[elemNum];
    }
    return -1;
}



//конструктор асинхронного списка
AsyncListValue::AsyncListValue()
{
    valueType=AsyncListValueType;
    head=NULL;
    tail=NULL;
}
// Получение текстового представления значения
QString AsyncListValue::GetAsStr()
{
    QString rez="<-(";
    AsyncListValue *index=this;
    while(index!=NULL && index->head!=NULL)
    {
        rez+=index->GetHead()->GetAsStr()+",";
        index=index->GetTail();
    }
    rez+=")->";
    return rez;
}
// Тестовый вывод специального обозначения
void AsyncListValue::TestOut(QTextStream& out)
{
    out << "type is asynclist, value = " << GetAsStr();
}
// Как-бы клонировщик
Value* AsyncListValue::Clone()
{
    AsyncListValue* newAsyncList=new AsyncListValue;
    if(head!=NULL)
        newAsyncList->SetHead(head->Clone());
    if(tail!=NULL)
        newAsyncList->SetTail((AsyncListValue*)(tail->Clone()));
    return newAsyncList;
}
void AsyncListValue::Append(Value *newValue)
{
    if(head==NULL)
    {
        head=newValue;
        tail=new AsyncListValue();
    }
    else
        tail->Append(newValue);
}
