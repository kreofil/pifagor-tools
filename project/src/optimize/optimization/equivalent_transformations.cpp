#include "methods.h"
#include "dfg_functions.h"

using namespace dfg;
using namespace dfg::optimization;

void single_element_parlists(DFG *dfg);
void parlists_in_datalists(DFG *dfg);
void parlists_interp(DFG *dfg);

void dfg::optimization::equivalent_transformations(DFG *dfg) {
  single_element_parlists(dfg);
  parlists_in_datalists(dfg);
  parlists_interp(dfg);
}

void single_element_parlists(DFG *dfg) {
  auto parlists = find_nodes(dfg, Inode::Type::act, "[---]");

  for (auto it = parlists.begin(); it != parlists.end(); ++it) {
    Inode* parlist = *it;

    if (parlist->useDefRefs.size() != 1)
      continue;

    auto parlistDefUse = parlist->def_useRefs;

    if (parlistDefUse.size() == 1 && (*parlistDefUse.begin())->type == Inode::Type::loc)
      continue;
    // else single element parlist
    Inode* single_element = *parlist->useDefRefs.begin();

    for (Inode* node : parlistDefUse) {
      single_element->def_useRefs.push_back(node);

      auto node_parlist_edge = std::find(node->useDefRefs.begin(),
                                            node->useDefRefs.end(),
                                            parlist);

      *node_parlist_edge = single_element;
    }

    dfg->nodes.remove(*it);
    delete *it;
    ++it;
  }
}

void parlists_in_datalists(DFG *dfg) {
  auto actors = find_nodes(dfg, Inode::Type::act);

  for (auto itDatalist = actors.begin(); itDatalist != actors.end();) {
    Inode* datalist = *itDatalist;

    if (datalist->value != "(---)") {
      ++itDatalist;
      continue;
    }

    bool is_changed = false;
    for (auto it = datalist->useDefRefs.begin();
         it != datalist->useDefRefs.end(); ++it) {
      if ((*it)->type != Inode::Type::act || (*it)->value != "[---]")
        continue;

      Inode *parlist = *it;

      // it - это также позиция, в которую надо поместить новые дуги

      it = datalist->useDefRefs.erase(it); // удаляю дугу от параллельного списка
      for(auto from : parlist->useDefRefs) {
        it = datalist->useDefRefs.insert(it, from);
        ++it;
        from->def_useRefs.push_back(datalist);
      }

      std::remove(parlist->def_useRefs.begin(),
                  parlist->def_useRefs.end(), datalist);
      is_changed = true;
      break;
    }

    if (is_changed == false)
      itDatalist++;
  }
}

void parlists_interp(DFG *dfg) {
  auto interps = find_nodes(dfg, Inode::Type::act, ":");

  for (Inode* interp : interps) {
    std::vector<Inode*> data_nodes, func_nodes;

    auto get_args = [&](int num, std::vector<Inode*> &nodes) {
      if (interp->useDefRefs[num]->type == Inode::Type::act &&
          interp->useDefRefs[num]->value == "[---]")
        nodes = interp->useDefRefs[num]->useDefRefs;
      else
        nodes.push_back(interp->useDefRefs[num]);
    };

    get_args(0, data_nodes);
    get_args(1, func_nodes);

    if (data_nodes.size() <= 1 && func_nodes.size() <= 1)
      continue;

    // удаляем входящие зависимости узла интерпретации:
    interp->useDefRefs[0]->def_useRefs.remove(interp);
    interp->useDefRefs[1]->def_useRefs.remove(interp);

    interp->useDefRefs.clear();

    Inode *results_parlist = interp;
    // узел интерпретации заменяем на узел сбора результатов в [---]:
    results_parlist->value = "[---]";

    const int delay_number = interp->delay;

    for (size_t i = 0; i < func_nodes.size(); ++i) {
      for (size_t j = 0; j < data_nodes.size(); ++j) {
        Inode *node = new Inode(Inode::Type::act, ":", delay_number);
        dfg->nodes.push_back(node);

        node->useDefRefs.push_back(data_nodes[j]);
        node->useDefRefs.push_back(func_nodes[i]);

        func_nodes[i]->def_useRefs.push_back(node);
        data_nodes[j]->def_useRefs.push_back(node);

        node->def_useRefs.push_back(results_parlist);
        results_parlist->useDefRefs.push_back(node);
      }
    }
  }
}
