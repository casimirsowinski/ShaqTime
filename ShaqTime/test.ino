///*
//Shaq Time LED bar light strip program
//Casimir Sowinski 2017
//This program runs two 300 LED three channel addressable LED strips in series.
//There are a total of 600 LEDs with two seperate 5V, 2A power supplies. The strips
//are compatible with the Pololu LED strip library. Some of this code is from the 
//library's example files. 
//*/
//
//#include <PololuLedStrip.h>
//#include <EEPROM.h>
//
///*Define strip*/
//#define LED_PIN 12
//PololuLedStrip<LED_PIN> ledStrip;
//
///*Struct array to hold colors values in RGB color space*/
//#define LED_COUNT 600
//rgb_color colors[LED_COUNT] = { 0 };
//
///*Shaq Ready vars*/
//bool init_SR = true;
//
//
//void setup() {
//		/*For debugging*/
//		Serial.begin(115200);
//
//		/*Pin modes*/
//		pinMode(LED_PIN, OUTPUT);
//}
//
//void loop() {
//
//		/*Shaq Ready test*/
//		if (init_SR) {
//				for (int i = 0; i < LED_COUNT; i++) {
//						unsigned int init_r = random(255);
//						unsigned int init_g = random(255);
//						unsigned int init_b = random(255);
//						rgb_color init_color{ init_r, init_g, init_b };
//						colors[i] = init_color;
//				}
//				init_SR = false;
//				ledStrip.write(colors, LED_COUNT);
//		}
//
//
//
//		
//		
//		ledStrip.write(colors, LED_COUNT);
//
//}