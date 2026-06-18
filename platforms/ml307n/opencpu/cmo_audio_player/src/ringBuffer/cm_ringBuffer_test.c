#include "cm_ringBuffer.h"

#include <nr_micro_shell.h>
#include <stdlib.h>

static uint8_t dataTest[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

void shell_test_my_ringBuffer(char argc, char **argv)
{
    CircularBuffer cb;
    initCircularBuffer(&cb, 10);

    writeCircularBuffer(&cb, dataTest, 10);

    size_t readSize = 0;
    uint8_t readBuffer[5];

    readSize = readCircularBuffer(&cb, readBuffer, 5);
    osPrintf("read %d bytes: ", readSize);
    for(int i = 0; i < readSize; ++i) {
        osPrintf(" %d", readBuffer[i]);
    }
    osPrintf("\r\n");


    readSize = readCircularBuffer(&cb, readBuffer, 3);
    osPrintf("read %d bytes: ", readSize);
    for(int i = 0; i < readSize; ++i) {
        osPrintf(" %d", readBuffer[i]);
    }
    osPrintf("\r\n");



    readSize = readCircularBuffer(&cb, readBuffer, 3);
    osPrintf("read %d bytes: ", readSize);
    for(int i = 0; i < readSize; ++i) {
        osPrintf(" %d", readBuffer[i]);
    }
    osPrintf("\r\n");

}


NR_SHELL_CMD_EXPORT(my_ringBuffer, shell_test_my_ringBuffer);

