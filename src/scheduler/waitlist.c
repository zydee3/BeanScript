
#include "waitlist.h"

struct WaitlistStruct {
    char* id;
    Instruction* instruction;
    UT_hash_handle hh;

    TimestampQueue* queue;
};

static Waitlist* waitlists = NULL;

/**
 * Inserts a waitlist into the waitlist map. If a waitlist with the same id already exists, then an assertion is thrown.
 * @param waitlist
 */
void waitlist_map_insert(Waitlist* waitlist) {
    assert(waitlist != NULL, "Attempting to insert NULL waitlist into waitlist map.");
    assert(strchr(waitlist->id, '\0') != NULL, "Waitlist id does not contain a null character.");

    Waitlist *current_waitlist = NULL;
    HASH_FIND_STR(waitlists, waitlist->id, current_waitlist);
    assert(current_waitlist == NULL, "Waitlist with id %s already exists.", waitlist->id);

    HASH_ADD_STR(waitlists, id, waitlist);
}

/**
 * Clears the waitlist map and frees all memory associated with it.
 */
void waitlist_map_clear() {
    Waitlist *current_waitlist = NULL;
    Waitlist *tmp = NULL;

    HASH_ITER(hh, waitlists, current_waitlist, tmp) {
        HASH_DEL(waitlists, current_waitlist);
        free(current_waitlist);
    }

    waitlists = NULL;
}

/**
 * Attempts to get a waitlist from the waitlist map. If a waitlist with the given id does not exist, then NULL is returned.
 *
 * @param id
 * @return
 */
Waitlist* waitlist_map_get(const char* id) {
    assert(id != NULL, "Attempting to get waitlist from waitlist map with NULL id.");
    assert(strchr(id, '\0') != NULL, "Waitlist id does not contain a null character.");

    Waitlist *current_waitlist = NULL;
    HASH_FIND_STR(waitlists, id, current_waitlist);

    return current_waitlist;
}





// Constructor and Destructor
/**
 * Creates a new waitlist with the given instruction and resize value.
 * @param instruction
 * @param resize_value
 * @return
 */
Waitlist* waitlist_new(Instruction* instruction, int capacity) {
    assert(instruction != NULL, "Attempting to create waitlist with NULL instruction.");
    assert(capacity > 0, "Attempting to create waitlist with capacity less than or equal to 0.");

    Waitlist* waitlist = (Waitlist*) malloc(sizeof(Waitlist));
    assert(waitlist != NULL, "Failed to allocate memory for waitlist.");

    waitlist->id = instruction_get_id(instruction); // This is a shallow copy because uthash requires an id field.
    waitlist->instruction = instruction;
    waitlist->queue = str_timestamp_queue_new(capacity, false);

    int num_sub_instructions = instruction_get_num_sub_instructions(instruction);
    for(int i = 0; i < num_sub_instructions; i++) {
        char* sub_instruction_id = instruction_get_sub_instruction_by_index(instruction, i);
        str_timestamp_queue_push(waitlist->queue, (time_t) 0, sub_instruction_id);
    }

    return waitlist;
}

void waitlist_delete(Waitlist** waitlist) {
    assert(waitlist != NULL, "Attempting to delete NULL waitlist.");
    assert(*waitlist != NULL, "Attempting to delete NULL *waitlist.");

    Waitlist* ptr_waitlist = *waitlist;

    str_timestamp_queue_delete(&(ptr_waitlist->queue));
    instruction_delete(&(ptr_waitlist->instruction));

    free(ptr_waitlist);
    *waitlist = NULL;
}

// Mutator Functions
void waitlist_insert_instruction(Waitlist* waitlist, Waitlist* instruction) {
    assert(waitlist != NULL, "Attempting to insert instruction into NULL waitlist.");
    assert(instruction != NULL, "Attempting to insert NULL instruction into waitlist.");

    str_timestamp_queue_push(waitlist->queue, (time_t) 0, instruction->id);
}

// Executors


void waitlist_execute() {
    assert(waitlists != NULL, "Attempting to execute waitlist with NULL waitlists.");

    Waitlist *current_waitlist = NULL;
    Waitlist *tmp = NULL;

    // for each waitlist in the waitlist map
    HASH_ITER(hh, waitlists, current_waitlist, tmp) {

        TimestampQueue* queue = current_waitlist->queue;
        while(str_timestamp_queue_can_pop(queue)) {
            char* instruction_id = str_timestamp_queue_peek_value(queue);
            Instruction* instruction = instruction_map_get(instruction_id);
            assert(instruction != NULL, "Instruction with id %s does not exist.", instruction_id);
            time_t cooldown = instruction_get_parameter_lower_value(instruction, COOLDOWN);
            time_t updated_timestamp = (time_t) get_current_time() + cooldown;

            str_timestamp_queue_pop(queue, updated_timestamp);

            instruction_execute(instruction);
        }
    }
}