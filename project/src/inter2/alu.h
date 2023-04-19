#ifndef ALU_H
#define ALU_H

#include "../rig/rig.h"
#include "../cg/cg.h"
#include "dataWorker.h"

class DataWorker;
class ALU
{
public:
    ALU() {};
    static Value* Interpret(Value* func, Value* arg, DataWorker *caller, int nodeNum, int posIndex);          //интерпретация
};

#endif // ALU_H
