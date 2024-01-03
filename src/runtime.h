
#ifndef BEANSCRIPT_RUNTIME_H
#define BEANSCRIPT_RUNTIME_H

#include <stdio.h>
#include <stdlib.h>

#include "parser/instruction.h"
#include "parser/parser.h"
#include "utility/str_list.h"

void runtime_prepare(const char* str_script_name);
void runtime_delete();
void runtime_start();
void runtime_print();


#endif //BEANSCRIPT_RUNTIME_H
