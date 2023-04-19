#include "dfg.h"
#include "bif.h"

#include <exception>
#include <map>

#include <QRegExp>
#include <QTextStream>
#include <QFile>
#include <QDir>

#include "dfg_functions.h"

using namespace dfg;

QString fileName(QString function_name) {
  QString result;
  result += "repository/" + function_name.replace(".", "/") + "/00.00/1.rig";
  return result;
}

//! временная структура для хранения пары <type, id> РИГ
struct EdgeNode {
  Inode::Type type;
  unsigned int id;

  EdgeNode(Inode::Type _type, unsigned int _id) : type(_type), id(_id) {
  }

  bool operator < (const EdgeNode& lhs) const {
    if (type == lhs.type)
      return id < lhs.id;
    return type < lhs.type;
  }
};

//! временная структура для хранения дуг, заданных парами <EdgeNode, EdgeNode>
struct __Edge {
  EdgeNode from, to;
  unsigned int slot;

  __Edge(EdgeNode _from, EdgeNode _to, unsigned int _slot)
    : from(_from), to(_to), slot(_slot) {
  }
};

enum class ImportState { Ext, Loc, Act };

void read_loc(QString fileString, ImportState &state,
              std::map<EdgeNode, Inode*> &nodeID2ptr,
              std::list<__Edge> &edges, DFG* &dfg);

void read_ext(QString fileString, ImportState &state,
              std::map<EdgeNode, Inode*> &nodeID2ptr, std::list<__Edge> &, DFG* &dfg);

void read_act(QString fileString, ImportState &,
              std::map<EdgeNode, Inode*> &nodeID2ptr,
              std::list<__Edge> &edges, DFG* &dfg);

DFG *dfg::import_dfg(QString path, QString functionName) {
  DFG* dfg = new DFG(functionName);

  std::map<EdgeNode, Inode*> nodeID2ptr;
  std::list<__Edge> edges;

  QFile file(fileName(functionName));
  if (false == file.open(QIODevice::ReadOnly | QIODevice::Text))
     throw std::runtime_error("can't open file");
  QTextStream ifst(&file);

  ImportState state = ImportState::Ext;

  while (false == ifst.atEnd()) {
    QString fileString = ifst.readLine();

    if (fileString.isEmpty())
      break;

    if (state == ImportState::Ext)
      read_ext(fileString, state, nodeID2ptr, edges, dfg);
    else if (state == ImportState::Loc)
      read_loc(fileString, state, nodeID2ptr, edges, dfg);
    else if (state == ImportState::Act)
      read_act(fileString, state, nodeID2ptr, edges, dfg);
  }

  // восстановление use-def дуг по индексам
  for (__Edge edge : edges) {
    Inode* to = nodeID2ptr[edge.to];
    Inode* from = nodeID2ptr[edge.from];
    to->useDefRefs[edge.slot] = from;
  }

  // построение def-use дуг
  for (Inode* node : dfg->nodes) {
    for (Inode* from : node->useDefRefs) {
      from->def_useRefs.push_back(node);
    }
  }

  return dfg;
}

