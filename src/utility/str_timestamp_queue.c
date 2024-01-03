/**
 * @file str_timestamp_queue.c
 *
 * TimestampQueue is a min priority queue that stores a timestamp and an associated string value. When a value is popped
 * from the queue, the timestamp is updated to the current timestamp and the queue is heapified. The user has the choice
 * of using shared memory for the value strings. If the user chooses to use shared memory, it is the callers
 * responsibility to free the value strings when the queue is deleted.
 */

#include <string.h>
#include "str_timestamp_queue.h"

typedef struct {
    time_t timestamp;
    char* value;
} TimestampNode;

typedef struct TimestampQueueStruct {
    int size;
    int capacity;
    bool is_using_shared_memory;
    TimestampNode** nodes;
} TimestampQueue;

/**
 * Creates a new node. If the value is using shared memory, it is the callers responsibility to free the value string.
 *
 * @param timestamp
 * @param value
 * @return
 */
TimestampNode* timestamp_node_new(time_t timestamp, char* value) {
    TimestampNode* timestamp_node = (TimestampNode*) malloc(sizeof(TimestampNode));
    assert(timestamp_node != NULL, "Failed to allocate memory for timestamp_node.");

    timestamp_node->timestamp = timestamp;
    timestamp_node->value = value;

    return timestamp_node;
}

/**
 * Deletes the node. If the value is using shared memory, it is the callers responsibility to free the value string.
 *
 * @param ptr_timestamp_node
 * @param is_using_shared_memory
 */
void timestamp_node_delete(TimestampNode** ptr_timestamp_node, bool is_using_shared_memory) {
    assert(ptr_timestamp_node != NULL, "Attempting to delete NULL ptr_timestamp_node.");
    assert(*ptr_timestamp_node != NULL, "Attempting to delete NULL timestamp_node.");

    TimestampNode* timestamp_node = *ptr_timestamp_node;

    if (is_using_shared_memory == false) {
        free(timestamp_node->value);
        timestamp_node->value = NULL;
    }

    free(timestamp_node);
    *ptr_timestamp_node = NULL;
}

// Constructor and Destructor
/**
 * Creates a new timestamp queue. If is_using_shared_memory is false, it is the callers responsibility to free the value
 * strings when the queue is deleted.
 *
 * @param capacity
 * @param is_using_shared_memory
 * @return
 */
TimestampQueue* str_timestamp_queue_new(int capacity, bool is_using_shared_memory) {
    TimestampQueue* timestamp_queue = (TimestampQueue*) malloc(sizeof(TimestampQueue));
    assert(timestamp_queue != NULL, "Failed to allocate memory for timestamp_queue.");

    timestamp_queue->size = 0;
    timestamp_queue->capacity = capacity;
    timestamp_queue->is_using_shared_memory = is_using_shared_memory;

    timestamp_queue->nodes = (TimestampNode**) malloc(sizeof(TimestampNode*) * capacity);

    return timestamp_queue;
}

/**
 * Deletes the timestamp queue. If the value is using shared memory, it is the callers responsibility to free the value
 * strings.
 *
 * @param ptr_timestamp_queue
 */
void str_timestamp_queue_delete(TimestampQueue** ptr_timestamp_queue) {
    assert(ptr_timestamp_queue != NULL, "Attempting to delete NULL ptr_timestamp_queue.");
    assert(*ptr_timestamp_queue != NULL, "Attempting to delete NULL timestamp_queue.");

    TimestampQueue* timestamp_queue = *ptr_timestamp_queue;
    TimestampNode** nodes = timestamp_queue->nodes;
    const bool is_using_shared_memory = timestamp_queue->is_using_shared_memory;

    for (int i = 0; i < timestamp_queue->size; i++) {
        timestamp_node_delete(&nodes[i], is_using_shared_memory);
        nodes[i] = NULL;
    }

    free(timestamp_queue);
    *ptr_timestamp_queue = NULL;
}

// Helper Function


// Accessor Functions
/**
 * Returns true if the timestamp queue contains the given value, otherwise false.
 *
 * @param timestamp_queue
 * @param value
 * @return
 */
bool str_timestamp_queue_contains(TimestampQueue* timestamp_queue, char* value) {
    assert(timestamp_queue != NULL, "Attempting to check if NULL timestamp_queue contains value.");
    assert(value != NULL, "Attempting to check if timestamp_queue contains NULL value.");

    TimestampNode** nodes = timestamp_queue->nodes;
    assert(nodes != NULL, "Attempting to check if NULL timestamp_queue contains value.");

    const int size = timestamp_queue->size;
    for (int i = 0; i < size; i++) {
        if (strcmp(nodes[i]->value, value) == 0) {
            return true;
        }
    }

    return false;
}

