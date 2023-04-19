#ifndef DFG_H
#define DFG_H

#include <QString>
#include <vector>
#include <list>

namespace dfg {
  enum Slots { Data, Func };

  struct Inode {
    enum class Type {
      ext, loc, bif, act
    };

    Type type;
    QString value;
    unsigned int delay;

    struct AttachedNodeData {
      int intValue;
      QString strValue;
      double doubleValue;
      AttachedNodeData(int _intValue = 0,
                       QString _strValue = "",
                       double _doubleValue = 0.0)
        : intValue(_intValue), strValue(_strValue), doubleValue(_doubleValue) {
      }
    } attached;

    std::vector<Inode*> useDefRefs; //! индекс элемента = номер слота
    std::list<Inode*> def_useRefs;

    Inode(const Type _type, const QString _value, const int _delay = 0)
      : type(_type), value(_value), delay(_delay) {
    }
  };

  struct DFG {
    QString function_name;
    std::list<Inode*> nodes;

    DFG(QString _function_name) : function_name(_function_name){
    }
  };

  DFG* import_dfg(QString path, QString functionName);
  void export_dfg(QString _path, DFG*dfg);
}

#endif // DFG_H
