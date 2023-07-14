#include <gb/gb.h>
#include <string.h>

const char memoryTestString[] = "String for testing RAM init";

char initString[sizeof(memoryTestString)];
uint8_t score;
uint16_t unlockedLevels;

void initialiseMemory() {
    if (memcmp(initString, memoryTestString, sizeof(memoryTestString)) != 0) {
        memcpy(initString, memoryTestString, sizeof(memoryTestString));
        score = 0;
        unlockedLevels = 0b11;
    }
}
