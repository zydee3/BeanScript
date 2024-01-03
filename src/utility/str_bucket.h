#ifndef BEANSCRIPT_STR_BUCKET_H
#define BEANSCRIPT_STR_BUCKET_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../main.h"
#include "str_list.h"

typedef struct StrBucketStruct StrBucket;

// Constructor and Destructor
StrBucket*  str_bucket_new(int bucket_resize_value, int group_resize_value, bool is_using_shared_memory);
void        str_bucket_delete(StrBucket** ptr_buckets);

// Accessor Functions
int        str_bucket_get_size(StrBucket* buckets);
StrList*   str_bucket_get_bucket(StrBucket* buckets, int index);
bool       str_bucket_is_using_shared_memory(StrBucket* buckets);

// Mutator Functions
int         str_bucket_insert_bucket(StrBucket* buckets);
void        str_bucket_insert_str(StrBucket* buckets, int bucket_idx, char* item);
void        str_bucket_clear(StrBucket* buckets);

// Utility Functions
void        str_bucket_print(StrBucket* bucket, bool should_format);

#endif //BEANSCRIPT_STR_BUCKET_H
