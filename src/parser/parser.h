#ifndef BEANSCRIPT_PARSER_H
#define BEANSCRIPT_PARSER_H

#include "instruction.h"
#include "lexer.h"
#include "../main.h"

extern const char* DELIMITERS;

void parse_line_into_instruction(Instruction* instruction, char* str_instruction);

#endif //BEANSCRIPT_PARSER_H
