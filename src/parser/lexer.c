/**
 * @class Lexer.c
 *
 * The lexer is responsible for tokenizing a string and grouping tokens into buckets. Each bucket contains a group. A
 * group is a list of strings following the format
 * <instruction type> <instruction id> with <parameter 1>, <parameter 2>, ..., <parameter n>, where "with" is a
 * keyword that separates the instruction header from the parameters. Each < > is a group.
 *
 * Example:
 *  - format: <instruction type> <instruction id> with <parameter 1>, <parameter 2>, ..., <parameter n>
 *  - Input: key k6 with button g, k5, duration 1, after 1, repeat 1, D2w D23eF@#fe3F 32qeF e3 F_@$%%&*(!@),,,,,,,,,
 *  - Output:
 *      Bucket (8) {
 *          "Bucket_OP": [key]
 *          "Bucket_ID": [k6]
 *          "Param_00":  [button, g]
 *          "Param_01":  [k5]
 *          "Param_02":  [duration, 1]
 *          "Param_03":  [after, 1]
 *          "Param_04":  [repeat, 1]
 *          "Param_05":  [D2w D23eF@#fe3F 32qeF e3 F_@$%%&*(!@)]
 *      }
 */

#include "lexer.h"
#include "instruction.h"

const int BUCKET_TYPE_ID = 0;

/**
 * Inserts the token into the bucket.
 *
 * True is returned if :
 * - The token was an instruction type or id.
 * - the token was "with"
 * - the token ended with a comma, indicating the end of a parameter group.
 *
 * See @class Lexer.c file summary for a more detailed explanation.
 * @param bucket
 * @param token
 * @return
 */
static bool insert_token(StrBucket* bucket, char* token) {
    // If the bucket is empty, then insert the token as the instruction type.
    // The bucket is only ever empty when the instruction type has not been set.
    // The bucket containing the instruction type should contain nothing else. See @class Lexer.c file summary.
    const bool is_bucket_empty = str_bucket_get_size(bucket) == 0;
    if (is_bucket_empty) {
        str_bucket_insert_bucket(bucket);
        str_bucket_insert_str(bucket, BUCKET_TYPE_ID, token);
        return true;
    }

    // If the current token is "with", then the id bucket is now over. Insert a new bucket for the first parameter.
    const bool is_end_of_header = strcmp(token, "with") == 0;
    if (is_end_of_header) {
        return true;
    }

    // If the current token ends with a comma, then the current parameter group is over. Insert a new bucket for the
    // next parameter.
    const bool is_end_of_parameter = str_remove_trailing_delimiters(token, ",") > 0;
    const int bucket_id = str_bucket_get_size(bucket) - 1;
    str_bucket_insert_str(bucket, bucket_id, token);

    return is_end_of_parameter;
}

/**
 * Tokenizes and groups a string into buckets based on the ignored_chars parameter.
 *
 * See @class Lexer.c file summary for a more detailed explanation.
 *
 * @param str_instruction
 * @param ignored_chars
 * @return
 */
static void tokenize_and_insert(StrBucket* bucket, char* str_instruction, const char* ignored_chars) {
    char* current_token = str_instruction;
    bool is_bucket_full = false;

    while(*current_token) {
        // Remove leading ignored chars. If the token becomes empty, then do nothing. This happens when the entire
        // string is composed of ignored chars.
        current_token += str_remove_leading_ignored_chars(current_token, ignored_chars);
        if (current_token == NULL || *current_token == '\0') {
            break;
        }

        // Finds the first ignored character. The string composed of characters before the ignored character is the
        // current token. The ignored character is replaced with a null terminator to end the current token.
        char* next_ignored_char = strpbrk(current_token, ignored_chars);
        if (next_ignored_char != NULL) {
            *next_ignored_char = '\0';
        }

        // If the current bucket is full (i.e., completed), then insert a new bucket to be used.
        if (is_bucket_full) {
            str_bucket_insert_bucket(bucket);
        }

        // Insert the current token into the bucket. Based on the token, the bucket may become full. A bucket is full
        // when the token is an instruction type, id, is "with", or the token ends with a comma signifying the end of
        // the parameter group.
        is_bucket_full = insert_token(bucket, current_token);

        // Set the beginning of the next token to the character after the newly inserted null terminator.
        if (next_ignored_char) {
            current_token = next_ignored_char + 1;
        } else {
            break;
        }
    }
}

/**
 * Tokenizes and groups a string into buckets based on the ignored_chars parameter.
 *
 * See @class Lexer.c file summary for a more detailed explanation.
 *
 * @param str_instruction
 * @param ignored_chars
 * @return
 */
StrBucket* tokenize_to_buckets(char* str_instruction, const char* ignored_chars) {
    assert(str_instruction != NULL, "Attempting to bucket NULL string.");
    assert(ignored_chars != NULL, "Attempting to bucket string with NULL ignored_chars.");

    str_remove_trailing_delimiters(str_instruction, ",\n");
    if(strlen(str_instruction) == 0) {
        return NULL;
    }

    StrBucket* bucket = str_bucket_new(1, 1, true);
    tokenize_and_insert(bucket, str_instruction, ignored_chars);

    if (str_bucket_get_size(bucket) == 0) {
        str_bucket_delete(&bucket);
        return NULL;
    }

    return bucket;
}