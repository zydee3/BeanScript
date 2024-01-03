#include "instruction.h"

/**
 * @brief A struct representing a single instruction. An instruction can be a single key, a group of keys, a routine, a
 * waitlist, script declaration, window declaration, etc.
 * - id: The id or target of this instruction. This must be unique if it is an id.
 * - indent_count: Leading spaces in the instruction string; used for parsing hierarchy.
 * - keycode: Keycode for single-key press instructions.
 * - parameters:An array of integers where adjacent pairs define the lower and upper bounds of each parameter,
 *   respectively. For example, [lower_bound1, upper_bound1, lower_bound2, upper_bound2, ...].
 * - type: The type of this instruction. See InstructionType for more details.
 * - sub_instructions: List of sub-instructions; relevant for instruction groups.
 */
struct InstructionStruct {
    char* id;
    int indent_count;
    unsigned short keycode;
    int* parameters;
    InstructionType type;
    StrList* sub_instructions;
    int line_number;
    UT_hash_handle hh;
};

/**
 * @brief A map of all instructions. The key is the id of the instruction and the value is the instruction itself.
 */
static Instruction* instructions = NULL;
static const char* instruction_alias_prefix = "Alias_";
static int instruction_alias_counter = 0;

/**
 * @brief Inserts instruction into map; exits if an instruction with the same ID already exists.
 */
void instruction_map_insert(Instruction* instruction) {
    assert(instruction != NULL, "Attempting to insert NULL instruction.");
    assert(strchr(instruction->id, '\0') != NULL, "Instruction id does not contain a null character.");

    Instruction* current_instruction = NULL;
    HASH_FIND_STR(instructions, instruction->id, current_instruction);
    assert(current_instruction == NULL, "Instruction with id %s already exists.", instruction->id);

    HASH_ADD_STR(instructions, id, instruction);
}

/**
 * @brief Clears all instructions from the map.
 */
void instruction_map_clear() {
    Instruction *current_instruction = NULL;
    Instruction *tmp = NULL;

    HASH_ITER(hh, instructions, current_instruction, tmp) {
        HASH_DEL(instructions, current_instruction);
        instruction_delete(&current_instruction);
    }

    instructions = NULL;
}

/**
 * @brief Retrieves instruction by ID from map or NULL if no instruction with the given ID exists.
 */
Instruction* instruction_map_get(const char* id) {
    assert(id != NULL, "Attempting to get instruction with NULL id.");
    assert(strchr(id, '\0') != NULL, "Instruction id does not contain a null character.");

    Instruction* instruction = NULL;
    HASH_FIND_STR(instructions, id, instruction);

    return instruction;
}

/**
 * @brief Generates a unique alias for instruction referencing.
 */
char* instruction_map_generate_alias(const char* original_id) {
//    char* alias = (char*) malloc(sizeof(char) * (strlen(instruction_alias_prefix) + 10));
//    assert(alias != NULL, "Failed to allocate memory for instruction alias.");
//
//    sprintf(alias, "%s%02d", instruction_alias_prefix, instruction_alias_counter++);
//    return alias;

    // Generate a string in the format "Alias(original_id)" using strlen to determine the length of the string.
    const size_t alias_len = strlen(instruction_alias_prefix) + strlen(original_id) + 10;
    char* alias = (char*) malloc(sizeof(char) * alias_len);
    assert(alias != NULL, "Failed to allocate memory for instruction alias.");

    sprintf(alias, "%s%02d(%s)", instruction_alias_prefix, instruction_alias_counter, original_id);
    instruction_alias_counter++;
    return alias;
}

void instruction_map_print() {
    Instruction* current_instruction = NULL;
    Instruction* tmp = NULL;

    HASH_ITER(hh, instructions, current_instruction, tmp) {
        instruction_print(current_instruction, true);
    }
}

/**
 * @brief A definition instruction is an instruction that defines a new object. For example,
 * "key sample with button a".
 */
bool instruction_type_is_definition(InstructionType type) {
    switch(type) {
        case KEY:
        case SCRIPT:
        case WINDOW:
        case WAITLIST:
        case ROUTINE:
        case GROUP:
            return true;
        default:
            return false;
    }
}

