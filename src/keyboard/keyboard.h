#ifndef BEANSCRIPT_KEYBOARD_H
#define BEANSCRIPT_KEYBOARD_H

#ifdef _WIN32

#include <errno.h>
#include <stdbool.h>

#include "interception.h"
#include "./keycodes.h"

typedef struct {
    InterceptionDevice id;
    InterceptionContext context;
    InterceptionKeyStroke stroke;
} Keyboard;

Keyboard* keyboard_new();
void keyboard_delete(Keyboard** ptr_keyboard);
bool keyboard_send(Keyboard* keyboard, const char* key_id);

#endif

#endif //BEANSCRIPT_KEYBOARD_H