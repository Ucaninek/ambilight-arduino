#include <FastLED.h>

#define DATA_PIN 5
#define LED_PIN 2
#define NUM_LEDS 30

long lastHeartbeat = 0;

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("sa beyler");
}

void loop() {
  if (millis() - lastHeartbeat > 2500) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(0, 0, 0);
    }
    FastLED.show();
  }

  if (!Serial) return;
  if (Serial.available() <= 0) return;
  String data = Serial.readStringUntil('\n');
  if (data == "0,0") {
    lastHeartbeat = millis();
    return;
  }

  //if (Serial.available() >= 63) Serial.println("buffer overflow?");
  int delimIndx = data.indexOf(',');
  if (delimIndx == -1) return;  //bad data;
  int offset = data.substring(0, delimIndx).toInt();
  int length = data.substring(delimIndx + 1).toInt();
  if (length <= 0) return;  //bad data;

  //Serial.print("entering read mode with offset ");
  //Serial.print(String(offset));
  //Serial.print(" and with length ");
  //Serial.println(String(length));
  //digitalWrite(LED_PIN, 1);
  //digitalWrite(LED_BUILTIN, 0);
  byte buf[length];
  int bytesRead = Serial.readBytes(buf, length);
  if (bytesRead == 0) {
    Serial.println("read failed");
    digitalWrite(LED_BUILTIN, 1);
    return;
  } else if (bytesRead < length) Serial.println("byte stream cut off early, ignoring");

  for (int i = 0; i < bytesRead; i += 3) {
    leds[(i + offset) / 3] = CRGB(buf[i], buf[i + 1], buf[i + 2]);
  }

  //Serial.print("Wrote ");
  //Serial.print(String(bytesRead / 3));
  //Serial.println(" leds");

  digitalWrite(LED_PIN, 0);
  FastLED.show();
  lastHeartbeat = millis();
}