/**
 * Returns the current number of elements in the timestamp queue.
 *
 * @param timestamp_queue
 * @return
 */
int str_timestamp_queue_get_size(TimestampQueue* timestamp_queue) {
    assert(timestamp_queue != NULL, "Attempting to get size of NULL timestamp_queue.");
    return timestamp_queue->size;
}

static void swap(TimestampNode** a, TimestampNode** b) {
    TimestampNode* temp = *a;
    *a = *b;
    *b = temp;
}

static void bubble_down(TimestampNode** nodes, int size, int current_idx) {
    int left_child_idx = current_idx * 2 + 1;
    int right_child_idx = current_idx * 2 + 2;

    if (left_child_idx >= size) {
        return;
    }

    int min_idx = current_idx;

    if (nodes[left_child_idx]->timestamp < nodes[min_idx]->timestamp) {
        min_idx = left_child_idx;
    }

    if (right_child_idx < size && nodes[right_child_idx]->timestamp < nodes[min_idx]->timestamp) {
        min_idx = right_child_idx;
    }

    if (min_idx != current_idx) {
        swap(&nodes[current_idx], &nodes[min_idx]);
        bubble_down(nodes, size, min_idx);
    }
}

static void bubble_up(TimestampNode** nodes, int size, int current_idx) {
    if (current_idx == 0) {
        return;
    }

    int parent_idx = (current_idx - 1) / 2;

    if (nodes[current_idx]->timestamp < nodes[parent_idx]->timestamp) {
        swap(&nodes[current_idx], &nodes[parent_idx]);
        bubble_up(nodes, size, parent_idx);
    }
}

char* str_timestamp_queue_peek_value(TimestampQueue* timestampQueue) {
    assert(timestampQueue != NULL, "Attempting to peek value of NULL timestampQueue.");
    assert(timestampQueue->size > 0, "Attempting to peek value of empty timestampQueue.");

    return timestampQueue->nodes[0]->value;
}

bool str_timestamp_queue_can_pop(TimestampQueue* timestamp_queue) {
    assert(timestamp_queue != NULL, "Attempting to get size of NULL timestamp_queue.");

    if (timestamp_queue->size == 0) {
        return false;
    }

    time_t current_timestamp = get_current_time();
    TimestampNode* min_node = timestamp_queue->nodes[0];

    return current_timestamp <= min_node->timestamp;
}

/**
 * Pops the minimum value from the timestamp queue. The timestamp of the popped element is updated to the parameter
 * updated_timestamp and the queue is heapified.
 * @param timestamp_queue
 * @param updated_timestamp
 * @return
 */
char* str_timestamp_queue_pop(TimestampQueue* timestamp_queue, time_t updated_timestamp) {
    assert(timestamp_queue != NULL, "Attempting to get size of NULL timestamp_queue.");

    TimestampNode** nodes = timestamp_queue->nodes;
    assert(nodes != NULL, "Attempting to pop from NULL timestamp_queue.");

    const int size = timestamp_queue->size;
    assert(size > 0, "Attempting to pop from empty timestamp_queue.");

    TimestampNode* min_node = nodes[0];
    char* min_value = min_node->value;

    // Update timestamp
    min_node->timestamp = updated_timestamp;

    // Bubble down
    bubble_down(nodes, size, 0);

    return min_value;
}

// Mutator Functions
/**
 * Pushes a new value to the timestamp queue. If the value is using shared memory, it is the callers responsibility to
 * free the value string when the queue is deleted.
 *
 * @param timestamp_queue
 * @param timestamp
 * @param value
 */
void str_timestamp_queue_push(TimestampQueue* timestamp_queue, time_t timestamp, char* value) {
    assert(timestamp_queue != NULL, "Attempting to push to NULL timestamp_queue.");

    TimestampNode** nodes = timestamp_queue->nodes;
    assert(nodes != NULL, "Attempting to push to NULL timestamp_queue.");

    const int size = timestamp_queue->size;
    assert(size < timestamp_queue->capacity, "Attempting to push to full timestamp_queue.");

    // Copy value if not using shared memory
    char* new_value = value;
    if (timestamp_queue->is_using_shared_memory == false) {
        new_value = (char*) malloc(sizeof(char) * (strlen(value) + 1));
        strcpy(new_value, value);
    }

    TimestampNode* new_node = timestamp_node_new(timestamp, new_value);
    nodes[size] = new_node;

    // Bubble up
    bubble_up(nodes, size, size);

    // Update size
    timestamp_queue->size = size + 1;
}

// Utility Functions
void str_timestamp_queue_print(TimestampQueue* timestamp_queue, bool should_format) {

}