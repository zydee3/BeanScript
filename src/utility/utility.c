/**
 * @file utility.c
 *
 * A simple utility module for common string, integer, and primitive array operations.
 */

#include "utility.h"

/**
 * Returns the index of the target int in the array, or -1 if the target int is not found.
 * @param array
 * @param size
 * @param target
 * @return
 */
int str_array_find(const char** array, int size, const char* target) {
    assert(array != NULL, "Attempting to find int in NULL array.");
    assert(size > 0, "Attempting to find int in empty array.");

    for (int i = 0; i < size; i++) {
        if (strcmp(array[i], target) == 0) {
            return i;
        }
    }

    return -1;
}

/**
 * Removes all delimiters defined by the parameter from the end of the string. Returns the number of delimiters removed.
 * @param S
 * @param delimiters
 * @return
 */
int str_remove_trailing_delimiters(char *S, const char *delimiters) {
    int removed_count = 0;
    int len = strlen(S);

    while (len > 0 && strchr(delimiters, S[len - 1]) != NULL) {
        S[len - 1] = '\0';
        len--;
        removed_count++;
    }

    return removed_count;
}

/**
 *  Removes all delimiters defined by the parameter from the beginning of the string. Returns the number of delimiters
 *  removed.
 * @param S
 * @param delimiters
 * @return
 */
int str_remove_leading_ignored_chars(char *S, const char *delimiters) {
    int removed_count = 0;
    char *current_token = S;

    while(*current_token && strchr(delimiters, *current_token)) {
        *current_token = '\0';
        current_token++;
        removed_count++;
    }

    return removed_count;
}

/**
 * Returns the element at the specified index in the array, or the default value if the index is out of bounds.
 * @param int_array
 * @param int_array_len
 * @param idx
 * @param default_value
 * @return
 */
int int_array_get_or_default(int* int_array, int int_array_len, int idx, int default_value) {
    if (idx < int_array_len) {
        return int_array[idx];
    }

    return default_value;
}

/**
 * Returns the current time in ms since epoch.
 *
 * @return
 */
time_t get_current_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    // Convert timeval to milliseconds and store in time_t
    time_t milliseconds = (time_t)(tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;

    return milliseconds;
}
