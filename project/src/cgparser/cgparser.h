#ifndef CGPARSER_H
#define CGPARSER_H

#include "../cg/cg.h"
#include <fstream>

//=================================================================================================
// Признаки автоматов УГ
//enum Auto {interp, dlist, plist, alist,ret};

//=================================================================================================
// Класс, определяющий парсер, осуществляющий разбор текстового представления РИГ.
class CgParser {
    class EndOfCg {};

    Cg* pCg;          // Указатель на подключаемый экземпляр УГ
    QString inFile;     // Имя входного файла
    QStringList text;   // Список строк, содержащий текстовое представление УГ
    QStringList::const_iterator it; // Итератор для обхода текста УГ
    QString value;      // Значение накапливаемой конструкции (лексемы)
    int line;           // Номер текущей строки текста
    int column;         // Номер текущей позиции в строке
    //QMap<QString,SpecType> elementaryValues;     // Специальные знаки
    bool flag;          // Флаг, определяющий, что это ожидаемый файл
    bool endOfCgFlag; // Флаг определяющий завершение текста УГ
public:
    // Конструктор.
    CgParser(Cg* _pCg, QString _inFile);
    CgParser(Cg* _pCg, QString cgText, bool uselessFlag);

    // Функция запуска парсера после всех начальных установок
    bool Start();

    //---------------------------------------------------------------------------------------------
    // Правила, описывающие синтаксис УГ
    //---------------------------------------------------------------------------------------------

    // Управляющий граф
    bool IsCg(Cg& cg);
    // Описание узлов УГ
    bool CgNodes(Cg& cg);
    // Синалы
    bool Signals(Cg& cg);
    // Динамические связи
    bool DLinks(Cg& cg);
    // Заголовок УГ
    bool CgTitle();
    // Узел УГ
    bool CgNode(CtrlNode** ppCtrlNode);
    // Заголовок сигналов
    bool STitle();
    // Сигнал
    bool isSignal(Cg& cg);
    // Заголовок динамических связей
    bool DTitle();
    // Динамическая связь
    bool DLink(Cg& cg);
    // Автомат
    bool isAutomat(Automat** ppAutomat);
    // Динамические связи
    bool isLinks(CtrlNode& cn);
    // Динамическая связь
    bool isLink(CtrlNode& cn);


    // Положительное
    bool Positive(QString& str);
    // Цифра
    bool Digit(QChar& sym);
    // Пропуск
    bool Blank();
    // Конец строки
    bool EndOfLine();
    // Искомое слово
    bool Word(QString str);
    // Имя
    bool Name(QString& str);
    // Идентификатор
    bool Id(QString& str);
    // Буква
    bool Letter(QChar& sym);

    //---------------------------------------------------------------------------------------------
    // Вспомогательные методы и утилиты
    //---------------------------------------------------------------------------------------------

    // Вывод информации об ошибке в УГ
    void Error(QString message = "Default error message");
    // Переход на следующую анализируемую строку
    void NextLine();
    // Загрузка текста УГ
    bool Load(QString fileName);
    // Тестовый вывод загруженного текста
    void TestOut();

    //Символ-геттер
    QChar GetSymbol(int col);
    // Возвращение указателя на УГ
    Cg* GetCg(){ return pCg; }
};

#endif // CGPARSER_H
