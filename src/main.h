#ifndef BEANSCRIPT_MAIN_H
#define BEANSCRIPT_MAIN_H

#define DISABLE_ASSERTS false
#define IS_MODULE_TESTING false

#include <stdbool.h>

void assert(bool condition, const char* message, ...);

#endif //BEANSCRIPT_MAIN_H
