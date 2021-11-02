#include <stdio.h>
#include <ber_tlv_reader.h>

void runTestData() {

    size_t objLen = 21 + 256;

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
    tlvObject[15] = 0xDF;
    tlvObject[16] = 0xF0;
    tlvObject[17] = 0x70;
    tlvObject[18] = 0x82;
    tlvObject[19] = 0x01;
    tlvObject[20] = 0x00;

    for (size_t i=0; i<256; ++i) {
        tlvObject[i+21] = i;
    }

    printOutput(objLen,tlvObject); 

}

int main() {
    runTestData();
   
    return 0;
}

