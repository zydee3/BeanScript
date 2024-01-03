
#ifndef BEANSCRIPT_WAITLIST_H
#define BEANSCRIPT_WAITLIST_H

#include <stdio.h>
#include <sys/time.h>

#include "src/parser/instruction.h"
#include "src/utility/uthash.h"
#include "src/utility/str_timestamp_queue.h"

typedef struct WaitlistStruct Waitlist;

// Constructor and Destructor
Waitlist* waitlist_new(Instruction* instruction, int resize_value);
void waitlist_delete(Waitlist** waitlist);

// Mutator Functions
void waitlist_insert_instruction(Waitlist* waitlist, Waitlist* instruction);

// Executors
void waitlist_execute();

#endif //BEANSCRIPT_WAITLIST_H
