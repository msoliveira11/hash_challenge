#include <ber_tlv_reader.h>

void printIndent(uint8_t level) {
    for (uint8_t i=0; i<level; ++i) {
        printf("  ");
    }
}

void printOutput(const size_t len, const uint8_t objects[]) {
    uint32_t i=0;
    uint8_t currentState=0;
    char *strs[len];
    struct StackNode* valLengthStack = NULL;
    bool primitive = false;
    int8_t indentationLevel = 0;

    while (i < len) {

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
                // Decoding first byte TAG field.
                char *tagClass;

                uint8_t b87 = objects[i] >> 6;
                if (b87 == 0) {
                    tagClass = (char*)malloc(strlen("universal class")+1 * sizeof(char));
                    strcpy(tagClass, "universal class");
                } else if (b87 == 1) {
                    tagClass = (char*)malloc(strlen("application class")+1 * sizeof(char));
                    strcpy(tagClass, "application class");
                } else if (b87 == 2) {
                    tagClass = (char*)malloc(strlen("context-specific class")+1 * sizeof(char));
                    strcpy(tagClass, "context-specific class");
                } else {
                    tagClass = (char*)malloc(strlen("private class")+1 * sizeof(char));
                    strcpy(tagClass, "private class");
                }

                char *tagType;
                uint8_t b6 = (objects[i] >> 5) & 0x1;
                if (b6 == 0) {
                    tagType = (char*)malloc(strlen("primitive")+1 * sizeof(char));
                    strcpy(tagType, "primitive");
                    primitive = true;
                } else {
                    tagType = (char*)malloc(strlen("constructed")+1 * sizeof(char));
                    strcpy(tagType, "constructed");
                    primitive = false;
                } 

                char *tagBytes;
                int tagBytesSize = 2;
                tagBytes = (char*)malloc(tagBytesSize+1 * sizeof(char));
                sprintf(tagBytes,"%x",objects[i]);
                tagBytes[0] = toupper(tagBytes[0]);
                tagBytes[1] = toupper(tagBytes[1]);
                uint8_t b54321 = (objects[i] & 0x1F);
                if (b54321 < 31) {
                    currentState = ST_LEN; // Is ready to move to next state
                } else {
                    // If TAG needs subsequent bits
                    // Concatenate this byte to TAG string
                    ++i; // Moves to next byte
                    while (1) {
                        tagBytesSize += 5; // 5 digits
                        tagBytes = (char*)realloc(tagBytes,tagBytesSize+1 * sizeof(char));
                        char newTagString[5];
                        char newVal[2];
                        sprintf(newVal,"%x",objects[i]);
                        newTagString[0] = ' ';
                        newTagString[1] = '0';
                        newTagString[2] = 'x';
                        newTagString[3] = toupper(newVal[0]);
                        newTagString[4] = toupper(newVal[1]);
                        strcat(tagBytes,newTagString);
                        if ((objects[i] >> 7) == 1) { // We must check next byte
                            ++i;
                        } else { // We may finish TAG decoding and move to LEN decoding
                            currentState = ST_LEN;
                            break;
                        }
                    }
                }
                char *tagString;
                int tagLength = 0;
                tagLength += strlen("TAG - 0x");
                tagLength += tagBytesSize; 
                tagLength += strlen(" (");
                tagLength += strlen(tagClass);
                tagLength += strlen(", ");
                tagLength += strlen(tagType);
                tagLength += strlen(")\n");
                tagString = (char*)malloc(tagLength+1 * sizeof(char));
                strcpy(tagString,"TAG - 0x");
                strcat(tagString,tagBytes);
                strcat(tagString," (");
                strcat(tagString,tagClass);
                strcat(tagString,", ");
                strcat(tagString,tagType);
                strcat(tagString,")\n");
                printIndent(indentationLevel);
                printf("%s",tagString);

                // Releasing memory after printing TAG on screen
                free(tagBytes);
                free(tagClass);
                free(tagType);
                free(tagString);

                ++i; // Move to next byte
                uint32_t lowerCounter = pop(&valLengthStack);
                if (lowerCounter != INT_MIN) {
                    push(&valLengthStack,--lowerCounter);
                }
                break;
            }
            case ST_LEN:            
            {
                // Storing actual i 
                uint8_t startingI = i;
                // Checks data length
                char *lengthString;
                char lenVal[3];
                if ((objects[i] >> 7) == 0) {
                    // Single byte length
                    valLength = (objects[i] & 0x7F);
                } else { 
                    // More than 1 byte length
                    // currentState = ST_LEN;
                    uint8_t numberOfBytes = (objects[i] & 0x7F); // number of subsequent bytes required to compose Length Value
                    uint8_t bytes = 1;
                    while (bytes <= numberOfBytes) {
                        ++i; // checking next byte
                        // we must shift this byte content left to concantenate with subsequent bytes
                        valLength += (objects[i] << (numberOfBytes-bytes)*8); 
                        ++bytes;
                    }
                }

                sprintf(lenVal,"%d",valLength);
                int totalLen = strlen("LEN - ") + strlen(lenVal) + strlen(" bytes\n");
                lengthString = (char*)malloc(totalLen+1 * sizeof(char));
                strcpy(lengthString,"LEN - ");
                strcat(lengthString,lenVal);
                strcat(lengthString," bytes\n");
                printIndent(indentationLevel);
                printf("%s",lengthString);

                // Releasing memory after printing TAG on screen
                free(lengthString);

                ++i; // move to next byte
                uint32_t lowerCounter = pop(&valLengthStack);
                if (lowerCounter != INT_MIN) {
                    push(&valLengthStack,(lowerCounter-(i-startingI)));
                }
                push(&valLengthStack,valLength);

                if (primitive) {
                    currentState = ST_VAL;
                } else {
                    currentState = ST_TAG;
                    printf("\n");
                    ++indentationLevel;
                }
                break;
            }
            case ST_VAL:
            {
                // Storing actual i 
                uint8_t startingI = i;
                char *valString;
                uint32_t valPos = 0;
                uint32_t currentLength = pop(&valLengthStack);
                if (currentLength != 0) {
                    int totalLen = strlen("VAL -") + 5*currentLength + strlen("\n");
                    valString = (char*)malloc(totalLen+1 * sizeof(char));
                    strcpy(valString,"VAL -");
                    while (valPos < currentLength) {
                        char val[5];
                        char hexVal[3];
                        if (objects[i] < 10) {
                            strcpy(val," 0x0");
                            sprintf(hexVal,"%x",objects[i]);
                            val[4] = toupper(hexVal[0]);
                        } else {
                            strcpy(val," 0x");
                            sprintf(hexVal,"%x",objects[i]);
                            val[3] = toupper(hexVal[0]);
                            val[4] = toupper(hexVal[1]);
                        }
                        ++i;
                        ++valPos;
                        strcat(valString,val);
                    }
                    strcat(valString,"\n");
                    printIndent(indentationLevel);
                    printf("%s\n",valString);
                    // Releasing memory after printing TAG on screen
                    free(valString);
                } else {
                    printf("\n");
                }

                uint32_t lowerCounter = pop(&valLengthStack);
                while (lowerCounter != INT_MIN) {
                    int32_t bytesLeft = (int32_t)(lowerCounter-(i-startingI));
                    //printf("bytes left: %d\n", bytesLeft);
                    if (bytesLeft <= 0) {
                        --indentationLevel;
                        //printf("level: %d\n", indentationLevel);
                    } else {
                        push(&valLengthStack,bytesLeft);
                        break;
                    }
                    lowerCounter = pop(&valLengthStack);
                }
                currentState = ST_IDLE;
            }
            default:
            {
                break;
            }
        }
    }
}
