#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>

#include "main.h"
#include "runtime.h"
#include "keyboard/keycodes.h"
#include "parser/instruction.h"
#include "utility/str_timestamp_queue.h"

#ifdef __linux__
    #include <unistd.h>
#else
    #include <io.h>
#endif

void assert(bool condition, const char* message, ...) {
#if DISABLE_ASSERTS
    return;
# else
    if(condition == true) {
        return;
    }

    va_list args;
    va_start(args, message);
    vfprintf(stderr, message, args);
    fprintf(stderr, "\n");
    va_end(args);

    exit(EXIT_FAILURE);
#endif
}

int main() {
#if IS_MODULE_TESTING
    srand(time(NULL));

    char alphabet[27] = "abcdefghijklmnopqrstuvwxyz";
    int num_inserts = 10;

    TimestampQueue* queue = str_timestamp_queue_new(num_inserts, false);
    for(int i = 0; i < num_inserts; i++) {
        int random_idx = rand() % 27;
        char* random_char = (char*) malloc(sizeof(char) * 2);
        random_char[0] = alphabet[random_idx];
        random_char[1] = '\0';

        str_timestamp_queue_push(queue, random_idx, random_char);
    }

    for(int i = 0; i < num_inserts; i++) {
        char* item = str_timestamp_queue_pop(queue, 9999);
        printf("%s\n", item);
    }

    str_timestamp_queue_delete(&queue);
#else
        const char *filename = "sample.bs";

        runtime_prepare(filename);
        instruction_map_print();
        runtime_start();

        instruction_map_clear();
        key_map_clear();
        runtime_delete();
#endif

    return 0;
}