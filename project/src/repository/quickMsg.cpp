#include "../repository/quickMsg.h"
#include "../repository/msgServer.h"
#include <QVariant>
/*
#ifdef Q_OS_LINUX
const char* codecNameOut = CODEC_LIN_UTF8;
const char* codecNameIn = CODEC_LIN_UTF8;
#endif

#ifdef Q_OS_WIN32
const char* codecNameOut = CODEC_WIN_IBM866;
const char* codecNameIn = CODEC_WIN_1251;
#endif
*/

// --- Конструктор -----------------------------------------------------
// enmType - тип сообщения (WRITE_MSG, WRITE_MSG_SILENT, WRITE_PRINT)
// strName - название библиотеки, класса, кнопки,...
QuickMsg::QuickMsg(enmMsg enmType, QString strName)
{
    this->enmType = enmType;
    this->strMacro = strName;
}

// --- Инициализация текущей функции -----------------------------------
// strMicro - название функции, процедуры
void QuickMsg::init(QString strMicro)
{
    this->strMicro = strMicro;
}

// --- Вывод сообщения об ошибке ---------------------------------------
// strText - сообщение
// intErr  - номер сообщения об ошибке
// intErl  - строка с ошибкой
void QuickMsg::write(QString strText, int intErr, int intErl)
{
    MsgServer* msgServer=MsgServer::getMsgServer();
    if(!(msgServer->checkNicety(this->strMacro)))
        return;
    QString strLine(intErl);
    QString strErr(intErr);

    if (this->strMicro != "")
    {
        this->writeSys(strText + " (#" + strErr + ")", this->strMacro +
                       "/" + this->strMicro + ": line " + strLine);
    }
    else
    {
        this->writeSys(strText, this->strMacro + ": line " + strLine);
    }
}

// --- Вывод информационного сообщения ---------------------------------
// strText - сообщение
void QuickMsg::write(QString strText)
{
    MsgServer* msgServer=MsgServer::getMsgServer();
    if(!(msgServer->checkNicety(this->strMacro)))
        return;
    if (this->strMicro != "")
    {
        this->writeSys(strText, this->strMacro + "/" + this->strMicro);
    }
    else
    {
        this->writeSys(strText, this->strMacro );
    }
}

// --- Проверка на пустоту ---------------------------------------------
// qVar       - проверяемая переменная
// intTypeMsg - тип сообщения
//              0 - переменная переданная в качестве входного параметра пуста
//              1 - не могу получить новый экземпляр класса ''strMsg''
//              2 - пользовательское сообщение (strMsg)
// strMsg     - если содержит пробел, то приводим к типу 2, "" - не сообщаем
bool QuickMsg::isNotNothing(QVariant qVar, int intTypeMsg, const char* strMsg)
{
    bool blnResult;
    QString strMsgX;
    QString strTemp;
    int intType;

    // значение по умолчанию
    blnResult = true;

    intType = qVar.userType();

    switch (intType)
    {
        case QVariant::Bool:        //1	- bool
            blnResult = qVar.toBool();
            break;
        case QVariant::Double:      //6	- double
            blnResult = (qVar.toDouble() > 0);
            break;
        case QVariant::Int:         //2	- int
            blnResult = (qVar.toInt() > 0);
            break;
        case QVariant::String:      //10 - QString
            strTemp = qVar.toString();
            blnResult = false;
            if (!strTemp.isEmpty() && !strTemp.isNull())
            {
                blnResult = true;
            }
            break;
        case QVariant::UserType:	//127 - пользовательские классы
            blnResult = !(!qVar.isValid() || qVar.isNull());
            break;
        case QVariant::Invalid:     //0	- no type
            blnResult = false;
            break;
        default:
            blnResult = false;      // неизвестный тип
    }

    // вывод сообщения
    if (blnResult == false)
    {
        strMsgX = QString(strMsg);
        if (!strTemp.isEmpty() && !strTemp.isNull())
        {
            // определяемся с содержанием сообщения
            if (strMsgX.contains(" "))
            {
                intTypeMsg = 2;
            }
            switch (intTypeMsg)
            {
                case 0:
                    strTemp = "Input param ''"+strMsgX+"'' is empty";
                case 1:
                    strTemp = "Can't get a new instance of the class ''"+strMsgX+"''";
                case 2:
                    strTemp = strMsg;
                default:
                    strTemp.setNum(intTypeMsg);
                    this->write("Input incorrect type of text msg ''"+strTemp+"''");
                    strTemp = strMsgX;
            }
            // выводим
            this->write(strMsgX);
        }
    }

    // возвращаем результат проверки
    return blnResult;
}