/**
 * @brief An instruction can be defined in-place if it is a press, release, or hold instruction. For example,
 * "press button a".
 */
bool instruction_can_define_inplace(InstructionType type) {
    return type == PRESS || type == RELEASE || type == HOLD;
}

/**
 * @brief All parameters of a transaction instruction are saved as sub instructions to be referenced at execution time.
 * For example:
 *      - key sample with button a"
 *      - press sample
 */
bool instruction_type_is_transaction(InstructionType type) {
    switch(type) {
        case PRESS:
        case HOLD:
        case RELEASE:
        case START:
        case STOP:
            return true;
        default:
            return false;
    }
}

/**
 * @brief All parameters of a scheduler instruction are saved as sub instructions to be referenced at execution time.
 * For example: sample_key should be stored as a sub instruction.
 *      - "key sample_key with button a"
 *      - "routine sample_routine with sample_key"
 */
bool instruction_type_is_scheduler(InstructionType type) {
    return type == ROUTINE || type == WAITLIST || type == RANDOM;
}

/**
 * @brief Creates a new instruction with default values.
 * - id: NULL
 * - indent_count: 0
 * - keycode: 0
 * - parameters: See InstructionParameterDefaultValues for more details.
 * - type: NONE
 * - sub_instructions: NULL
 */
Instruction* instruction_new() {
    Instruction* instruction = (Instruction*) malloc(sizeof(Instruction));
    assert(instruction != NULL, "Failed to allocate memory for instruction.");

    instruction->id = NULL;
    instruction->type = NONE;
    instruction->keycode = 0;
    instruction->sub_instructions = NULL;
    instruction->indent_count = 0;

    instruction->parameters = (int*) malloc(2 * NUM_INSTRUCTION_PARAMETERS * sizeof(int));
    assert(instruction->parameters != NULL, "Failed to allocate memory for instruction parameters.");

    memcpy(instruction->parameters, InstructionParameterDefaultValues, 2 * NUM_INSTRUCTION_PARAMETERS * sizeof(int));

    instruction->line_number = -1;

    return instruction;
}

/**
 * @brief Deletes instruction and its sub-instructions if applicable.
 * @param ptr_instruction A pointer to the instruction to delete.
 */
void instruction_delete(Instruction** ptr_instruction) {
    assert(ptr_instruction != NULL, "Attempting to delete instruction behind NULL pointer.");
    assert(*ptr_instruction != NULL, "Attempting to delete NULL instruction.");

    Instruction* instruction = *ptr_instruction;

    if(instruction->id != NULL) {
        free(instruction->id);
        instruction->id = NULL;
    }

    free(instruction->parameters);

    if(instruction->sub_instructions != NULL) {
        str_list_delete(&instruction->sub_instructions);
        instruction->sub_instructions = NULL;
    }

    free(instruction);
    *ptr_instruction = NULL;
}

/**
 * @brief Returns the id.
 */
char* instruction_get_id(Instruction* instruction) {
    assert(instruction != NULL, "Attempting to get id of NULL instruction.");

    return instruction->id;
}

/**
 * @brief Returns the type of the given instruction.
 */
InstructionType instruction_get_type(Instruction* instruction) {
    assert(instruction != NULL, "Attempting to get type of NULL instruction.");

    return instruction->type;
}

/**
 * @brief Returns the keycode.
 */
unsigned short instruction_get_keycode(Instruction* instruction) {
    assert(instruction != NULL, "Attempting to get keycode of NULL instruction.");

    return instruction->keycode;
}

/**
 * @brief Returns the number of spaces at the front.
 * @note A space counts as +1, while a tab counts as +4.
 */
int instruction_get_indent_count(Instruction* instruction) {
    assert(instruction != NULL, "Attempting to get number of prefix spaces of NULL instruction.");

    return instruction->indent_count;
}

/**
 * @brief Gets the lower value of the parameter.
 */
int instruction_get_parameter_lower_value(Instruction* instruction, InstructionParameter parameter) {
    assert(instruction != NULL, "Attempting to get parameter of NULL instruction.");

    return instruction->parameters[2 * parameter];
}

/**
 * @brief Gets the lower value of the parameter.
 */
