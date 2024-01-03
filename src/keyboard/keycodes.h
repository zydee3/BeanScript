#ifndef BEANSCRIPT_KEYCODES_H
#define BEANSCRIPT_KEYCODES_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <windows.h>
#elif defined(__linux__)
    #include <unistd.h>
#else
    #error "Unsupported platform"
#endif

#include "src/main.h"
#include "src/utility/uthash.h"

typedef struct {
    const char *id;
    unsigned short code;
    UT_hash_handle hh;
} Key;


void key_map_create();
void key_map_clear();
Key* key_map_get(const char* id);
const char* key_map_get_id(unsigned short code);

#endif //BEANSCRIPT_KEYCODES_H