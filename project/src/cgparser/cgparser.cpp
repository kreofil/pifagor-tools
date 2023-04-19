#include "cgparser.h"

//=================================================================================================
// Конструктор
CgParser::CgParser(Cg* _pCg, QString _inFile): pCg(_pCg), inFile(_inFile), flag(true) {
    endOfCgFlag = false;
}
CgParser::CgParser(Cg* _pCg, QString cgText, bool uselessFlag): pCg(_pCg), inFile("cgparserTemp.txt"), flag(true) {


    QFile file("cgparserTemp.txt");
    if(file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        stream << cgText;
        if(stream.status() != QTextStream::Ok) {
            qDebug() << "File writing error!!!";
        }
        file.close();
    }
    else
    {
        qDebug() << "cgparserTemp.txt error!!!";
    }


    endOfCgFlag = false;
}
//=================================================================================================
// Функция, запускающая парсер после всех начальных установок
bool CgParser::Start() {
    bool result=false;
    if(Load(inFile)) {
        result=true;
        TestOut();
        it = text.constBegin();
        line = column = 0;
        try {
            if(this->IsCg(*pCg) && flag) {
                qDebug() << "OK";
            } else {
                result=false;
             Error("Invalid CG");
            }
        }
        catch (EndOfCg) {
            qDebug() << "The end of CG achived";
        }
    }
    return result;
}

//-------------------------------------------------------------------------------------------------
// Правила, описывающие синтаксис УГ
//-------------------------------------------------------------------------------------------------