int instruction_get_parameter_upper_value(Instruction* instruction, InstructionParameter parameter) {
    assert(instruction != NULL, "Attempting to get parameter of NULL instruction.");

    return instruction->parameters[2 * parameter + 1];
}

/**
 * @brief Returns the name of the sub-instruction at the given index.
 */
char* instruction_get_sub_instruction_by_index(Instruction* instruction, int index) {
    assert(instruction != NULL, "Attempting to get sub-instruction of NULL instruction.");
    assert(instruction->type == GROUP, "Attempting to get sub-instruction of instruction that is not a group.");
    assert(index >= 0 && index < str_list_get_size(instruction->sub_instructions),
           "Attempting to get sub-instruction of instruction with invalid index.");

    return str_list_get_str(instruction->sub_instructions, index);
}

/**
 * @brief Retrieves the count of sub-instructions. Errors if the instruction is not a group.
 */
int instruction_get_num_sub_instructions(Instruction* instruction) {
    assert(instruction != NULL, "Attempting to get number of sub-instructions of NULL instruction.");
    assert(instruction->type == GROUP,
           "Attempting to get number of sub-instructions of instruction that is not a group.");

    return str_list_get_size(instruction->sub_instructions);
}

/**
 * @brief Assigns the ID of an instruction. Errors if the ID already exists. A copy of the given ID is made.
 */
void instruction_set_id(Instruction* instruction, const char* id) {
    assert(instruction != NULL, "Attempting to set id of NULL instruction.");
    assert(id != NULL, "Attempting to set id of instruction to NULL.");
    assert(instruction->id == NULL, "Attempting to set id of instruction that already has an id. (current: %s, new: %s)",
           instruction->id, id);

    size_t id_len = strlen(id);
    if (id[id_len - 1] != '\0') {
        id_len++;
    }

    instruction->id = (char*) malloc(sizeof(char) * id_len);
    assert(instruction->id != NULL, "Failed to allocate memory for instruction id.");

    strcpy(instruction->id, id);
    instruction->id[id_len - 1] = '\0';
}

/**
 * @brief Assigns the type of an instruction. Errors if the instruction already has a type.
 */
void instruction_set_type(Instruction* instruction, InstructionType type) {
    assert(instruction != NULL, "Attempting to set type of NULL instruction.");
    assert(instruction->type == NONE, "Attempting to set type of instruction that already has a type.");
    assert(type != NONE, "Attempting to set type of instruction to NONE.");

    instruction->type = type;
}

/**
 * @brief Assigns the number of spaces at the front of the instruction in the instruction string.
 * @note A space counts as +1, while a tab counts as +4.
 */
void instruction_set_indent_count(Instruction* instruction, int indent_count) {
    assert(instruction != NULL, "Attempting to set number of prefix spaces of NULL instruction.");
    assert(indent_count >= 0, "Attempting to set indent count of instruction to negative value.");

    instruction->indent_count = indent_count;
}


/**
 * @brief Assigns the keycode.
 */
void instruction_set_keycode(Instruction* instruction, unsigned short keycode) {
    assert(instruction != NULL, "Attempting to set keycode of NULL instruction.");

    instruction->keycode = keycode;
}

/**
 * @brief Assigns the lower value of the parameter.
 */
void instruction_set_parameter_lower_value(Instruction* instruction, InstructionParameter parameter, int lower_value) {
    assert(instruction != NULL, "Attempting to set the lower value parameter of NULL instruction.");

    instruction->parameters[2 * parameter] = lower_value;
}

/**
 * @brief Assigns the upper value of the parameter.
 */
void instruction_set_parameter_upper_value(Instruction* instruction, InstructionParameter parameter, int lower_value) {
    assert(instruction != NULL, "Attempting to set the upper value parameter of NULL instruction.");

    instruction->parameters[2 * parameter + 1] = lower_value;
}

/**
 * @brief Adds the given sub-instruction. A copy of the given sub-instruction is added.
 */
