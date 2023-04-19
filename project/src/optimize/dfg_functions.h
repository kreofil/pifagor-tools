#ifndef DFG_FUNCTIONS_H
#define DFG_FUNCTIONS_H

#include "dfg.h"
#include <set>
#include <map>

namespace dfg {
  std::list<Inode*> find_nodes(DFG* dfg, Inode::Type type);
  std::list<Inode*> find_nodes(DFG* dfg, Inode::Type type, QString value);
  std::list<Inode*> find_nodes(DFG* dfg, Inode::Type type, unsigned int delay);

  Inode* find_single_node(DFG* dfg, Inode::Type type, QString value);

  std::set<Inode *> collect_use_def(DFG* dfg, Inode* node);

  std::map<Inode*, Inode*> clone(std::list<Inode*> what, DFG* to);

  void set_attached_value(DFG* dfg, Inode::AttachedNodeData value);

  QString type2string(Inode::Type type);

  //! удаляет узел what и заменяет все его использования на which
  void remove_node(DFG *dfg, Inode* what, Inode *which);

  template<typename T>
  void vector_append(std::vector<T> &to, std::vector<T> &from) {
    to.reserve(to.size()+from.size());
    to.insert(to.end(), from.begin(), from.end());
  }

  std::vector<std::list<Inode*> > get_multilevel_dfg(DFG* dfg);

  class CantGetNodeEx : std::runtime_error {
  public:
    CantGetNodeEx(const char* _what) : std::runtime_error(_what), m_what(_what) {
    }
    virtual const char* what()throw(){
      return m_what.c_str();
    }
  private:
    std::string m_what;
  };

  class NoSuchFunction : std::runtime_error {
  public:
    NoSuchFunction(const char* _what) : std::runtime_error(_what), m_what(_what) {
    }
    virtual const char* what()throw(){
      return m_what.c_str();
    }
  private:
    std::string m_what;
  };
}


#endif // DFG_FUNCTIONS_H