void dfg::export_dfg(QString _path, DFG* dfg) {
  std::map<Inode*, EdgeNode> ptr2nodeID;
  std::vector<Inode*> locs, exts, acts;

  QString path = fileName(dfg->function_name);
  QFileInfo fileInfo;
  fileInfo.setFile(path);

  QString dir_path = fileInfo.path();
  bool ok = QDir().mkpath(dir_path);

  QFile file(path);
  if (false == file.open(QIODevice::WriteOnly | QIODevice::Text))
     throw std::runtime_error("can't open file");
  QTextStream ofst(&file);

  auto set_node_id = [&] (Inode::Type type, std::vector<Inode*>& vec,
      Inode* ptr) {
    ptr2nodeID.insert(std::make_pair(ptr, EdgeNode(type, vec.size())));
    vec.push_back(ptr);
  };

  acts.push_back(nullptr); // place for argument
  exts.push_back(nullptr); // place for ext0
  for (Inode* ptr : dfg->nodes) {
    if (ptr->type == Inode::Type::ext) {
      if (ptr->value == dfg->function_name) {
        ptr2nodeID.insert(std::make_pair(ptr, EdgeNode(Inode::Type::ext, 0)));
        exts[0] = ptr;
      }
      else {
        set_node_id(Inode::Type::ext, exts, ptr);
      }
    }
    else if (ptr->type == Inode::Type::loc)
      set_node_id(Inode::Type::loc, locs, ptr);
    else if (ptr->type == Inode::Type::act) {
      if (ptr->value == "arg") {
        ptr2nodeID.insert(std::make_pair(ptr, EdgeNode(Inode::Type::act, 0)));
        acts[0] = ptr;
      }
      else
        set_node_id(Inode::Type::act, acts, ptr);
    }
  }

  ofst << "External\n";
  for (Inode* node : exts) {
    ofst << "\t" << ptr2nodeID.find(node)->second.id << "\t"
         << node->value << "\n";
  }

  ofst << "Local\n";
  for (Inode* node : locs) {
    if (node->value == "{---}")
      ofst << "\t" << ptr2nodeID.find(node)->second.id << "\t"
           << "{" << node->delay << "}"
           << ptr2nodeID.find(node->useDefRefs[0])->second.id << "\n";
    else
      ofst << "\t" << ptr2nodeID.find(node)->second.id << "\t"
           << node->value << "\n";
  }

  ofst << "id	delay	operation	links		positions\n";
  for (Inode* node : acts) {
    ofst << ptr2nodeID.find(node)->second.id << "\t" << node->delay << "\t"
         << node->value << "\t\t";
    for (size_t i = 0; i < node->useDefRefs.size(); ++i) {
      Inode* used = node->useDefRefs[i];
      if (used->type == Inode::Type::loc)
        ofst << "loc:" << ptr2nodeID.find(used)->second.id;
      else if (used->type == Inode::Type::ext)
        ofst << "ext:" << ptr2nodeID.find(used)->second.id;
      else if (used->type == Inode::Type::act)
        ofst << ptr2nodeID.find(used)->second.id;
      else if (used->type == Inode::Type::bif)
        ofst << used->value;

      if (i != node->useDefRefs.size() - 1)
        ofst << " ";
    }
    ofst << "\t\tpos 0 0 0 0\n";
    //\\t(.*)\\t\\tpos.*$");
    //0	0	arg				pos 5 30 5 31
  }
}

void read_loc(QString fileString, ImportState &state,
              std::map<EdgeNode, Inode*> &nodeID2ptr,
              std::list<__Edge> &edges, DFG* &dfg) {
  QRegExp rxLocExt("^\\t(\\d+)\\t(.*)$");
  if (fileString == "id	delay	operation	links		positions") {
    state = ImportState::Act;
    return;
  }
  // parse delay const:
  // Str <- ["	", CId, "	{", CDelayNum, "}", CNodeNum], !,
  QRegExp rx_match_delay("^\\t(\\d+)\\t\\{(\\d+)\\}(\\d+)$");
  if (-1 != rx_match_delay.indexIn(fileString, 0)) {
    Inode *node = new Inode(Inode::Type::loc, "{---}",
                                 rx_match_delay.cap(2).toInt());
    node->useDefRefs.push_back(nullptr);
    dfg->nodes.push_back(node);
    nodeID2ptr[EdgeNode(Inode::Type::loc, rx_match_delay.cap(1).toInt())] = node;

    edges.push_back(__Edge(EdgeNode(Inode::Type::act, rx_match_delay.cap(3).toInt()),
                           EdgeNode(Inode::Type::loc, rx_match_delay.cap(1).toInt()),
                           0));
    return;
  }
  // parse simple const:
  if (-1 != rxLocExt.indexIn(fileString, 0)) {
    Inode *node = new Inode(Inode::Type::loc,
                                 rxLocExt.cap(2), 0);
    dfg->nodes.push_back(node);
    nodeID2ptr[EdgeNode(Inode::Type::loc, rxLocExt.cap(1).toInt())] = node;
    return;
  }
}