void instruction_add_sub_instruction(Instruction* instruction, const char* sub_instruction_id) {
    assert(instruction != NULL, "Attempting to add sub-instruction to NULL instruction.");
    assert(sub_instruction_id != NULL, "Attempting to add NULL sub-instruction to instruction.");

    if (instruction->sub_instructions == NULL) {
        const int resize_value = 1;
        const bool is_using_shared_memory = false;
        instruction->sub_instructions = str_list_new(resize_value, is_using_shared_memory);
    }

    str_list_insert_str(instruction->sub_instructions, sub_instruction_id);
}

/**
 * @brief Overwrites @param instruction's keycode and parameters from @param ref_instruction. sub_instructions are
 * overwritten too if @param instruction and @param ref_instruction are both of type group.
 * @param instruction The instruction whose values will be overwritten.
 * @param ref_instruction The instruction whose values will be copied from.
 */
void instruction_copy_values(Instruction* instruction, Instruction* ref_instruction) {
    assert(instruction != NULL, "Attempting to copy values to NULL instruction.");
    assert(ref_instruction != NULL, "Attempting to copy values from NULL instruction.");

    // unsigned short keycode;
    instruction->keycode = ref_instruction->keycode;

    // int* parameters;
    for (int i = 0; i < 2 * NUM_INSTRUCTION_PARAMETERS; i++) {
        instruction->parameters[i] = ref_instruction->parameters[i];
    }

    // InstructionType type;
    if (instruction->type == GROUP && ref_instruction->type == GROUP) {
        if (instruction->sub_instructions != NULL) {
            str_list_delete(&instruction->sub_instructions);
            instruction->sub_instructions = NULL;
        }

        instruction->sub_instructions = str_list_copy(ref_instruction->sub_instructions);
        assert(instruction->sub_instructions != NULL, "Failed to allocate memory for instruction sub_instructions.");
    }
}

/**
 * @brief Sets the line number for an instruction, representing its location in the script file.
 */
void instruction_set_line_number(Instruction* instruction, int line_number) {
    assert(instruction != NULL, "Attempting to set line number of NULL instruction.");
    assert(line_number > 0, "Attempting to set line number of instruction to non-positive value.");

    instruction->line_number = line_number;
}

bool instruction_execute(Instruction* instruction) {
    assert(instruction != NULL, "Attempting to execute NULL instruction.");

    InstructionType instruction_type = instruction_get_type(instruction);
    assert(instruction_type != NONE, "Attempting to execute instruction with type NONE.");

    return false;
}

/**
 * @brief Prints the properties of an instruction.
 */
void instruction_print(Instruction* instruction, bool should_format) {
    assert(instruction != NULL, "Attempting to print NULL instruction.");

    printf("Instruction (line %d) {", instruction->line_number);
    char* key_code_id = key_map_get_id(instruction->keycode);

    if (should_format) {
        printf("\n");
        printf("\ttype: %s\n", InstructionTypeLookupArray[instruction->type]);
        printf("\tid: %s\n", instruction->id);
        printf("\tindent_count: %d\n", instruction->indent_count);
        printf("\tbutton: %s\n", key_code_id);
        for (int i = 0; i < NUM_INSTRUCTION_PARAMETERS; i++) {
            printf("\t%s: Random(min=%d, max=%d)\n", InstructionParameterLookupArray[i],
                   instruction->parameters[2 * i], instruction->parameters[2 * i + 1]);
        }

        printf("\tsub_instructions: ");

        if (instruction->sub_instructions != NULL) {
            str_list_print(instruction->sub_instructions, false);
        } else {
            printf("[]");
        }

        printf("\n}\n");
    } else {
        printf("type: %s, ", InstructionTypeLookupArray[instruction->type]);
        printf("id: %s, ", instruction->id);
        printf("indent_count: %d, ", instruction->indent_count);
        printf("button: %s, ", key_code_id);
        for (int i = 0; i < NUM_INSTRUCTION_PARAMETERS; i++) {
            printf("%s: Random(min=%d, max=%d), ", InstructionParameterLookupArray[i],
                   instruction->parameters[2 * i], instruction->parameters[2 * i + 1]);
        }

        printf("sub_instructions: ");

        if (instruction->sub_instructions != NULL) {
            str_list_print(instruction->sub_instructions, false);
        } else {
            printf("[]");
        }

        printf("}");
    }
}
