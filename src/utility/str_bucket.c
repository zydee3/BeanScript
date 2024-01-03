/**
 * @file str_bucket.c
 *
 * An implementation for a string bucketing data structure. A bucket is a collection of related strings. Naturally, each
 * buckets id is the index of the bucket in the bucket collection. The bucket collection is a dynamic array of buckets
 * that is dynamically resized. The user has the choice of using shared memory for the value strings. If the user
 * chooses to use shared memory, it is the callers responsibility to free the value strings when the queue is deleted.
 *
 * This class uses @class StrList.h to implement each bucket.
 */

#include "str_bucket.h"

/**
 * @brief A struct for representing a list of buckets. A bucket is collection of related strings. Each bucket has an id.
 * - bucket_ids: The list holding the id of each bucket where the ith id is the id of the ith bucket.
 * - buckets: The list of buckets where the ith bucket is the bucket with the id of the ith id in bucket_ids.
 * - size: The number of buckets currently initialized.
 * - capacity: The number of buckets that can be currently held.
 * - resize_value: The amount the bucket capacity is increased or decreased when more or less room is needed.
 * - bucket_resize_value: Similar to resize_value, but for each bucket instead.
 * - is_using_shared_memory: A boolean representing true if the strings stored in the string buckets are referenced from
 * variables or data structures outside of this string bucket. If true, this bucket will not be in charge of freeing the
 * memory of the strings in each bucket and will only free itself; otherwise this bucket will free the memory of the
 * strings in each bucket and itself when deleted. In other words, if true, the bucket does not hold ownership of any
 * of the strings in the buckets.
 */
struct StrBucketStruct {
    StrList** buckets;
    int size;
    int capacity;
    int resize_value;
    int bucket_resize_value;
    bool is_using_shared_memory;
};

/**
 * @brief Creates a new bucket collection with default values.
 * - buckets: NULL
 * - size: 0
 * - capacity: 0
 * - resize_value: given by @param resize_value
 * - bucket_resize_value: given by @param bucket_resize_value
 * - is_using_shared_memory: given by @param is_using_shared_memory
 *
 * @param resize_value The value for how much the bucket collection is increased by when the capacity is reached or how
 * much the bucket array is decreased by when there are (resize_value + 1) empty spaces in the bucket array.
 * @param bucket_resize_value The value for how much the buckets are increased by when the capacity is reached or how
 * much the bucket array is decreased by when there are (resize_value + 1) empty spaces in the bucket array.
 * @param is_using_shared_memory A boolean representing true if the strings stored in the string buckets are referenced
 * from variables or data structures outside of this string bucket. If true, this bucket will not be in charge of
 * freeing the memory of the strings in each bucket and will only free itself; otherwise this bucket will free the
 * memory of the strings in each bucket and itself when deleted.
 * @return A pointer to the newly created bucket collection.
 */
StrBucket* str_bucket_new(int resize_value, int bucket_resize_value, bool is_using_shared_memory) {
    StrBucket* buckets = (StrBucket*) malloc(sizeof(StrBucket));
    assert(buckets != NULL, "Failed to allocate memory for buckets.");

    buckets->buckets = NULL;
    buckets->size = 0;
    buckets->capacity = 0;
    buckets->resize_value = resize_value;
    buckets->bucket_resize_value = bucket_resize_value;
    buckets->is_using_shared_memory = is_using_shared_memory;

    return buckets;
}

void str_bucket_delete(StrBucket** ptr_buckets) {
    assert(ptr_buckets != NULL, "Attempting to delete buckets behind NULL pointer.");
    assert(*ptr_buckets != NULL, "Attempting to delete NULL buckets.");

    StrBucket* buckets = *ptr_buckets;
    str_bucket_clear(buckets);

    free(buckets);
    *ptr_buckets = NULL;
}

int str_bucket_get_size(StrBucket* buckets) {
    assert(buckets != NULL, "Attempting to get size of NULL buckets.");

    return buckets->size;
}

StrList* str_bucket_get_bucket(StrBucket* buckets, int index) {
    assert(buckets != NULL, "Attempting to get bucket from NULL buckets.");
    assert(index >= 0 && index < buckets->size, "Attempting to get bucket from buckets with invalid index.");

    return buckets->buckets[index];
}

bool str_bucket_is_using_shared_memory(StrBucket *buckets) {
    assert(buckets != NULL, "Attempting to get is_using_shared_memory from NULL buckets.");

    return buckets->is_using_shared_memory;
}

/**
 * @brief Adds resize_value number of buckets to the bucket collection if is full or null.
 */
static void add_buckets_if_necessary(StrBucket* buckets) {
    const bool is_bucket_null = buckets->size == 0;
    if (is_bucket_null) {
        buckets->capacity = buckets->resize_value;
        buckets->buckets = (StrList**) malloc(sizeof(StrList*) * buckets->capacity);
        assert(buckets->buckets != NULL, "Failed to allocate memory for buckets->bucket_ids.");
    }

    const bool is_bucket_full = buckets->size >= buckets->capacity;
    if (is_bucket_full) {
        buckets->capacity += buckets->resize_value;

        buckets->buckets = (StrList**) realloc(buckets->buckets, sizeof(StrList*) * buckets->capacity);
        assert(buckets->buckets != NULL, "Failed to allocate memory for buckets->buckets.");

        for (int idx_bucket = buckets->size; idx_bucket < buckets->capacity; idx_bucket++) {
            buckets->buckets[idx_bucket] = NULL;
        }
    }
}

int str_bucket_insert_bucket(StrBucket* buckets) {
    assert(buckets != NULL, "Attempting to get bucket from NULL buckets.");

    add_buckets_if_necessary(buckets);

    StrList* new_bucket = str_list_new(buckets->bucket_resize_value, buckets->is_using_shared_memory);
    buckets->buckets[buckets->size] = new_bucket;
    buckets->size++;

    return buckets->size - 1;
}

void str_bucket_insert_str(StrBucket* buckets, int bucket_idx, char *item) {
    assert(buckets != NULL, "Attempting to insert item into NULL buckets.");
    assert(bucket_idx >= 0 && bucket_idx < buckets->size, "Attempting to insert item into buckets with invalid index.");
    assert(item != NULL, "Attempting to insert NULL item.");

    StrList* bucket = str_bucket_get_bucket(buckets, bucket_idx);
    assert(bucket != NULL, "Attempting to insert item into NULL bucket.");

    str_list_insert_str(bucket, item);
}

void str_bucket_clear(StrBucket* buckets) {
    assert(buckets != NULL, "Attempting to clear NULL buckets.");

    if (buckets->buckets != NULL) {
        for (int i = 0; i < buckets->size; i++) {
            str_list_delete(&buckets->buckets[i]);
        }

        free(buckets->buckets);
        buckets->buckets = NULL;
    }

    buckets->size = 0;
}

/**
 * @brief Prints the given bucket collection.
 * @param bucket The bucket collection to print.
 * @param should_format True if 'Bucket (size) {\n ...\n ...\n }\n' should be printed; otherwise false for '{ ..., ... }'
 */
void str_bucket_print(StrBucket* bucket, bool should_format) {
    assert(bucket != NULL, "Attempting to print NULL bucket.");

    printf("Bucket (%d) {\n", bucket->size);
    for (int i = 0; i < bucket->size; i++) {
        printf("\tBucket %02d: ", i);
        str_list_print(bucket->buckets[i], false);
        printf("\n");
    }

    printf("}\n");
}