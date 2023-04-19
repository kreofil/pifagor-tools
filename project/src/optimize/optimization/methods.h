#ifndef METHODS_H
#define METHODS_H
#include "../dfg.h"
#include "dfg_functions.h"
#include <vector>

namespace dfg {
  namespace optimization {
    void dead_code(DFG *dfg);
    void equivalent_transformations(DFG *dfg);
    void inline_substitution(DFG *to, DFG *from);
    void repeated_code(DFG *dfg);
    DFG *recursive_invariant(DFG* sub_dfg);
    std::vector<DFG*> parlist_invariant(DFG* dfg, std::vector<DFG*>& all_dfgs);
  }
}

#endif // METHODS_H
