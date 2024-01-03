/**
 * @file routine.c
 *
 * This file contains the implementation of the routine. A routine is a collection of instructions that are executed
 * sequentially. When an instruction is inserted into a routine, the routine will not execute the instruction until the
 * routine has cycled through of the instructions that were present when the cycle first started. For example, consider
 * the routine A, B, C, and D. If the routine is currently executing B and E, F are inserted, then the routine will
 * execute C and D, return back to A before executing B, C, D, E and F.
 */

#include "routine.h"

struct RoutineStruct {
    char* id;
    Instruction* instruction;
    UT_hash_handle hh;

    StrList* instruction_ids;
    int current_idx;
    int bound_idx;
};

static Routine* routines = NULL;

/**
 * Inserts a routine into the routine map. If a routine with the same id already exists, then an assertion is thrown.
 * @param routine
 */
void routine_map_insert(Routine* routine) {
    assert(routine != NULL, "Attempting to insert NULL routine into routine map.");
    assert(strchr(routine->id, '\0') != NULL, "Routine id does not contain a null character.");

    Routine* current_routine = NULL;
    HASH_FIND_STR(routines, routine->id, current_routine);
    assert(current_routine == NULL, "Routine with id %s already exists.", routine->id);

    HASH_ADD_STR(routines, id, routine);
}

/**
 * Clears the routine map and frees all memory associated with it.
 */
void routine_map_clear() {
    Routine* current_routine = NULL;
    Routine* tmp = NULL;

    HASH_ITER(hh, routines, current_routine, tmp) {
        HASH_DEL(routines, current_routine);
        free(current_routine);
    }

    routines = NULL;
}

/**
 * Attempts to get a routine from the routine map. If a routine with the given id does not exist, then NULL is returned.
 *
 * @param id
 * @return
 */
Routine* routine_map_get(const char* id) {
    assert(id != NULL, "Attempting to get routine from routine map with NULL id.");
    assert(strchr(id, '\0') != NULL, "Routine id does not contain a null character.");

    Routine* current_routine = NULL;
    HASH_FIND_STR(routines, id, current_routine);

    return current_routine;
}


void routine_map_print() {
    Routine* current_routine = NULL;
    Routine* tmp = NULL;

    printf("Routines [");
    HASH_ITER(hh, routines, current_routine, tmp) {
        printf("%s\n", current_routine->id);
    }

    printf("]\n");
}

/**
 * Creates a new routine based on the given instruction. The resize value is used to determine the initial size of the
 * instruction id list. The resize value should be greater than 0.
 *
 * The sub-instructions of the given instruction are deep copied to the instruction id list.
 *
 * @param id
 */
Routine* routine_new(Instruction* instruction, int resize_value) {
    assert(instruction != NULL, "Attempting to create routine with NULL instruction.");
    assert(resize_value > 0, "Attempting to create routine with resize value less than 1.");

    InstructionType instruction_type = instruction_get_type(instruction);
    assert(instruction_type_is_scheduler(instruction_type), "Attempting to create routine with instruction that is not a routine.");

    Routine* routine = malloc(sizeof(Routine));
    routine->id = instruction_get_id(instruction); // This is a shallow copy because uthash requires an id field.
    routine->instruction = instruction;
    routine->instruction_ids = str_list_new(resize_value, false);

    int num_sub_instructions = instruction_get_num_sub_instructions(instruction);
    for(int idx = 0; idx < num_sub_instructions; idx++) {
        char* sub_instruction_id = instruction_get_sub_instruction_by_index(instruction, idx);
        str_list_insert_str(routine->instruction_ids, sub_instruction_id);
    }

    routine->current_idx = 0;
    routine->bound_idx = -1;

    return routine;
}

/**
 * Deletes the given routine and frees all memory associated with it.
 *
 * @param routine
 */
void routine_delete(Routine** routine) {
    assert(routine != NULL, "Attempting to delete NULL routine.");
    assert(*routine != NULL, "Attempting to delete NULL routine.");

    free((*routine)->id);
    (*routine)->id = NULL;

    str_list_delete(&((*routine)->instruction_ids));
    (*routine)->instruction_ids = NULL;

    free(*routine);
    *routine = NULL;
}

/**
 * Inserts the given instruction into the given routine. The instruction is inserted at the end of the routine.
 *
 * @param routine
 * @param instruction
 */
void routine_insert_instruction(Routine* routine, Instruction* instruction) {
    assert(routine != NULL, "Attempting to insert instruction into NULL routine.");
    assert(instruction != NULL, "Attempting to insert NULL instruction into routine.");

    char* instruction_id = instruction_get_id(instruction);
    str_list_insert_str(routine->instruction_ids, instruction_id);

    if (routine->bound_idx == -1) {
        routine->bound_idx = str_list_get_size(routine->instruction_ids);
    }
}

/**
 * Attempts to execute the next available routine instruction. If the routine cannot be executed, then nothing happens.
 * If the routine is executed, then the current index is incremented. If the current index is greater than the size of
 * the routine, then the current index is reset to 0. If the routine is bound to a specific index, then the current
 * index is reset to 0 and the bound index is reset to -1.
 *
 * @param routine
 */
static void routine_execute_(Routine* routine) {
    assert(routine != NULL, "Attempting to iterate NULL routine.");

    char* instruction_id = str_list_get_str(routine->instruction_ids, routine->current_idx);
    Instruction* instruction = instruction_map_get(instruction_id);
    assert(instruction != NULL, "Attempting to get instruction with id %s from instruction map.", instruction_id);

    if(instruction_execute(instruction) == false) {
        return;
    }

    routine->current_idx++;

    if (routine->bound_idx >= 0 && routine->current_idx >= routine->bound_idx) {
        routine->current_idx = 0;
        routine->bound_idx = -1;
    }else if (routine->current_idx >= str_list_get_size(routine->instruction_ids)) {
        routine->current_idx = 0;
    }
}

void routine_execute() {
    Routine* current_routine = NULL;
    Routine* tmp = NULL;

    HASH_ITER(hh, routines, current_routine, tmp) {
        routine_execute_(current_routine);
    }
}


