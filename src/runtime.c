/**
 * @file runtime.c
 *
 * This file contains the runtime for the script. The runtime is responsible for executing the script.
 */

#include "runtime.h"

static StrList* execution_list = NULL;

static FILE* open_file(const char* filename) {
    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    return file;
}

/**
 * Attempts to execute a sub-instruction. A sub-instruction is an instruction that is indented and is therefore ran
 * as a result of another instruction.
 *
 * @param instruction
 * @param all_instructions
 * @return
 */
static bool try_handle_sub_instruction(Instruction* instruction, StrList* all_instructions) {
    const bool is_sub_instruction = instruction_get_indent_count(instruction) > 0;
    if (is_sub_instruction == false) {
        return false;
    }

    const int num_total_instructions = str_list_get_size(all_instructions);
    assert(num_total_instructions > 0, "Attempting to add sub instruction to empty execution list.");

    const int current_indent_count = instruction_get_indent_count(instruction);

    for(int idx = num_total_instructions - 1; idx >= 0; idx--) {
        const char* str_ref_instruction_id = str_list_get_str(all_instructions, idx);
        Instruction* ref_instruction = instruction_map_get(str_ref_instruction_id);
        assert(ref_instruction != NULL, "Attempting to reference instruction for sub-instruction.");

        const int ref_indent_count = instruction_get_indent_count(ref_instruction);

        if (ref_indent_count < current_indent_count) {
            const char* current_instruction_id = instruction_get_id(instruction);
            instruction_add_sub_instruction(ref_instruction, current_instruction_id);
            return true;
        }
    }

    return false;
}

/**
 * Compiles the script into a list of instructions that are ready to be executed.
 *
 * @param str_script_name
 */
void runtime_prepare(const char* str_script_name) {
    execution_list = str_list_new(1, true);

    const char *filename = str_script_name;
    FILE* file = open_file(filename);

    const int max_line_length = 256;
    char* line = (char*) malloc(max_line_length);

    int current_line = 0;

    // Keep a record of all parsed instructions. This assists with sub-instruction nesting. Once the instructions are
    // parsed, this list is no longer needed.
    StrList* all_instructions = str_list_new(1, true);


    while (fgets(line, max_line_length, file) != NULL) {
        Instruction* instruction = instruction_new();
        instruction_set_line_number(instruction, ++current_line);

        // Call the parser, which calls the lexer, to parse the line into an instruction.
        parse_line_into_instruction(instruction, line);

        // If the instruction is empty, then do nothing. This should never happen because if there was an error parsing
        // the line, then this compiler would have exited.
        InstructionType type = instruction_get_type(instruction);
        if (type == NONE) {
            instruction_delete(&instruction);
            continue;
        }

        // Insert the instruction into the instruction map. This houses all parsed instructions regardless of execution
        // order and thus each instruction must have a unique name. Naming is in the parsing stage.
        instruction_map_insert(instruction);

        // Keep a record of all parsed instructions. This assists with sub-instruction nesting.
        const char* current_instruction_id = instruction_get_id(instruction);
        str_list_insert_str(all_instructions, current_instruction_id);

        // If the instruction is a sub-instruction, then add it to the parent instruction and continue to the next
        // instruction. Sub-instructions are not added to the execution list because they are ran as a result of the
        // parent instruction.
        const bool is_sub_instruction = try_handle_sub_instruction(instruction, all_instructions);
        if (is_sub_instruction == true) {
            continue;
        }

        // If the instruction is a transaction, then add it to the execution list.
        const bool is_transaction = instruction_type_is_transaction(type);
        if (is_transaction == true) {
            const char* str_instruction_id = instruction_get_id(instruction);
            str_list_insert_str(execution_list, str_instruction_id);
        }
    }

    str_list_delete(&all_instructions);

    free(line);
    fclose(file);
}

void runtime_start() {
    str_list_print(execution_list, true);
    for (int i = 0; i < str_list_get_size(execution_list); i++) {
        const char* str_instruction_id = str_list_get_str(execution_list, i);
        Instruction* instruction = instruction_map_get(str_instruction_id);
        assert(instruction != NULL, "Attempting to get instruction from instruction map.");

        instruction_print(instruction, true);
    }
}

void runtime_delete() {
    str_list_delete(&execution_list);
}

void runtime_print() {
    str_list_print(execution_list, false);
}