#include <ber_tlv_reader.h>

enum States
{
    ST_IDLE,
    ST_TAG,
    ST_TAG2,
    ST_LEN,
    ST_VAL
};

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
                    primitive = false;
                } 

                uint8_t b54321 = (objects[i] & 0x1F);
                if (b54321 < 31) {
                    currentState = ST_LEN; // Is ready to move to next state
                } else {
                    currentState = ST_TAG2;
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
                tagVal[0] = toupper(tagVal[0]);
                tagVal[1] = toupper(tagVal[1]);
                strcat(tagString,tagVal);
                strcat(tagString," (");
                strcat(tagString,tagClass);
                strcat(tagString,", ");
                strcat(tagString,tagType);
                strcat(tagString,")\n");
                printIndent(indentationLevel);
                printf("%s",tagString);
                ++i; // Move to next byte
                uint32_t lowerCounter = pop(&valLengthStack);
                if (lowerCounter != INT_MIN) {
                    push(&valLengthStack,--lowerCounter);
                }
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
                    ++i;
                } else { 
                    // More than 1 byte length
                    currentState = ST_LEN;
                    uint8_t numberOfBytes = (objects[i] >> 7);
                    uint8_t bytes = 0;
                    while (bytes < numberOfBytes) { // Talvez precise mexer nessa condicao
                        valLength += (objects[i] << bytes*8);
                        ++bytes;
                        ++i;
                    }
                }
                uint32_t lowerCounter = pop(&valLengthStack);
                if (lowerCounter != INT_MIN) {
                    push(&valLengthStack,(lowerCounter-(i-startingI)));
                }
                push(&valLengthStack,valLength);
                sprintf(lenVal,"%d",valLength);
                int totalLen = strlen("LEN - ") + strlen(lenVal) + strlen(" bytes\n");
                lengthString = (char*)malloc(totalLen * sizeof(char));
                strcpy(lengthString,"LEN - ");
                strcat(lengthString,lenVal);
                strcat(lengthString," bytes\n");
                printIndent(indentationLevel);
                printf("%s",lengthString);

                if (primitive) {
                    currentState = ST_VAL;
                } else {
                    currentState = ST_TAG; // Checar se deveria ser ST_IDLE
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
                    valString = (char*)malloc(totalLen * sizeof(char));
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
                    }
                    else {
                        push(&valLengthStack,bytesLeft);
                    }
                    lowerCounter = pop(&valLengthStack);
                }
                /* uint32_t lowerCounter = pop(&valLengthStack);
                if (lowerCounter != INT_MIN) {
                    int32_t bytesLeft = (int32_t)(lowerCounter-(i-startingI));
                    printf("bytes left: %d\n", bytesLeft);
                    if (bytesLeft <= 0) {
                        --indentationLevel;
                        printf("level: %d\n", indentationLevel);
                    }
                    push(&valLengthStack,bytesLeft);
                } else {
                    printf("INT_MIN\n");
                }*/
                currentState = ST_IDLE;
            }
            default:
            {
                break;
            }
        }
    }
}
