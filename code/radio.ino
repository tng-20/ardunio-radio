#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define TEA5767_ADDRESS 0x60
#define POT_PIN A3     
#define LIST_MAX 10

LiquidCrystal_I2C lcd(0x27, 20, 4);

int readings[LIST_MAX] = {0};
int currentIndex = 0;
long frequencyTotal = 0;
float oldFrequency = 0;

void setFrequency(float freq);
void sendData(byte *data);

void setup() {
    Serial.begin(9600);
    lcd.init();
    lcd.backlight();
    Wire.begin();
}

void loop() {
    int channel = analogRead(POT_PIN);
    
    frequencyTotal = frequencyTotal - readings[currentIndex];  
    readings[currentIndex] = channel;  
    frequencyTotal = frequencyTotal + readings[currentIndex];  

    currentIndex = (currentIndex + 1) % LIST_MAX;  

    float channelFrequency = frequencyTotal / LIST_MAX;
    float frequency = (0.031280547 * channelFrequency) + 76;
    
    if (abs(frequency - oldFrequency) > 0.1) {
        lcd.clear();
        Serial.print("Tuning to: ");
        Serial.println(frequency);
        setFrequency(frequency);
        oldFrequency = frequency;
        lcd.setCursor(5,0);
        lcd.print(frequency);
    }
}

//Functions for TEA5767

void setFrequency(float freq) {
    uint16_t freqB = (4 * ((freq * 1000000) + 225000)) / 32768;
    
    byte data[5];
    data[0] = (freqB >> 8) & 0xFF;
    data[1] = freqB & 0xFF;
    data[2] = 0xB0;
    data[3] = 0x10;
    data[4] = 0x00;
    sendData(data);
}

void sendData(byte *data) {
    Wire.beginTransmission(TEA5767_ADDRESS);
    for (int i = 0; i < 5; i++) {
        Wire.write(data[i]);
    }
    Wire.endTransmission();
}
