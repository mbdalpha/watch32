// QDSP/HDSP-2000 8-character smart display driver for ESP32
// Reference: https://www.instructables.com/Arduino-QDSP-Display/

#pragma once

#include <Arduino.h>

class QDSP_Smart {
public:
    QDSP_Smart(const uint8_t dataPins[7], const uint8_t addrPins[3],
               uint8_t cePin, uint8_t rstPin)
        : _cePin(cePin), _rstPin(rstPin)
    {
        memcpy(_dataPins, dataPins, 7);
        memcpy(_addrPins, addrPins, 3);
    }

    void begin() {
        for (int i = 0; i < 7; i++) pinMode(_dataPins[i], OUTPUT);
        for (int i = 0; i < 3; i++) pinMode(_addrPins[i], OUTPUT);
        pinMode(_cePin, OUTPUT);
        pinMode(_rstPin, OUTPUT);

        digitalWrite(_cePin, HIGH);
        reset();
    }

    void reset() {
        digitalWrite(_rstPin, LOW);
        delayMicroseconds(10);
        digitalWrite(_rstPin, HIGH);
        delayMicroseconds(100);
    }

    // Write ASCII char to digit position 0-7
    void writeChar(uint8_t pos, char ascii) {
        for (int i = 0; i < 3; i++)
            digitalWrite(_addrPins[i], (pos >> i) & 0x01);

        // 7-bit ASCII — D7 must be tied to GND
        for (int i = 0; i < 7; i++)
            digitalWrite(_dataPins[i], (ascii >> i) & 0x01);

        // Pulse CE to latch
        delayMicroseconds(1);
        digitalWrite(_cePin, LOW);
        delayMicroseconds(1);
        digitalWrite(_cePin, HIGH);
        delayMicroseconds(1);
    }

    void writeString(const char* str, uint8_t maxChars = 8) {
        for (uint8_t i = 0; i < maxChars && str[i] != '\0'; i++)
            writeChar(i, str[i]);
    }

    void clear() {
        for (uint8_t i = 0; i < 8; i++)
            writeChar(i, ' ');
    }

private:
    uint8_t _dataPins[7];
    uint8_t _addrPins[3];
    uint8_t _cePin;
    uint8_t _rstPin;
};
