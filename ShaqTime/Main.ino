/*
Shaq Time LED Control
Casimir Sowinski
2016-2017
*/

#include <PololuLedStrip.h>
//#include <EEPROM.h>
#include "colorConversion.h"

#define LED_COUNT		600
#define LED_PIN			10
#define BUT_PIN			8
#define KEY_PIN			9

/*Globals*/
typedef enum {
		idle,
		idlePulse,
		ready,
		active,
		activePulse
}state;

PololuLedStrip<LED_PIN> ledStrip;
/*Tracks overall state*/
int currentState;
/*How long to run pulses*/
const int activePulseDuration = 10;
const int idlePulseDuration = 20;
/*Current amount of pulse time left*/
int activePulseTimer = 0;
int idlePulseTimer = 0;
/*Holds color information for active, idle, and pulses*/
rgb_color colorReady[LED_COUNT] = { 0, 0, 0 };
rgb_color colorIdle[LED_COUNT] = { 0, 0, 0 };
rgb_color colorActive[LED_COUNT] = { 0, 0, 0 };
/*Holds parameters for trig functions*/
float theta_r, theta_g, theta_b;
float delta_theta_r = 0.03;
float delta_theta_g = 0.11;
float delta_theta_b = 0.07;
float const MAX_THETA = 6.2831;

/*Function prototypes*/
void advanceReady();
void advanceActive();
void advanceIdle();
bool debounce(int pin);
bool debounce2(int pin);
void prepReady();
void prepIdle();
void prepActive();

hsv colHSV = hsv{ 0, 1, 1 };
rgb colRGB = rgb{ 0, 0, 0 };
bool colorDirection = 0;

void setup() {								
		pinMode(LED_PIN, OUTPUT);
		pinMode(BUT_PIN, INPUT_PULLUP);
		pinMode(KEY_PIN, INPUT_PULLUP);
		/*Allow pull-ups time to raise to input voltage*/
		delay(10); 

		/*Check key switch, set starting state*/
		if (debounce(KEY_PIN)) {
				currentState = ready;
				prepReady();
		}
		else {
				currentState = idle;
				prepIdle();
		}
}

void loop() {
		/*Check key state, switch to correct mode*/
		if (currentState == idle) {
				if (debounce2(KEY_PIN)) {
						currentState = ready;
						prepReady();
				}
		}
		else {
				if (!debounce2(KEY_PIN)) {
						currentState = idle;
						prepIdle();
				}
		}

		/*Handle states, check for button presses to start pulses*/
		if (currentState == ready) {
				advanceReady();
				/*Check for button press, transition to active*/
				if (debounce2(BUT_PIN)) {
						currentState = active;
						prepActive();
						advanceActive();
				}
		}
		else if (currentState == active) {
				advanceActive();
				/*Check for button press, start active pulse*/
				if (debounce2(BUT_PIN)) {
						activePulseTimer = activePulseDuration;
				}
		}
		else if (currentState == idle) {
				advanceIdle();
				/*Check for button press, start idle pulse*/
				if (debounce2(BUT_PIN)) {
						idlePulseTimer = idlePulseDuration;
				}
		}
}

void advanceReady() {
		unsigned int rMax = random(255);
		unsigned int gMax = random(1);
		unsigned int bMax = random(1);
		//colorReady[LED_COUNT / 2 - 1] = rgb_colorReady{ random(rMax), random(gMax), random(bMax) };
		//colorReady[LED_COUNT / 2] = rgb_colorReady{ random(rMax), random(gMax), random(bMax) };
		//colorReady[LED_COUNT / 2 - 1] = rgb_colorReady{ rMax, gMax, bMax };
		//colorReady[LED_COUNT / 2] = rgb_colorReady{ rMax, gMax, bMax };
		int omega = 2;
		/*downstream (right)*/
		colorReady[LED_COUNT / 2] = rgb_color{ 120 + 120 * sin(theta_r), gMax, 120 + 120 * sin(theta_b + 0.2) };
		/*closest to arduino (left)*/
		colorReady[LED_COUNT / 2 - 1] = rgb_color{ 0 + 64 * sin(theta_r), gMax, 0 + 64 * sin(theta_b) };

		//colorReady[LED_COUNT / 2 - 1] = rgb_colorReady{ rMax, gMax, bMax };
		//gummy worm
		//colorReady[LED_COUNT / 2] = rgb_colorReady{128 + 128 * sin(theta_r), 128 + 128 * sin(theta_g),  bMax};
		//robocop
		//colorReady[LED_COUNT / 2] = rgb_colorReady{128 + 128 * sin(theta_r), gMax, 128 + 128 * sin(theta_b)};

		theta_r = theta_r + delta_theta_r;
		theta_g = theta_g + delta_theta_g;
		theta_b = theta_b + delta_theta_b;

		//iv effect
		if (theta_r > MAX_THETA) {
				theta_r = 0;
		}
		if (theta_g > MAX_THETA) {
				theta_g = 0;
		}
		if (theta_b > MAX_THETA) {
				theta_b = 0;
		}

		// Propagate colorReady from the center outwards
		for (int i = 1; i < LED_COUNT / 2 - 1; i++) {
				colorReady[i] = colorReady[i + 1];
		}
		for (int i = LED_COUNT; i > LED_COUNT / 2; i--) {
				colorReady[i] = colorReady[i - 1];
		}

		//for (int i = 0; i < LED_COUNT; i++) {
		//		colorReady[i] = rgb_colorReady{ 100, 0, 0 };
		//}

		// Write out colorReady					
		ledStrip.write(colorReady, LED_COUNT);

}

