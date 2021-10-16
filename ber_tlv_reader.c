#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

enum States
{
    ST_IDLE,
    ST_TAG,
    ST_TAG2,
    ST_LEN,
    ST_VAL
};

void printOutput(const size_t len, const uint8_t objects[]) {
    uint32_t i=0;
    uint8_t currentState=0;
    char *strs[len];

    while (i < len) {
        bool primitive = false;
        uint32_t valLength = 0;
        switch (currentState) 
        {
            case ST_IDLE:
            {
                // Beginning of the process.
                if ((objects[i] == 0x00) || (objects[i] == 0xFF)) {
                    ++i;
                } else {
                    currentState = ST_TAG;
                }
                break;
            }
            case ST_TAG:
            {
                // Decoding TAG field.
                char *tagClass;
                uint8_t b87 = objects[i] >> 6;
                if (b87 == 0) {
                    tagClass = (char*)malloc(strlen("universal class") * sizeof(char));
                    strcat(tagClass, "universal class");
                } else if (b87 == 1) {
                    tagClass = (char*)malloc(strlen("application class") * sizeof(char));
                    strcat(tagClass, "application class");
                } else if (b87 == 2) {
                    tagClass = (char*)malloc(strlen("context-specific class") * sizeof(char));
                    strcat(tagClass, "context-specific class");
                } else {
                    tagClass = (char*)malloc(strlen("private class") * sizeof(char));
                    strcat(tagClass, "private class");
                }

                char *tagType;
                uint8_t b6 = (objects[i] >> 5) & 0x1;
                if (b6 == 0) {
                    tagType = (char*)malloc(strlen("primitive") * sizeof(char));
                    strcat(tagType, "primitive");
                    primitive = true;
                } else {
                    tagType = (char*)malloc(strlen("constructed") * sizeof(char));
                    strcat(tagType, "constructed");
                } 

                uint8_t b54321 = (objects[i] & 0x1F);
                if (b54321 < 31) {
                    currentState = ST_LEN; // Is ready to move to next state
                    printf("ST_LEN\n");
                } else {
                    currentState = ST_TAG2;
                    printf("ST_TAG2\n");
                }
                char *tagString;
                int tagLength = 0;
                tagLength += strlen("TAG - 0x");
                tagLength += 2;
                tagLength += strlen(" (");
                tagLength += strlen(tagClass);
                tagLength += strlen(", ");
                tagLength += strlen(tagType);
                tagLength += strlen(")\n");
                tagString = (char*)malloc(tagLength * sizeof(char));
                strcpy(tagString,"TAG - 0x");
                char tagVal[2];
                sprintf(tagVal,"%x",objects[i]);
                strcat(tagString,tagVal);
                strcat(tagString," (");
                strcat(tagString,tagClass);
                strcat(tagString,", ");
                strcat(tagString,tagType);
                strcat(tagString,")\n");
                printf("%s",tagString);
                ++i; // Move to next byte
                break;
            }
            case ST_TAG2:
            {
                // If TAG needs subsequent bits
                // Concatenate this byte to TAG string
                if ((objects[i] >> 7) == 1) {
                    ++i;
                } else {
                    currentState = ST_LEN;
                }
                break;  
            }
            case ST_LEN:
            {
                // Checks data length
                char *lengthString;
                char lenVal[3];
                uint8_t bytes = 0;
                if ((objects[i] >> 7) == 0) {
                    // Single byte length
                    bytes = (objects[i] & 0x7F);
                    ++i;
                } else { 
                    // More than 1 byte length
                    currentState = ST_LEN;
                    uint32_t lenSize = 0;
                    uint8_t numberOfBytes = (objects[i] >> 7);
                    while (bytes < numberOfBytes) { // Talvez precise mexer nessa condicao
                        lenSize += objects[i];
                        ++bytes;
                    }
                }
                sprintf(lenVal,"%d",bytes);
                int totalLen = strlen("LEN - ") + strlen(lenVal) + strlen(" bytes\n");
                lengthString = (char*)malloc(totalLen * sizeof(char));
                strcpy(lengthString,"LEN - ");
                strcat(lengthString,lenVal);
                strcat(lengthString," bytes\n");
                printf("%s",lengthString);
                if (primitive) {
                    currentState = ST_VAL;
                } else {
                    currentState = ST_TAG; // Checar se deveria ser ST_IDLE
                }
                break;
            }
            case ST_VAL:
            {
                // Checks data length
                char *lengthString;
                char lenVal[3];
                uint8_t bytes = 0;
                if ((objects[i] >> 7) == 0) {
                    // Single byte length
                    bytes = (objects[i] & 0x7F);
                    ++i;
                } else { 
                    // More than 1 byte length
                    currentState = ST_LEN;
                    uint32_t lenSize = 0;
                    uint8_t numberOfBytes = (objects[i] >> 7);
                    while (bytes < numberOfBytes) { // Talvez precise mexer nessa condicao
                        lenSize += objects[i];
                        ++i;
                    }
                }
                sprintf(lenVal,"%d",bytes);
                int totalLen = strlen("LEN - ") + strlen(lenVal) + strlen(" bytes\n");
                lengthString = (char*)malloc(totalLen * sizeof(char));
                strcpy(lengthString,"LEN - ");
                strcat(lengthString,lenVal);
                strcat(lengthString," bytes\n");
                printf("%s",lengthString);
                if (primitive) {
                    currentState = ST_VAL;
                } else {
                    currentState = ST_TAG; // Checar se deveria ser ST_IDLE
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }
}



int main() {

    size_t objLen = 13;

    uint8_t tlvObject[objLen] = {
        0xE1, 0x0B, 0xC1, 0x03, 0x01, 0x02,
        0x03, 0xC2, 0x00, 0xC3, 0x02, 0xAA,
        0xBB
    };

    printOutput(objLen,tlvObject); 
   
    return 0;
}

