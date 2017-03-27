///*
//Shaq Time LED Control
//Casimir Sowinski
//2016-2017
//*/
//
//#include <PololuLedStrip.h>
////#include <EEPROM.h>
//
//#define LED_COUNT		600
//#define LED_PIN			10
//#define BUT_PIN			8
//#define KEY_PIN			9
//
///*Globals*/
//rgb_color col_00[LED_COUNT] = { 0, 0, 0 };
//rgb_color col_01[LED_COUNT] = { 0, 0, 0 };
//rgb_color col_10[LED_COUNT] = { 0, 0, 0 };
//rgb_color col_11[LED_COUNT] = { 0, 0, 0 };
//bool g_primaryState = 0;
//bool g_secondaryState = 0;
//
///*Function prototypes*/
//void advance_00();
//void advance_01();
//void advance_10();
//void advance_11();
//bool debounce(int pin);
//
//
//void setup() {								
//		pinMode(LED_PIN, OUTPUT);
//		pinMode(BUT_PIN, INPUT_PULLUP);
//		pinMode(KEY_PIN, INPUT_PULLUP);
//		/*Allow pull-ups time to raise to input voltage*/
//		delay(10); 
//}
//
//void loop() {
//
//
//}
//
//
///*If switch on pin is pressed wait for a time, check pin again
//and return state to filter out mechanical oscillations. (active
//low logic)*/
//bool debounce(int pin) {
//		/*If button IS pressed*/
//		if (digitalRead(pin) == 0) {
//				delay(20);
//				if (digitalRead(pin) == 0) {
//						return true;
//				}
//				else {
//						return false;
//				}
//		}
//		else {
//				return false;
//		}
//}
//
//void advance_00() {
//
//}
//void advance_01() {
//
//}
//void advance_10() {
//
//}
//void advance_11() {
//
//}