void advanceActive() {
		/*Pick random LED not near edges, make blueish*/
		const int SPREAD_MAX = 10;
		int spread = random(SPREAD_MAX);
		int center = random(LED_COUNT - 2 * SPREAD_MAX) + SPREAD_MAX;
		unsigned char r = random(20);
		unsigned char g = r;
		unsigned char b = random(255);
		colorActive[center] = rgb_color{ r, g, b };
		while (spread > 0) {
				colorActive[center - spread] = colorActive[center];
				colorActive[center + spread] = colorActive[center];
				spread--;
		}

		/*Pick one more random LED, make greenish*/
		spread = random(SPREAD_MAX);
		center = random(LED_COUNT - 2 * SPREAD_MAX) + SPREAD_MAX;
		r = random(20);
		g = random(255);
		b = r;
		colorActive[center] = rgb_color{ r, g, b };
		while (spread > 0) {
				colorActive[center - spread] = colorActive[center];
				colorActive[center + spread] = colorActive[center];
				spread--;
		}

		/*Pick one more random LED, make greenish*/
		spread = random(SPREAD_MAX);
		center = random(LED_COUNT - 2 * SPREAD_MAX) + SPREAD_MAX;
		r = random(255);
		g = r;
		b = random(20);
		colorActive[center] = rgb_color{ r, g, b };
		while (spread > 0) {
				colorActive[center - spread] = colorActive[center];
				colorActive[center + spread] = colorActive[center];
				spread--;
		}


		float mult = .25;


		if (activePulseTimer > 0) {
		//if (debounce2(BUT_PIN)){
				unsigned char pulseColor = random(200);
				unsigned char rand1 = random(255);
				unsigned char rand2 = random(255);
				unsigned char rand3 = random(255);
				/*downstream (right)*/
				colorActive[LED_COUNT / 2] = rgb_color{ rand1, rand2, rand3 };
				/*closest to arduino (left)*/
				colorActive[LED_COUNT / 2 - 1] = rgb_color{ rand3, rand1, rand2 };

				// Propagate colorActive from the center outwards
				for (int i = 1; i < LED_COUNT / 2 - 1; i++) {
						colorActive[i] = colorActive[i + 1];
				}
				for (int i = LED_COUNT; i > LED_COUNT / 2; i--) {
						colorActive[i] = colorActive[i - 1];
				}

				// Propagate colorActive towards the center
				//for (int i = LED_COUNT / 2 - 1; i >= 0; i++) {
				//		colorActive[i] = colorActive[i - 1];
				//}
				//for (int i = LED_COUNT / 2; i >= LED_COUNT ; i--) {
				//		colorActive[i] = colorActive[i + 1];
				//}

				activePulseTimer--;
		}
		else {

				// Propagate colorActive from the center outwards
				for (int i = 1; i < LED_COUNT / 2 - 1; i++) {
						colorActive[i] = colorActive[i + 1];
				}
				for (int i = LED_COUNT; i > LED_COUNT / 2; i--) {
						colorActive[i] = colorActive[i - 1];
				}
				// Propagate colorActives in both directions
				for (int i = 1; i < LED_COUNT; i++) {
						colorActive[i].red = mult * (colorActive[i].red + colorActive[i - 1].red + colorActive[i + 1].red);
						colorActive[i].green = mult * (colorActive[i].red + colorActive[i - 1].red + colorActive[i + 1].green);
						colorActive[i].blue = mult * (colorActive[i].green + colorActive[i - 1].blue + colorActive[i + 1].blue);
				}
		}
		//for (int i = 0; i < LED_COUNT; i++) {
		//		colorActive[i] = rgb_color{ 0, 0, 0 };
		//}

		///*HSV attempt*/
		//{
		//		/*Scan hue value of color up and down*/
		//		if (colorDirection == 0) {
		//				colHSV.h = colHSV.h + 0.5;
		//				if (colHSV.h > 360) {
		//						colHSV.h = 360;
		//						colorDirection = 1;
		//				}

		//		}
		//		else {
		//				colHSV.h = colHSV.h - 0.5;
		//				if (colHSV.h < 0) {
		//						colHSV.h = 0;
		//						colorDirection = 0;
		//				}
		//		}
		//		colRGB = hsv2rgb(colHSV);
		//		rgb_color colorRGB256 = rgb_color{ colRGB.r, colRGB.g, colRGB.b };
		//		colorActive[LED_COUNT / 2] = colorRGB256;
		//		/*closest to arduino (left)*/
		//		colorActive[LED_COUNT / 2 - 1] = colorRGB256;

		//		// Propagate colorActive from the center outwards
		//		for (int i = 1; i < LED_COUNT / 2 - 1; i++) {
		//				colorActive[i] = colorActive[i + 1];
		//		}
		//		for (int i = LED_COUNT; i > LED_COUNT / 2; i--) {
		//				colorActive[i] = colorActive[i - 1];
		//		}
		//}
		
		ledStrip.write(colorActive, LED_COUNT);
}