//=================================================================================================
// Управляющий граф
bool CgParser::IsCg(Cg& cg) {
//_0:
    QString str;
    if(Name(str)) { cg.SetName(str); goto _1;}
    return false;
_1:
    if(EndOfLine()) {goto _2;}
    Error("End of line proposed");
_2:
    if(CgNodes(cg)) {goto _3;}
    return false;
_3:
    if(Signals(cg)) {goto _4;}
    goto _end;
_4:
    if(DLinks(cg)) {goto _end;}
    goto _end;
_end:
    qDebug() << "IsCg Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Описание узлов УГ
bool CgParser::CgNodes(Cg& cg) {
    CtrlNode* pCtrlNode = 0;
//_0:
    if(CgTitle()) {goto _1;}
    return false;
_1:
    if(CgNode(&pCtrlNode)) {
        cg.Append(pCtrlNode);
        goto _2;
    }
    Error("Invalid node of control graph");
    return false;
_2:
    if(endOfCgFlag) {
        //throw EndOfCg();
        goto _end;
    }
    if(CgNode(&pCtrlNode)) {
        cg.Append(pCtrlNode);
        goto _2;
    }
    goto _end;
_end:
    qDebug() << "CgNodes Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Описание сигналов
bool CgParser::Signals(Cg& cg) {
//_0:
    if(STitle()) {goto _1;}
    return false;
_1:
    if(endOfCgFlag) {
        //throw EndOfCg();
        goto _end;
    }
    if(isSignal(cg)) {goto _1;}
    goto _end;
_end:
    qDebug() << "Signals Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Описание внешних связей
bool CgParser::DLinks(Cg& cg) {
//_0:
    if(DTitle()) {goto _1;}
    return false;
_1:
    if(endOfCgFlag) {
        //throw EndOfCg();
        goto _end;
    }
    if(DLink(cg)) {goto _1;}
    goto _end;
_end:
    qDebug() << "DLinks Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Заголовок управляющего графа
bool CgParser::CgTitle() {
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
    if(Word(QString("automat"))) {goto _5;}
    Error("<automat> word proposed");
    return false;
_5:
    if(Blank()) {goto _6;}
    Error("Blanks proposed");
    return false;
_6:
    if(Word(QString("inode"))) {goto _7;}
    Error("<inode> word proposed");
    return false;
_7:
    if(Blank()) {goto _8;}
    Error();
    return false;
_8:
    if(Word(QString("links"))) {goto _9;}
    Error("<links> word proposed");
    return false;
_9:
    if(EndOfLine()) {goto _end;}
    Error("End of line proposed");
    return false;
_end:
    qDebug() << "CgTitle Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Вершина управляющего графа
bool CgParser::CgNode(CtrlNode** ppCtrlNode) {
    QString str;
    QString str1;
    QString str2;
    QString str3;
    CtrlNode* pCtrlNode = 0;
    Automat* pAutomat = 0;
    AutomatState zeroState(0),curState;
    //CtrlNode* pCtrlNode = 0;
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
    if(isAutomat(&pAutomat)) {goto _5;}
    Error("Automat proposed");
    return false;
_5:
    if(Positive(str))
    {
        curState.state=str.toInt();
        goto _6;
    }
    Error("Positive number proposed");
    return false;
_6:
    if(Blank()) {goto _7;}
    Error("Blanks proposed");
    return false;
_7:
    if(Positive(str3)) {
        // Формирование узла УГ
        pCtrlNode = new CtrlNode(str2.toInt(), pAutomat, curState, str3.toInt());
        goto _8;
    }
    Error("Positive number proposed");
    return false;
_8:
    if(Blank()) {goto _9;}
    if(EndOfLine()) {goto _end;}
    Error("Blanks or End of line or End of file proposed");
    return false;
_9:
    if(isLinks(*pCtrlNode)) {goto _10;}
    if(EndOfLine()) {goto _end;}
    Error("Links or End of line or End of file proposed");
    return false;
_10:
    if(EndOfLine()) {goto _end;}
    Error("End of line or file proposed");
    return false;
_end:
    //pActor->SetDelay(str2.toInt());
    //pActor->pos.Set(p);
    //pActor->pos.Set(10,20,30,40);
    //*ppActor = pActor;
    *ppCtrlNode = pCtrlNode;
    qDebug() << "CgNode Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Заголовок списка сигналов
bool CgParser::STitle() {
//_0:
    if(Word(QString("Signals:"))) {goto _1;}
    return false;
_1:
    if(Blank()) {goto _2;}
    Error("Blanks proposed");
    return false;
_2:
    if(Word(QString("(Number, Node, Input)"))) {goto _3;}
    Error("<(Number, Node, Input)> words proposed");
    return false;
_3:
    if(EndOfLine()) {goto _end;}
    Error("End of line proposed");
    return false;
_end:
    qDebug() << "STitle Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Сигнал
bool CgParser::isSignal(Cg& cg) {
    QString str1;
    QString str2;
//_0:
    if(Positive(str1)) {goto _1;}
    return false;
_1:
    if(Blank()) {goto _2;}
    Error("Blanks proposed");
    return false;
_2:
    if(Positive(str1)) {goto _3;}
    Error("Positive number proposed");
    return false;
_3:
    if(Blank()) {goto _4;}
    Error("Blanks proposed");
    return false;
_4:
    if(Positive(str2)) {goto _5;}
    Error("Positive number proposed");
    return false;
_5:
    if(EndOfLine()) {goto _end;}
    Error("End of line or file proposed");
    return false;
_end:
    Signal* pSignal = new Signal(str1.toInt(), str2.toInt());
    cg.Append(pSignal);
    qDebug() << "isSignal Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Заголовок динамических связей
bool CgParser::DTitle() {
//_0:
    if(Word(QString("Dynamic links:"))) {goto _1;}
    return false;
_1:
    if(Blank()) {goto _2;}
    Error("Blanks proposed");
    return false;
_2:
    if(Word(QString("(Number, Delay list, Node)"))) {goto _3;}
    Error("<(Number, Delay list, Node)> word proposed");
    return false;
_3:
    if(EndOfLine()) {goto _end;}
    Error("End of line proposed");
    return false;
_end:
    qDebug() << "DTitle Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Динамическая связь
bool CgParser::DLink(Cg& cg) {
    QString str1;
    QString str2;
//_0:
    if(Positive(str1)) {goto _1;}
    return false;
_1:
    if(Blank()) {goto _2;}
    Error("Blanks proposed");
    return false;
_2:
    if(Positive(str1)) {goto _3;}
    Error("Positive number proposed");
    return false;
_3:
    if(Blank()) {goto _4;}
    Error("Blanks proposed");
    return false;
_4:
    if(Positive(str2)) {goto _5;}
    Error("Positive number proposed");
    return false;
_5:
    if(EndOfLine()) {goto _end;}
    Error("End of line or file proposed");
    return false;
_end:
    DynLink* pDynLink = new DynLink(str1.toInt(), str2.toInt());
    cg.Append(pDynLink);
    qDebug() << "DLink Rool is successful in (" << line << "," << column << ")";
    return true;
}


//=================================================================================================
// Автомат
bool CgParser::isAutomat(Automat** ppAutomat) {
//_0:
    if(Word(QString(":"))) {*ppAutomat = new InterpAutomat(); goto _1;}
    if(Word(QString("(---)"))) {*ppAutomat = new DlistAutomat(); goto _1;}
    if(Word(QString("[---]"))) {*ppAutomat = new PlistAutomat(); goto _1;}
    if(Word(QString("<(--)"))) {*ppAutomat = new AlistAutomat(); goto _1;}
    if(Word(QString("arg"))) {*ppAutomat = new ArgAutomat(); goto _1;}
    if(Word(QString("return"))) {*ppAutomat = new RetAutomat(); goto _1;}
    return false;
_1:
    if(Word(QString(","))) {goto _end;}
    Error("Comma proposed");
    return false;
_end:
    qDebug() << "isAutomat Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Динамические связи
bool CgParser::isLinks(CtrlNode& cn) {
    QString str;
//_0:
    if(Word(QString("links:"))) {goto _1;}
    return false;
  _1:
    if(isLink(cn)) { goto _2;}
    Error("Link Rool proposed");
    return false;
  _2:
    if(isLink(cn)) {goto _2;}
    goto _end;
  _end:
    qDebug() << "isLinks Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Динамическая связь
bool CgParser::isLink(CtrlNode& cn) {
    QString str1;
    QString str2;
//_0:
    if(Positive(str1)) {goto _1;}
    return false;
  _1:
    if(Word(QString(","))) {goto _2;}
    Error("Comma proposed");
    return false;
  _2:
    if(Positive(str2)) {goto _3;}
    Error("Positive number proposed");
    return false;
  _3:
    if(Word(QString(";"))) {goto _end;}
    Error("Semicolon proposed");
    return false;
_end:
    cn.Append(str1.toInt(),str2.toInt());
    qDebug() << "isLink Rool is successful in (" << line << "," << column << ")";
    return true;
}

//=================================================================================================
// Положительное
bool CgParser::Positive(QString& str) {
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
// Цифра
bool CgParser::Digit(QChar& sym) {
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
bool CgParser::Blank() {
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
bool CgParser::Word(QString str) {
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
bool CgParser::EndOfLine() {
    //try {
        if(column >= it->size() && it < text.constEnd()) {
            NextLine();
            if(it == text.constEnd()) endOfCgFlag = true;
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
void CgParser::NextLine() {
    it++;
    //if(it == text.constEnd()) {
        //qDebug() << "End of RIG";
        //throw EndOfCg();
    //}
    line++;
    column = 0;
    return;
}

//=================================================================================================
void CgParser::Error(QString message) {
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
// Загрузка текста УГ
bool CgParser::Load(QString fileName) {
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
void CgParser::TestOut() {
    QStringList::const_iterator it;
    for (it = text.constBegin(); it != text.constEnd(); ++it) {
        QString str = *it;
        qDebug() << str; // << "   len = " << str.size();
    }
}
//=================================================================================================
// Символ-геттер
QChar CgParser::GetSymbol(int col) {
    if(col < it->size())
        return (*it)[col];
    else
        return QChar('\0');
}











//=================================================================================================
// Имя
bool CgParser::Name(QString& str) {
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
bool CgParser::Id(QString& str) {
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
// Буква
bool CgParser::Letter(QChar& sym) {

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
