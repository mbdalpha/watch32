// QDSP-2000 display test sketch for Wokwi
// Serial commands: type text (up to 8 chars), "demo", "count", "clear"

#include "qdsp_display.h"

const uint8_t DATA_PINS[7] = {13, 12, 14, 27, 26, 25, 33};
const uint8_t ADDR_PINS[3] = {15, 2, 4};
const uint8_t CE_PIN  = 16;
const uint8_t RST_PIN = 17;

QDSP_Smart display(DATA_PINS, ADDR_PINS, CE_PIN, RST_PIN);

char virtualDisplay[9] = "        ";

enum Mode { MODE_IDLE, MODE_DEMO, MODE_COUNT };
Mode currentMode = MODE_IDLE;
unsigned long lastUpdate = 0;

void printDisplay() {
    Serial.println();
    Serial.println(" +---+---+---+---+---+---+---+---+");
    Serial.print(  " | ");
    for (int i = 0; i < 8; i++) {
        Serial.print(virtualDisplay[i]);
        Serial.print(" | ");
    }
    Serial.println();
    Serial.println(" +---+---+---+---+---+---+---+---+");
    Serial.println("  [0] [1] [2] [3] [4] [5] [6] [7]");
}

void displayAndPrint(const char* text) {
    for (int i = 0; i < 8; i++) {
        if (text[i] == '\0') {
            for (int j = i; j < 8; j++)
                virtualDisplay[j] = ' ';
            break;
        } else {
            virtualDisplay[i] = text[i];
        }
    }
    virtualDisplay[8] = '\0';
    display.writeString(virtualDisplay);
    printDisplay();
}

const char* demoMessages[] = {
    "QDSP2000", "ESP32   ", "Hello!  ", "COOL    ", "Watch32 "
};
const int demoCount = 5;
int demoIndex = 0;

void runDemo() {
    if (millis() - lastUpdate >= 1000) {
        lastUpdate = millis();
        displayAndPrint(demoMessages[demoIndex]);
        demoIndex = (demoIndex + 1) % demoCount;
    }
}

unsigned long countValue = 0;

void runCount() {
    if (millis() - lastUpdate >= 500) {
        lastUpdate = millis();
        char buf[9];
        snprintf(buf, sizeof(buf), "%8lu", countValue);
        displayAndPrint(buf);
        countValue = (countValue + 1) % 100000000;
    }
}

void handleSerial() {
    if (!Serial.available()) return;

    String input = "";
    while (Serial.available()) {
        char c = Serial.read();
        if (c == '\n' || c == '\r') continue;
        input += c;
    }
    if (input.length() == 0) return;

    Serial.print("> ");
    Serial.println(input);

    if (input.equalsIgnoreCase("demo")) {
        currentMode = MODE_DEMO;
        demoIndex = 0;
        lastUpdate = 0;
        Serial.println("[demo mode]");
    } else if (input.equalsIgnoreCase("clear")) {
        currentMode = MODE_IDLE;
        display.clear();
        displayAndPrint("        ");
    } else if (input.equalsIgnoreCase("count")) {
        currentMode = MODE_COUNT;
        countValue = 0;
        lastUpdate = 0;
        Serial.println("[count mode]");
    } else {
        currentMode = MODE_IDLE;
        char buf[9] = "        ";
        for (int i = 0; i < 8 && i < (int)input.length(); i++)
            buf[i] = input[i];
        buf[8] = '\0';
        displayAndPrint(buf);
    }
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println("QDSP-2000 Display Test");
    Serial.println("Commands: <text>, demo, count, clear");
    Serial.println();

    display.begin();
    delay(100);
    displayAndPrint("Hello!  ");
}

void loop() {
    handleSerial();

    switch (currentMode) {
        case MODE_DEMO:  runDemo();  break;
        case MODE_COUNT: runCount(); break;
        default: break;
    }
}