void advanceIdle() {
		/*Pick random LED not near edges*/
		const int SPREAD_MAX = 10;
		int spread = random(SPREAD_MAX);
		int center = random(LED_COUNT - 2 * SPREAD_MAX) + SPREAD_MAX;
		unsigned char r = random(100);
		unsigned char g = random(100);
		unsigned char b = random(100);
		colorIdle[center] = rgb_color{ r, g, b };
		while (spread > 0) {
				colorIdle[center - spread] = colorIdle[center];
				colorIdle[center + spread] = colorIdle[center];
				spread--;
		}
		float mult = .3;
		// Propagate colorIdles in both directions
		for (int i = 1; i < LED_COUNT; i++) {
				colorIdle[i].red = 0.9 * mult * (colorIdle[i].red + colorIdle[i - 1].red + colorIdle[i + 1].red) + idlePulseTimer / 2;
				colorIdle[i].green = 0.8 * mult * (colorIdle[i].red + colorIdle[i - 1].green + colorIdle[i + 1].green);
				colorIdle[i].blue = 0.5 * mult * (colorIdle[i].green + colorIdle[i - 1].blue + colorIdle[i + 1].blue);

				/*Check bounds*/
				if (colorIdle[i].red > 250)
						colorIdle[i].red = 250;
				if (colorIdle[i].green > 250)
						colorIdle[i].green = 250;
				if (colorIdle[i].blue > 250)
						colorIdle[i].blue = 250;
				if (colorIdle[i].red < 0)
						colorIdle[i].red = 0;
				if (colorIdle[i].green < 0)
						colorIdle[i].green = 0;
				if (colorIdle[i].blue < 0)
						colorIdle[i].blue = 0;
		}

		if (idlePulseTimer > 0) {
				/*Pulse just starting*/
				if (idlePulseTimer == idlePulseDuration) {
						//for (int i = 1; i < LED_COUNT; i++) {
						//		/*Reset pulse colorIdles*/
						//		colorIdlePulse[i] = { 0, 0, 0 };
						//}
				}				
				idlePulseTimer--;
		}

		ledStrip.write(colorIdle, LED_COUNT);
}

void prepReady() {
		for (int i = 0; i < LED_COUNT; i++) {
				//colorReady[i] = rgb_color{ 10, 60 + 60 * sin(6.28 * i / LED_COUNT), 20 + 20 * sin(3.14 * i / LED_COUNT) };
				colorReady[i] = rgb_color{ 0, 0, 0 };
		}
}

void prepIdle() {
		for (int i = 0; i < LED_COUNT; i++) {
				colorIdle[i] = rgb_color{ 40 + 40 * sin(13 * i / LED_COUNT), 0, 0 };
		}
}

void prepActive() {
		for (int i = 0; i < LED_COUNT; i++) {
				colorActive[i] = rgb_color{ 1, 1, 1};
		}
}

/*If switch on pin is pressed wait for a time, check pin again
and return state to filter out mechanical oscillations. (active
low logic)*/
bool debounce(int pin) {
		/*If button IS pressed*/
		if (digitalRead(pin) == 0) {
				delay(20);
				if (digitalRead(pin) == 0) {
						return true;
				}
				else {
						return false;
				}
		}
		else {
				return false;
		}
}

bool debounce2(int pin) {
		return !digitalRead(pin);
}