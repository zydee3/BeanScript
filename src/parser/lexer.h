#ifndef BEANSCRIPT_LEXER_H
#define BEANSCRIPT_LEXER_H

#include <string.h>
#include <stdlib.h>


#include "../utility/str_bucket.h"
#include "../main.h"

extern const char* BUCKET_TYPE_NAME;
extern const char* BUCKET_ID_NAME;
extern const char* BUCKET_PARAMETER_NAME;

typedef enum {
    PARSING_OPERATION,
    PARSING_ID,
    PARSING_PARAMETER,
} ParsingState;

StrBucket* tokenize_to_buckets(char* str_instruction, const char* ignored_chars);

#endif //BEANSCRIPT_LEXER_H
