//*********************************************************
const bool debugging = true;

void initDebugPrint(void) {
    if (debugging) {
        Serial1.begin(115200);
        Serial1.println("\n\nDEBUG enabled\n");
    }
}

void debugPrint(const char *msg) {
    if (debugging) {Serial1.print(msg);}
}

void debugPrint(char msg) {
    if (debugging) {Serial1.print(msg);}
}

void debugPrintln(const char *msg) {
    if (debugging) {Serial1.println(msg);}
}

void debugPrintln(char msg) {
    if (debugging) {Serial1.print(msg);}
}

void closeDebugPrint(void) {
    if (debugging) {Serial1.flush();}
}
