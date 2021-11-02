/* --------------------------------------------------------------
 * file: ber_tlv_reader.h
 * author: Matheus Santos de Oliveira
 * e-mail: msoliveira.eca@gmail.com
 * description: Header file for tlv data decoder. 
 * Guidelines to this implementation may be found in the file
 * "ber_tlv_algorithm.txt" found in this repository.
 * --------------------------------------------------------------
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "stack.h"

// The decoding implementation is based on a State Machine.
// Each state focus on one of the possible classifications for each byte.
enum States
{
    ST_IDLE,
    ST_TAG,
    ST_LEN,
    ST_VAL
};


// This method is responsible for adjusting the indentation
// between lines printed.
// It receives the current indentation level to be adjusted.
void printIndent(uint8_t level);

// Main method, which contains the state machine, responsible
// for decoding data, and printing it to screen.
// It receives both array of bytes, along with it's length.
void printOutput(const size_t len, const uint8_t objects[]);
