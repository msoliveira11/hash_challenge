#include <stdio.h>
#include <ber_tlv_reader.h>

int main() {

    size_t objLen = 19;

    uint8_t tlvObject[objLen];

    tlvObject[0] = 0xE1;
    tlvObject[1] = 0x09;
    tlvObject[2] = 0xC1;
    tlvObject[3] = 0x03;
    tlvObject[4] = 0x01;
    tlvObject[5] = 0x02;
    tlvObject[6] = 0x03;
    tlvObject[7] = 0xC2;
    tlvObject[8] = 0x00;
    tlvObject[9] = 0xE1;
    tlvObject[10] = 0x04;
    tlvObject[11] = 0xC3;
    tlvObject[12] = 0x02;
    tlvObject[13] = 0xAA;
    tlvObject[14] = 0xBB;
    tlvObject[15] = 0xC3;
    tlvObject[16] = 0x02;
    tlvObject[17] = 0xAA;
    tlvObject[18] = 0xBB;

    printOutput(objLen,tlvObject); 
   
    return 0;
}

