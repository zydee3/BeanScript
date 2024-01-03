
#ifndef BEANSCRIPT_ROUTINE_H
#define BEANSCRIPT_ROUTINE_H

#include <stdio.h>

#include "src/parser/instruction.h"
#include "src/utility/uthash.h"

typedef struct RoutineStruct Routine;

// Constructor and Destructor
Routine* routine_new(Instruction* instruction, int resize_value);
void routine_delete(Routine** routine);

// Mutator Functions
void routine_insert_instruction(Routine* routine, Instruction* instruction);

// Executors
void routine_execute();

#endif //BEANSCRIPT_ROUTINE_H
