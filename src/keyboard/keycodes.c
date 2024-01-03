
#include "keycodes.h"

static Key* keys = NULL;

static void key_map_insert(const char* id, unsigned short code) {
    Key* current_key = NULL;

    HASH_FIND_STR(keys, id, current_key);
    assert(current_key == NULL, "Attempting to insert duplicate key into keycodes.");

    current_key = (Key*) malloc(sizeof(Key));
    assert(current_key != NULL, "Could not allocate memory for key.");
    current_key->id = id;
    current_key->code = code;

    HASH_ADD_STR(keys, id, current_key);
}

void key_map_create() {
    assert(keys == NULL, "Attempting to create key map when one already exists.");
    key_map_insert("none", 0x00);
    key_map_insert("escape", 0x01);
    key_map_insert("f1", 0x3B);
    key_map_insert("f2", 0x3C);
    key_map_insert("f3", 0x3D);
    key_map_insert("f4", 0x3E);
    key_map_insert("f5", 0x3F);
    key_map_insert("f6", 0x40);
    key_map_insert("f7", 0x41);
    key_map_insert("f8", 0x42);
    key_map_insert("f9", 0x43);
    key_map_insert("f10", 0x44);
    key_map_insert("f11", 0x57);
    key_map_insert("f12", 0x58);
    key_map_insert("printscreen", 0xB7);
    key_map_insert("scrolllock", 0x46);
    key_map_insert("pause", 0xC5);
    key_map_insert("`", 0x29);
    key_map_insert("1", 0x02);
    key_map_insert("2", 0x03);
    key_map_insert("3", 0x04);
    key_map_insert("4", 0x05);
    key_map_insert("5", 0x06);
    key_map_insert("6", 0x07);
    key_map_insert("7", 0x08);
    key_map_insert("8", 0x09);
    key_map_insert("9", 0x0A);
    key_map_insert("0", 0x0B);
    key_map_insert("-", 0x0C);
    key_map_insert("=", 0x0D);
    key_map_insert("backspace", 0x0E);
    key_map_insert("insert", 0xD2 + 1024);
    key_map_insert("home", 0xC7 + 1024);
    key_map_insert("pageup", 0xC9 + 1024);
    key_map_insert("pagedown", 0xD1 + 1024);
    key_map_insert("numberlock", 0x45);
    key_map_insert("divide", 0xB5 + 1024);
    key_map_insert("multiply", 0x37);
    key_map_insert("subtract", 0x4A);
    key_map_insert("add", 0x4E);
    key_map_insert("decimal", 0x53);
    key_map_insert("tab", 0x0F);
    key_map_insert("q", 0x10);
    key_map_insert("w", 0x11);
    key_map_insert("e", 0x12);
    key_map_insert("r", 0x13);
    key_map_insert("t", 0x14);
    key_map_insert("y", 0x2C);
    key_map_insert("u", 0x16);
    key_map_insert("i", 0x17);
    key_map_insert("o", 0x18);
    key_map_insert("p", 0x19);
    key_map_insert("[", 0x1A);
    key_map_insert("]", 0x1B);
    key_map_insert("\\", 0x2B);
    key_map_insert("delete", 0xD3 + 1024);
    key_map_insert("end", 0xCF + 1024);
    key_map_insert("capslock", 0x3A);
    key_map_insert("a", 0x1E);
    key_map_insert("s", 0x1F);
    key_map_insert("d", 0x20);
    key_map_insert("f", 0x21);
    key_map_insert("g", 0x22);
    key_map_insert("h", 0x23);
    key_map_insert("j", 0x24);
    key_map_insert("k", 0x25);
    key_map_insert("l", 0x26);
    key_map_insert(";", 0x27);
    key_map_insert("'", 0x28);
    key_map_insert("enter", 0x1C);
    key_map_insert("return", 0x1C);
    key_map_insert("shift", 0x2A);
    key_map_insert("z", 0x15);
    key_map_insert("x", 0x2D);
    key_map_insert("c", 0x2E);
    key_map_insert("v", 0x2F);
    key_map_insert("b", 0x30);
    key_map_insert("n", 0x31);
    key_map_insert("m", 0x32);
    key_map_insert(",", 0x33);
    key_map_insert(".", 0x34);
    key_map_insert("/", 0x35);
    key_map_insert("shiftright", 0x36);
    key_map_insert("ctrl", 0x1D);
    key_map_insert("window", 0xDB + 1024);
    key_map_insert("alt", 0x38);
    key_map_insert("space", 0x39);
    key_map_insert("altright", 0xB8 + 1024);
    key_map_insert("windowright", 0xDC + 1024);
    key_map_insert("apps", 0xDD + 1024);
    key_map_insert("ctrlright", 0x9D + 1024);

#ifdef __linux__
    // These actually are incorrect, however are placeholder since
    // we use WSL  for debugging / memory checking.
    key_map_insert("up", 0x9D + 1025);
    key_map_insert("left", 0x9D + 1025);
    key_map_insert("down", 0x9D + 1025);
    key_map_insert("right", 0x9D + 1025);
#else
    key_map_insert("up", MapVirtualKeyW(0x26, 0));
    key_map_insert("left", MapVirtualKeyW(0x25, 0));
    key_map_insert("down", MapVirtualKeyW(0x28, 0));
    key_map_insert("right", MapVirtualKeyW(0x27, 0));
#endif
}

void key_map_clear() {
    Key* current_key, *tmp;

    HASH_ITER(hh, keys, current_key, tmp) {
        HASH_DEL(keys, current_key);
        free(current_key);
    }

    keys = NULL;
}

Key* key_map_get(const char* id) {
    // assert(keys != NULL, "Attempting to get key from key map when one does not exist.");
    if (keys == NULL) {
        key_map_create();
    }

    Key* current_key = NULL;
    HASH_FIND_STR(keys, id, current_key);

    return current_key;
}

const char* key_map_get_id(unsigned short code) {
    // assert(keys != NULL, "Attempting to get key from key map when one does not exist.");
    if (keys == NULL) {
        key_map_create();
    }

    Key* current_key = NULL, *tmp;
    HASH_ITER(hh, keys, current_key, tmp) {
        if (current_key->code == code) {
            return current_key->id;
        }
    }

    return NULL;
}

