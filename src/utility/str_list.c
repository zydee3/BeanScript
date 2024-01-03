/**
 * @file str_list.c
 * @brief A dynamic list of strings. The list is implemented as an array of strings and is dynamically resized. The user
 * has the choice of using shared memory for the value strings. If the user chooses to use shared memory, it is the
 * callers responsibility to free the value strings when the queue is deleted.
 */

#include "str_list.h"

/**
 * @brief A dynamic list of strings.
 * - resize_value: The value at which the list grows or shrinks when strings are added or removed.
 * - is_using_shared_memory: True if the list owns the strings; otherwise, false.
 */
struct StrListStruct {
    char**  strings;
    int     size;
    int     capacity;
    int     resize_value;
    bool    is_using_shared_memory;
};

/**
 * @brief Initializes a new string list.
 * @param resize_value The value at which the list grows or shrinks when strings are added or removed.
 * @param is_using_shared_memory True if the list owns the strings; otherwise, false.
 * @return The newly created string list.
 */
StrList* str_list_new(int resize_value, bool is_using_shared_memory) {
    StrList* list = (StrList*) malloc(sizeof(StrList));
    assert(list != NULL, "Could not allocate memory for string list.");

    list->strings = NULL;
    list->size = 0;
    list->capacity = 0;
    list->resize_value = resize_value;
    list->is_using_shared_memory = is_using_shared_memory;

    return list;
}

/**
 * @brief Creates a deep copy of the string list if is_using_shared_memory is true; otherwise creates a shallow copy.
 */
StrList* str_list_copy(StrList* ref_list) {
    assert(ref_list != NULL, "Attempting to copy NULL string list.");

    StrList* list = str_list_new(ref_list->resize_value, ref_list->is_using_shared_memory);

    for (int i = 0; i < ref_list->size; i++) {
        str_list_insert_str(list, ref_list->strings[i]);
    }

    return list;
}

/**
 * @brief Frees memory for a string list.
 * @param ptr_list The pointer to the string list to delete.
 */
void str_list_delete(StrList** ptr_list) {
    assert(ptr_list != NULL, "Attempting to delete string list behind NULL pointer.");
    assert(*ptr_list != NULL, "Attempting to delete NULL string list.");

    StrList* list = *ptr_list;
    str_list_clear(list);

    free(list);
    *ptr_list = NULL;
}

/**
 * @brief Returns the number of strings in the list.
 */
int str_list_get_size(StrList* list) {
    assert(list != NULL, "Attempting to get size of NULL string list.");

    return list->size;
}

/**
 * @brief Fetches string at a given index.
 */
char* str_list_get_str(StrList* list, int index) {
    assert(list != NULL, "Attempting to get string from NULL string list.");
    assert(index >= 0 && index < list->size, "Attempting to get string from string list with invalid index.");

    return list->strings[index];
}

/**
 * @brief Returns the index of a string, or -1 if the string does not exist.
 */
int str_list_get_index(StrList* list, const char* str) {
    assert(list != NULL, "Attempting to get index from NULL string list.");
    assert(str != NULL, "Attempting to get index of NULL string.");

    for (int str_idx = 0; str_idx < list->size; str_idx++) {
        if (strcmp(list->strings[str_idx], str) == 0) {
            return str_idx;
        }
    }

    return -1;
}

/**
 * @brief Concatenates strings separated by `split_str`. Caller is responsible for freeing the returned string.
 * @return New merged string.
 */
char* str_list_concatenate(StrList* list, const char* split_str) {
    int str_len = 0;
    int split_str_len = (int) strlen(split_str);

    // Calculate the length of the string to be created.
    char* token = NULL;
    int group_size = str_list_get_size(list);

    for (int group_idx = 0; group_idx < group_size; group_idx++) {
        token = str_list_get_str(list, group_idx);
        str_len += (int) strlen(token);

        if (group_idx < group_size - 1) {
            str_len += split_str_len;
        } else {
            str_len += 1;
        }
    }

    // Concatenate each token into the string, separated by the split string parameter.
    // Of course null terminate the string.
    char* str = (char*) malloc(sizeof(char) * str_len);
    int num_chars_written = 0;

    for(int i = 0; i < group_size; i++) {
        token = str_list_get_str(list, i);
        int token_len = (int) strlen(token);

        strcpy(str + num_chars_written, token);
        num_chars_written += token_len;

        if (i < group_size - 1) {
            strcpy(str + num_chars_written, split_str);
            num_chars_written += split_str_len;
        } else {
            strcpy(str + num_chars_written, "\0");
        }
    }

    return str;
}

/**
 * @brief Expands the list by resize_value if it is full or null.
 */
static void expand_list_if_necessary(StrList* list) {
    const bool is_list_null = list->size == 0;
    if (is_list_null) {
        list->capacity = list->resize_value;
        list->strings = (char**) malloc(list->capacity * sizeof(char*));
        assert(list->strings != NULL, "Could not allocate memory for string list strings.");
    }

    const bool is_list_full = list->size >= list->capacity;
    if (is_list_full) {
        list->capacity += list->resize_value;
        list->strings = (char**) realloc(list->strings, sizeof(char*) * list->capacity);
        assert(list->strings != NULL, "Could not allocate memory for string list strings.");
    }

    if (is_list_null || is_list_full) {
        for(int idx_str = list->size; idx_str < list->capacity; idx_str++) {
            list->strings[idx_str] = NULL;
        }
    }
}

/**
 * @brief Inserts a string into the given string list. Deep copies the string if the string list is not using shared
 * memory; otherwise, stores a reference to the string.
 */
void str_list_insert_str(StrList* list, char* str) {
    assert(list != NULL, "Attempting to insert str into NULL string list.");
    assert(str != NULL, "Attempting to insert NULL str into string list.");

    expand_list_if_necessary(list);

    if (list->is_using_shared_memory) {
        list->strings[list->size] = str;
    } else {
        list->strings[list->size] = (char*) malloc(sizeof(char) * (strlen(str) + 1));
        assert(list->strings[list->size] != NULL, "Could not allocate memory for string list str.");
        strcpy(list->strings[list->size], str);
    }

    list->size++;
}

/**
 * @brief Clears the list, freeing memory if it owns the strings.
 */
void str_list_clear(StrList* list) {
    assert(list != NULL, "Attempting to clear NULL string list.");

    if (list->size == 0) {
        assert(list->strings == NULL, "String list size is 0 but string list strings is not NULL.");
        return;
    }

    if (list->is_using_shared_memory) {
        for (int i = 0; i < list->size; i++) {
            list->strings[i] = NULL;
        }
    } else {
        for (int i = 0; i < list->size; i++) {
            free(list->strings[i]);
        }
    }

    free(list->strings);
    list->strings = NULL;
    list->size = 0;
}

/**
 * @brief Prints the string list as a comma-separated string.
 * @param should_format True if 'List (size) [ ..., ...]\n should be printed; otherwise, false for '[..., ...]'
 */
void str_list_print(StrList* list, bool should_format) {
    assert(list != NULL, "Attempting to print NULL string list.");

    if (should_format) {
        printf("List (%d) [", list->size);
    } else {
        printf("[");
    }

    for (int i = 0; i < list->size; i++) {
        printf("%s", list->strings[i]);
        if (i < list->size - 1) {
            printf(", ");
        }
    }

    if (should_format) {
        printf("]\n");
    } else {
        printf("]");
    }
}