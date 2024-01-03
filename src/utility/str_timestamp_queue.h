
#ifndef BEANSCRIPT_STR_TIMESTAMP_QUEUE_H
#define BEANSCRIPT_STR_TIMESTAMP_QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../utility/utility.h"

#include "../main.h"

typedef struct TimestampQueueStruct TimestampQueue;

// Constructor and Destructor
TimestampQueue* str_timestamp_queue_new(int capacity, bool is_using_shared_memory);
void str_timestamp_queue_delete(TimestampQueue** ptr_timestamp_queue);

// Accessor Functions
bool str_timestamp_queue_contains(TimestampQueue* timestamp_queue, char* value);
int str_timestamp_queue_get_size(TimestampQueue* timestamp_queue);
char* str_timestamp_queue_peek_value(TimestampQueue* timestampQueue);
bool str_timestamp_queue_can_pop(TimestampQueue* timestamp_queue);
char* str_timestamp_queue_pop(TimestampQueue* timestamp_queue, time_t updated_timestamp);

// Mutator Functions
void str_timestamp_queue_push(TimestampQueue* timestamp_queue, time_t timestamp, char* value);

// Utility Functions
void str_timestamp_queue_print(TimestampQueue* timestamp_queue, bool should_format);

#endif //BEANSCRIPT_STR_TIMESTAMP_QUEUE_H
