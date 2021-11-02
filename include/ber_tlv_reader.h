#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "stack.h"

enum States
{
    ST_IDLE,
    ST_TAG,
    ST_LEN,
    ST_VAL
};

void printIndent(uint8_t level);

void printOutput(const size_t len, const uint8_t objects[]);