// --- Проверка на пустоту ---------------------------------------------
bool QuickMsg::isNotNothing(QString strText, int intTypeMsg, const char *strMsg)
{
    QVariant qVar;
    bool blnResult;

    qVar = strText;
    blnResult = this->isNotNothing(qVar, intTypeMsg, strMsg);

    // возвращаем результат проверки
    return blnResult;
}

// --- Проверка на пустоту ---------------------------------------------
bool QuickMsg::isNotNothing(QString strText, int intTypeMsg, QString strMsg)
{
    QVariant qVar;
    bool blnResult;
    const char *strTemp;

    //strTemp = strMsg.toAscii();
    strTemp = strMsg.toUtf8().constData();
    qVar = strText;
    blnResult = this->isNotNothing(qVar, intTypeMsg, strTemp);

    // возвращаем результат проверки
    return blnResult;
}

// --- Проверка на пустоту ---------------------------------------------
bool QuickMsg::isNotNothing(int intVar, int intTypeMsg, const char *strMsg)
{
    QVariant qVar;
    bool blnResult;

    qVar = intVar;
    blnResult = this->isNotNothing(qVar, intTypeMsg, strMsg);

    // возвращаем результат проверки
    return blnResult;
}

// --- Проверка на пустоту ---------------------------------------------
bool QuickMsg::isNotNothing(double dblVar, int intTypeMsg, const char *strMsg)
{
    QVariant qVar;
    bool blnResult;

    qVar = dblVar;
    blnResult = this->isNotNothing(qVar, intTypeMsg, strMsg);

    // возвращаем результат проверки
    return blnResult;
}

// --- Проверка на пустоту ---------------------------------------------
bool QuickMsg::isNotNothing(bool blnVar, int intTypeMsg, const char *strMsg)
{
    QVariant qVar;
    bool blnResult;

    qVar = blnVar;
    blnResult = this->isNotNothing(qVar, intTypeMsg, strMsg);

    // возвращаем результат проверки
    return blnResult;
}

// --- Замена подстроки в строке ---------------------------------------
QString QuickMsg::replaceSubString(QString strIn, QString strBad, QString strGood)
{
    QTextStream cout(stdout);
    QString strOut;

    // значение по умолчанию (ВНИМАНИЕ: никак не хочет создавать копию)
    strOut = strIn;

    // обрабатываем
    if (!strIn.isEmpty() || !strIn.isNull())
    {
        if (!strBad.isEmpty() || !strBad.isNull())
        {
            if (strGood.isNull())
            {
                strGood = "";
            }
            strOut  = strIn.replace(strBad, strGood);
        }
    }
    else
    {
        // выводим так, т.к. возможно зацикливание при вызове из writeSys
        /*
        QTextCodec::setCodecForTr(QTextCodec::codecForName(codecNameIn));
        cout.setCodec(codecNameOut);
        */
        cout << "Paramentr ''strIn'' is empty" << endl;
    }

    // возвращаем полученное значение
    return strOut;
}

