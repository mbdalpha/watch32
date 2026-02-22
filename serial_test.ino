void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("ESP32 Serial Test - Ready");
}

void loop() {
    if (Serial.available()) {
        String input = "";
        while (Serial.available()) {
            char c = Serial.read();
            if (c != '\n' && c != '\r') input += c;
        }
        if (input.length() > 0) {
            Serial.print("Echo: ");
            Serial.println(input);
        }
    }

    static unsigned long last = 0;
    if (millis() - last >= 3000) {
        last = millis();
        Serial.print("Uptime: ");
        Serial.print(millis() / 1000);
        Serial.println("s");
    }
}
