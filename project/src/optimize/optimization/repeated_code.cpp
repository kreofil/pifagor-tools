#include "methods.h"
#include "dfg_functions.h"
#include <QDebug>

using namespace dfg;
using namespace dfg::optimization;

void repeated_actors(DFG *dfg);

typedef std::multimap<int, Inode*>::iterator mapIter;

void dfg::optimization::repeated_code(DFG *dfg) {
  repeated_actors(dfg);
}

std::vector<std::vector<int> > calc_delay_structure(DFG *dfg) {
  std::vector<std::vector<int> > delay_structure;
  std::list<Inode*> delays = find_nodes(dfg, Inode::Type::loc, "{---}");
  delay_structure.resize(delays.size() + 1);

  for (Inode* loc : delays) {
    delay_structure[(*loc->def_useRefs.begin())->delay].push_back(loc->delay);
  }
  return delay_structure;
}

void plain_delay_structure(std::vector<std::vector<int> > &delay_structure,
                           std::vector<std::vector<int> > &plain_structure,
                           int number) {
  std::vector<int> delays = delay_structure[number];
  std::vector<int> &to = plain_structure[number];

  vector_append(to, delays);
  for (int delay : delays) {
    plain_delay_structure(delay_structure, plain_structure, delay);
    vector_append(to, plain_structure[delay]);
  }
}


QString get_node_name(Inode* node) {
  return type2string(node->type) + "," + node->value;
}

QString node_dependences2string(Inode* node) {
  QString result;
  for (Inode* dependence : node->useDefRefs) {
    result += QString::fromLatin1("@") + QString("0x%1").arg(
          (quintptr)dependence, QT_POINTER_SIZE * 2, 16, QChar('0'));
  }
  return result;
}

QString node2string(Inode* node) {
  return get_node_name(node) + node_dependences2string(node);
}

void repeated_actors(DFG *dfg) {
  // --- строим ЯПФ ---
  std::vector<std::list<Inode*> > multilevel_dfg = get_multilevel_dfg(dfg);

  // --- строим струткру вложенности задержанных списков ---
  std::vector<std::vector<int> >delay_structure = calc_delay_structure(dfg);
  std::vector<std::vector<int> >plain_delays;
  plain_delays.resize(delay_structure.size());
  plain_delay_structure(delay_structure, plain_delays, 0);

  // --- удаляем повторы на каждом слое ЯПФ ---
  //qDebug() << node2string(ret);
  for (size_t i = 0; i < multilevel_dfg.size(); ++i) {
    // 1) структура для сбора кодирования узлов уровня ЯПФ
    std::map<QString, std::multimap<int, Inode*> > coded_nodes;

    // 2) обходим уровень, кодируем узлы
    std::list<Inode*> &level = multilevel_dfg[i];
    for (Inode* node : level)
      coded_nodes[node2string(node)].insert(std::make_pair(node->delay, node));

    // 3) обходим кодированные узлы и удаляем дубли
    for (auto coded_node : coded_nodes) {
      std::multimap<int, Inode*> repeated_map = coded_node.second;

      // 3.1) обходим структуру вложенных списков
      for (size_t delay = 0; delay < plain_delays.size(); ++delay) {

        // проверяем что в списке delay есть узел
        std::pair<mapIter, mapIter> keyRangeDelay = repeated_map.equal_range(delay);
        if (keyRangeDelay.first == keyRangeDelay.second)
          continue;
        Inode* uniq = (*keyRangeDelay.first).second; // узел списка delay

        // 3.1.2 для списка с номером delay оставляю только uniq
        // (остальные повторы удаляю)
        auto it = keyRangeDelay.first;
        ++it;
        while(it != keyRangeDelay.second) {
          remove_node(dfg, (*it).second, uniq);
          it = repeated_map.erase(it);
        }

        // 3.1.3) обходим список inner_delay (списков вложенных в delay)
        // все элементы можно удалить (заменить на uniq)
        std::vector<int>& inner_delays = plain_delays[delay];
        for (int inner_delay : inner_delays) {
          std::pair<mapIter, mapIter> keyRangeRepeated = repeated_map.equal_range(inner_delay);
          // keyRange - задает диапазон удаляемых элементов

          for (auto it = keyRangeRepeated.first; it != keyRangeRepeated.second;) {
            remove_node(dfg, (*it).second, uniq);
            it = repeated_map.erase(it);
          }
        }
      }
    }
  }
}