// --- Вывод сообщения -------------------------------------------------
// strText - сообщение
bool QuickMsg::writeSys(QString strText, QString strSys)
{
    QTextStream cout(stdout);
    QString strMsg;
    //int intClick;     // раскомментировать в GUI
    bool blnResult;

    // значение по умолчанию
    blnResult = true;

    // формируем уточняющую строку на случай появления сбоя
    strSys = this->replaceSubString(strSys, "/", "\\");
    strMsg = "Подробности: " + strSys + ": " + strText;

    switch (this->enmType)
    {
        case WRITE_MSG:
        {
            // проверка выполнения на сервере
            if (this->isOnServer())
            {
                // меняем тип способа вывода, а он дальше сам дойдет до PRINT
                this->enmType = WRITE_PRINT;
                blnResult = this->writeSys(strText, strSys);
            }
            else
            {
                /* ЗАКОММЕНТИРОВАНО В СВЯЗИ С НЕДОСТУПНОСТЬЮ ИЗ КОНСОЛИ
                // выводим сообщение
                intClick = this->showDialog(BUTTON_OK, "", strText, strSys);
                switch (intClick)
                {
                    case QMessageBox::Ok:
                        blnResult = true;
                        break;
                    case QMessageBox::Cancel:
                        blnResult =  false;
                        break;
                    case QMessageBox::Yes:
                        blnResult =  true;
                        break;
                    case QMessageBox::No:
                        blnResult =  false;
                        break;
                    case QMessageBox::NoButton:
                        blnResult =  false;
                        break;
                    default:
                        this->writeInfo("Метод ''showDialog'' вернул неизвестное значение - ''"+QString(intClick)+"''");
                        blnResult =  false;
                }
                */
            }
            break;
        }
        case WRITE_PRINT:
        {
            /*
            QTextCodec::setCodecForTr(QTextCodec::codecForName(codecNameIn));
            cout.setCodec(codecNameOut);
            */
            cout << strSys + ": " + strText << endl;
            break;
        }
        default:
            this->write("Параметр ''intType'' содержит неизвестное значение - ''"+QString(this->enmType)+"''");
    }

    // возвращаем полученное значение
    return blnResult;
}

// ---- Непосредственное отображение диалога -----------------------------------
// enmButtonSet - набор кнопок (BUTTON_OK, BUTTON_OK_CANCEL, BUTTON_YES_NO)
// strTitle     - заголовок окна
// strText      - текст в окне
// strDetail    - текст в разделе "Подробнее"
int QuickMsg::showDialog(enmButton enmButtonSet, QString strTitle, QString strText, QString strDetail)
{
    int intResult;

    /* ЗАКОММЕНТИРОВАНО В СВЯЗИ С НЕДОСТУПНОСТЬЮ ИЗ КОНСОЛИ
    QMessageBox Dialog;

    // значение по умолчанию
    intResult = QMessageBox::NoButton;

    // определяемся с набором кнопок
    switch (enmButtonSet)
    {
        case BUTTON_OK:
            Dialog.setStandardButtons(QMessageBox::Ok);
            Dialog.setDefaultButton(QMessageBox::Ok);
            break;
        case BUTTON_OK_CANCEL:
            Dialog.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            Dialog.setDefaultButton(QMessageBox::Ok);
            break;
        case BUTTON_YES_NO:
            Dialog.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            Dialog.setDefaultButton(QMessageBox::Yes);
            break;
        default:
            this->writeInfo("В качестве параметра ''intButtonSet'' передано неизвестное значение - ''"+QString(intButtonSet)+"''");
            return intResult;
    }

    // прописываем текст в раздел "Подробнее"
    if (strDetail.isEmpty() == false)
    {
        Dialog.setDetailedText(strDetail);
    }

    // определяемся с заголовком окна
    if (strTitle.isEmpty())
    {
        strTitle = "Диалоговое окно";
    }

    // прописываем основную текстовку
    Dialog.setText(strTitle);
    Dialog.setInformativeText(strText);

    intResult = Dialog.exec();
    */

    // возвращаем результат
    return intResult;
}

// --- Код выполняется на сервере (нет поддержки UI)? -------------------------------
// Внимание: необходима реализация проверки
bool QuickMsg::isOnServer()
{
    return true;
}
