//*********************************************************
struct structEEPROM {
    int cycles;
};

union {
    structEEPROM eevar;
    char eeArray[sizeof(structEEPROM)];
} EEPROMData;

void readEEPROM(void) {
    for (int i=0; i<sizeof(structEEPROM); i++) {
        EEPROMData.eeArray[i] = EEPROM.read(i);
    }
}

void writeEEPROM(void) {
    for (int i=0; i<sizeof(structEEPROM); i++) {
        EEPROM.write(i, EEPROMData.eeArray[i]);
    }
}
