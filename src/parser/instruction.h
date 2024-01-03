#ifndef BEANSCRIPT_INSTRUCTION_H
#define BEANSCRIPT_INSTRUCTION_H

#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/utility/utility.h"
#include "src/keyboard/keycodes.h"
#include "src/utility/str_list.h"
#include "src/main.h"
#include "src/utility/uthash.h"

#define DEFAULT_DURATION_LOWER_BOUND 50
#define DEFAULT_DURATION_UPPER_BOUND 70

#define DEFAULT_BEFORE_LOWER_BOUND 0
#define DEFAULT_BEFORE_UPPER_BOUND 0

#define DEFAULT_AFTER_LOWER_BOUND 30
#define DEFAULT_AFTER_UPPER_BOUND 50

#define DEFAULT_REPEAT_LOWER_BOUND 0
#define DEFAULT_REPEAT_UPPER_BOUND 0

#define DEFAULT_COOLDOWN_LOWER_BOUND 0
#define DEFAULT_COOLDOWN_UPPER_BOUND 0

typedef enum {
    KEY,
    PRESS,
    HOLD,
    RELEASE,
    START,
    STOP,
    SCRIPT,
    WINDOW,
    WAITLIST,
    ROUTINE,
    RANDOM,
    GROUP,
    NONE,
} InstructionType;

typedef enum {
    DURATION,
    BEFORE,
    AFTER,
    REPEAT,
    COOLDOWN,
} InstructionParameter;

static const char* InstructionTypeLookupArray[] = {
        "key",
        "press",
        "hold",
        "release",
        "start",
        "stop",
        "script",
        "window",
        "waitlist",
        "routine",
        "random",
        "group",
        "none",
};

static const char* InstructionParameterLookupArray[] = {
        "duration",
        "before",
        "after",
        "repeat",
        "cooldown",
};

static const int InstructionParameterDefaultValues[] = {
        DEFAULT_DURATION_LOWER_BOUND,
        DEFAULT_DURATION_UPPER_BOUND,
        DEFAULT_BEFORE_LOWER_BOUND,
        DEFAULT_BEFORE_UPPER_BOUND,
        DEFAULT_AFTER_LOWER_BOUND,
        DEFAULT_AFTER_UPPER_BOUND,
        DEFAULT_REPEAT_LOWER_BOUND,
        DEFAULT_REPEAT_UPPER_BOUND,
        DEFAULT_COOLDOWN_LOWER_BOUND,
        DEFAULT_COOLDOWN_UPPER_BOUND,
};

static const int NUM_INSTRUCTION_TYPES = sizeof (InstructionTypeLookupArray) / sizeof (InstructionTypeLookupArray[0]);
static const int NUM_INSTRUCTION_PARAMETERS = sizeof (InstructionParameterLookupArray) / sizeof (InstructionParameterLookupArray[0]);

typedef struct InstructionStruct Instruction;

// Collection Functions
void            instruction_map_insert(Instruction* instruction);
void            instruction_map_clear();
Instruction*    instruction_map_get(const char* id);
char*           instruction_map_generate_alias(const char* original_id);
void            instruction_map_print();

// Instruction Helpers
bool            instruction_type_is_definition(InstructionType type);
bool            instruction_can_define_inplace(InstructionType type);
bool            instruction_type_is_transaction(InstructionType type);
bool            instruction_type_is_scheduler(InstructionType type);

// Constructor and Destructor
Instruction*    instruction_new();
void            instruction_delete(Instruction** ptr_instruction);

// Accessor Functions
char*           instruction_get_id(Instruction* instruction);
InstructionType instruction_get_type(Instruction* instruction);
unsigned short  instruction_get_keycode(Instruction* instruction);
int             instruction_get_indent_count(Instruction* instruction);
int             instruction_get_parameter_lower_value(Instruction* instruction, InstructionParameter parameter);
int             instruction_get_parameter_upper_value(Instruction* instruction, InstructionParameter parameter);
char*           instruction_get_sub_instruction_by_index(Instruction* instruction, int index);
int             instruction_get_num_sub_instructions(Instruction* instruction);

// Mutator Functions
void            instruction_set_id(Instruction* instruction, const char* id);
void            instruction_set_type(Instruction* instruction, InstructionType type);
void            instruction_set_indent_count(Instruction* instruction, int indent_count);
void            instruction_set_keycode(Instruction* instruction, unsigned short keycode);
void            instruction_set_parameter_lower_value(Instruction* instruction, InstructionParameter parameter, int lower_value);
void            instruction_set_parameter_upper_value(Instruction* instruction, InstructionParameter parameter, int upper_value);
void            instruction_add_sub_instruction(Instruction* instruction, const char* sub_instruction_id);
void            instruction_copy_values(Instruction* instruction, Instruction* ref_instruction);
void            instruction_set_line_number(Instruction* instruction, int line_number);

// Executors
bool instruction_execute(Instruction* instruction);

// Utility Functions
void            instruction_print(Instruction* instruction, bool should_format);

#endif //BEANSCRIPT_INSTRUCTION_H