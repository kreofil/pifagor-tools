#ifndef EXTREF_H
#define EXTREF_H

#include <QtCore>
//#include <QString>

//=================================================================================================
// Класс, используемый для создания и хранения внешних ссылок
class ExtRef: public QObject {
    QString name;   // Имя внешней ссылки
    int index;      // Значение дескриптора, формируемое во время компоновки
public:
    // Конструктор с инициализацией имени внешней ссылки
    ExtRef(QString n): name(n), index(-1) {}

    // Получение имени внешней ссылки
    QString GetName() { return name; }

    // Тестовый вывод внешней ссылки
    void TestOut(QTextStream& out);
    QString GetAsStr();
};

#endif // EXTREF_H