void read_ext(QString fileString, ImportState &state,
              std::map<EdgeNode, Inode*> &nodeID2ptr,
              std::list<__Edge> &, DFG* &dfg) {
  QRegExp rxLocExt("^\\t(\\d+)\\t(\\S+)$");

  if (fileString == "Local") {
    state = ImportState::Loc;
    return;
  }
  if (-1 == rxLocExt.indexIn(fileString, 0))
    return;

  Inode *node = new Inode(Inode::Type::ext, rxLocExt.cap(2));
  dfg->nodes.push_back(node);
  nodeID2ptr[EdgeNode(Inode::Type::ext, rxLocExt.cap(1).toInt())] = node;
}

void read_act(QString fileString, ImportState &,
              std::map<EdgeNode, Inode*> &nodeID2ptr,
              std::list<__Edge> &edges, DFG* &dfg) {
  //Str <- [CId, "	", CDelayNum, "	", Op, "		", Inps, "		pos", _], !,
  QRegExp rx_match_node("^(\\d+)\\t(\\d+)\\t(\\S+)\\t(.*)\\t\\tpos.*$");
  if (-1 == rx_match_node.indexIn(fileString, 0))
    return;

  Inode *node = new Inode(Inode::Type::act, rx_match_node.cap(3),
                               rx_match_node.cap(2).toInt());
  dfg->nodes.push_back(node);
  nodeID2ptr[EdgeNode(Inode::Type::act, rx_match_node.cap(1).toInt())] = node;

  auto useDefDeps = rx_match_node.cap(4);
  QRegExp rx_match_loc("loc:(\\d+)");
  QRegExp rx_match_ext("ext:(\\d+)");
  QRegExp rx_match_act("(\\d+)");
  QRegExp rx_match_bif("([.]{2}|[.]|!=|<=|>=|[+]|[-]|[*]|\\/|dup"
                       "|int|float|bool"
                       "|<|=|>|[|]|[?]|[#]|\\[\\]|signal)");

  int inpNum = 0;
  int pos = 0;
  while (true) {
    int nextpos = pos;
    if (useDefDeps == "\t" || useDefDeps.size() <= pos)
      break; // no args or end of string

    auto inpIsNotRx = [&](QRegExp &rx) -> bool {
      if ((nextpos = rx.indexIn(useDefDeps, pos)) == -1 || nextpos - pos > 1)
        return true;
      return false;
    };

    auto processInp = [&](Inode::Type type, QRegExp &rx) -> bool{
      if (inpIsNotRx(rx))
        return false;
      pos = nextpos;
      edges.push_back(__Edge(EdgeNode(type, rx.cap(1).toInt()),
                             EdgeNode(Inode::Type::act, rx_match_node.cap(1).toInt()),
                             inpNum));
      node->useDefRefs.push_back(nullptr);
      pos += rx.matchedLength();
      ++inpNum;
      return true;
    };

    if (processInp(Inode::Type::loc, rx_match_loc))
      continue;
    if (processInp(Inode::Type::ext, rx_match_ext))
      continue;
    if (processInp(Inode::Type::act, rx_match_act))
      continue;

    // process bif inputs
    if (inpIsNotRx(rx_match_bif))
      continue;

    pos = nextpos;
    Inode* bifnode = get_bif(rx_match_bif.cap(1));
    node->useDefRefs.push_back(bifnode);
    dfg->nodes.push_back(bifnode);
    pos += rx_match_bif.matchedLength();
    ++inpNum;
  } /*while (true) // цикл по входам*/
}

