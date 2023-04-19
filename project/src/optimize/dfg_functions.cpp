#include "dfg_functions.h"
#include <set>
#include <map>
#include <QDebug>
using namespace dfg;

void collect_use_def_(DFG* dfg, std::set<Inode*> init_nodes, std::set<Inode *> &buff);

std::list<Inode*> dfg::find_nodes(DFG* dfg, Inode::Type type) {
  std::list<Inode*> result;

  for (Inode* node : dfg->nodes) {
    if (node->type == type)
      result.push_back(node);
  }

  return result;
}

std::list<Inode*> dfg::find_nodes(DFG* dfg, Inode::Type type, QString value) {
  std::list<Inode*> result;

  for (Inode* node : find_nodes(dfg, type)) {
    if (node->value == value)
      result.push_back(node);
  }

  return result;
}

std::list<Inode*> dfg::find_nodes(DFG* dfg, Inode::Type type, unsigned int delay) {
  std::list<Inode*> result;

  for (Inode* node : find_nodes(dfg, type)) {
    if (node->delay == delay)
      result.push_back(node);
  }

  return result;
}

Inode* dfg::find_single_node(DFG* dfg, Inode::Type type, QString value) {
  std::list<Inode*> result = find_nodes(dfg, type, value);
  if (result.size() != 1)
    throw CantGetNodeEx((QString::fromLatin1("node ") +
                        type2string(type) + " : " + value +
                        " - is not single").toLatin1());

  return *result.begin();
}

std::set<Inode*> dfg::collect_use_def(DFG* dfg, Inode* node) {
  std::set<Inode*> init_nodes = {node};
  std::set<Inode*> buff;
  collect_use_def_(dfg, init_nodes, buff);
  return buff;
}

void collect_use_def_(DFG* dfg, std::set<Inode*> init_nodes, std::set<Inode *> &buff) {
  if (init_nodes.empty())
    return;

  auto init_node_it = init_nodes.begin();

  for (Inode* node : (*init_node_it)->useDefRefs) {
    if (init_nodes.find(node) == init_nodes.end() && buff.find(node) == buff.end()) {
      init_nodes.insert(node);
    }
  }

  buff.insert(*init_node_it);
  init_nodes.erase(*init_node_it);

  collect_use_def_(dfg, init_nodes, buff);
}

std::map<Inode*, Inode*> dfg::clone(std::list<Inode*> what, DFG* to) {
  std::map<Inode*, Inode*> from_to_map;

  for (Inode *from_node : what) {
    Inode *to_node = new Inode(from_node->type, from_node->value, from_node->delay);
    to->nodes.push_back(to_node);
    from_to_map[from_node] = to_node;
  }

  for (Inode *from_node : what) {
    Inode *to_node = from_to_map[from_node];

    for (Inode *from_edge : from_node->def_useRefs) {
      Inode* to_edge = from_to_map[from_edge];
      to_node->def_useRefs.push_back(to_edge);
    }

    for (Inode *from_edge : from_node->useDefRefs) {
      Inode* to_edge = from_to_map[from_edge];
      to_node->useDefRefs.push_back(to_edge);
    }
  }

  return from_to_map;
}

void dfg::set_attached_value(DFG* dfg, Inode::AttachedNodeData value) {
  for (Inode *node : dfg->nodes) {
    node->attached = value;
  }
}

QString dfg::type2string(Inode::Type type) {
  switch (type) {
  case Inode::Type::ext: return "ext";
  case Inode::Type::loc: return "loc";
  case Inode::Type::bif: return "bif";
  case Inode::Type::act: return "act";
  }
  throw std::domain_error("bad Inode type");
}

void dfg::remove_node(DFG* dfg, Inode* what, Inode *which) {
  //qDebug() << "REMOVE: " << what << " -> " << what->value << endl;
  for (Inode* what_using : what->def_useRefs) {
    which->def_useRefs.push_back(what_using);
    std::replace(what_using->useDefRefs.begin(), what_using->useDefRefs.end(),
                 what, which);
  }
  dfg->nodes.remove(what);
  delete what;
}

int calc_multilevel_structure(std::list<Inode*>&nodes, Inode* goal) {
  if (goal->attached.intValue >= 0)
    return goal->attached.intValue;

  int max_child_depth = -1;
  for (Inode* node : goal->useDefRefs) {
    int depth = calc_multilevel_structure(nodes, node);
    if (depth > max_child_depth)
        max_child_depth = depth;
  }

  goal->attached.intValue = max_child_depth + 1;
  return goal->attached.intValue;
}

std::vector<std::list<Inode*> > dfg::get_multilevel_dfg(DFG* dfg) {
  // размечаем DFG начиная с узла return
  Inode* ret = find_single_node(dfg, Inode::Type::act, "return");

  set_attached_value(dfg, Inode::AttachedNodeData(-1, "", 0.));
  int dfg_depth = calc_multilevel_structure(dfg->nodes, ret);
  // 2) группируем узлы DFG по уровням
  std::vector<std::list<Inode*> > multilevel_dfg;
  multilevel_dfg.resize(dfg_depth+1);

  for (Inode* node : dfg->nodes) {
    int level_number = node->attached.intValue;
    if (level_number < 0)
      continue; // for unused code
    std::list<Inode*> &level = multilevel_dfg[level_number];
    level.push_back(node);
  }
  //qDebug() << multilevel_dfg;
  return multilevel_dfg;
};

