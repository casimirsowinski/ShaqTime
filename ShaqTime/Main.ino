/*
Shaq Time LED Control
Casimir Sowinski
2016-2017
*/

#include <PololuLedStrip.h>
//#include <EEPROM.h>

#define LED_COUNT		599
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
const int activePulseDuration = 20;
const int idlePulseDuration = 20;
/*Current amount of pulse time left*/
int activePulseTimer = 0;
int idlePulseTimer = 0;
/*Holds color information for active, idle, and pulses*/
rgb_color colorReady[LED_COUNT] = { 0, 0, 0 };
rgb_color colorActivePulse[LED_COUNT] = { 0, 0, 0 };
rgb_color colorIdle[LED_COUNT] = { 0, 0, 0 };
rgb_color colorIdlePulse[LED_COUNT] = { 0, 0, 0 };

//bool g_primaryState = 0;
//bool g_secondaryState = 0;

/*Function prototypes*/
void advanceReady();
void advanceActive();
void advanceActivePulse();
void advanceIdle();
void advanceIdlePulse();
bool debounce(int pin);

void setup() {								
		Serial.begin(115200);
		pinMode(LED_PIN, OUTPUT);
		pinMode(BUT_PIN, INPUT_PULLUP);
		pinMode(KEY_PIN, INPUT_PULLUP);
		/*Allow pull-ups time to raise to input voltage*/
		delay(10); 

		/*Check key switch, set starting state*/
		if (debounce(KEY_PIN)) {
				currentState = ready;
		}
		else {
				currentState = idle;
		}
}

void loop() {
		/*Check key state, switch to correct mode*/
		if (currentState == idle) {
				if (debounce(KEY_PIN)) {
						currentState = ready;
				}
		}
		else {
				if (!debounce(KEY_PIN)) {
						currentState = idle;
				}
		}

		/*Handle states, check for button presses to start pulses*/
		if (currentState == ready) {
				advanceReady();
				/*Check for button press, transition to active*/
				if (debounce(BUT_PIN)) {
						currentState = active;
				}
		}
		else if (currentState == active) {
				advanceActive();
				/*Check for button press, start active pulse*/
				if (debounce(BUT_PIN)) {
						activePulseTimer = activePulseDuration;
				}
		}
		else if (currentState == idle) {
				advanceIdle();
				/*Check for button press, start idle pulse*/
				if (debounce(BUT_PIN)) {
						idlePulseTimer = idlePulseDuration;
				}
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

void advanceReady() {
		/*Pick random LED not near edges*/
		const int SPREAD_MAX = 10;
		int spread = random(SPREAD_MAX);
		int center = random(LED_COUNT - 2 * SPREAD_MAX) + SPREAD_MAX;
		unsigned char r = random(20);
		unsigned char g = r;
		unsigned char b = random(255);
		colorReady[center] = rgb_color{ r, g, b };
		while (spread > 0) {
				colorReady[center - spread] = colorReady[center];
				colorReady[center + spread] = colorReady[center];
				spread--;
		}
		float mult = .2;
		// Propagate colors in both directions
		for (int i = 1; i < LED_COUNT; i++) {
				colorReady[i].red = mult * (colorReady[i].red + colorReady[i - 1].red + colorReady[i + 1].red);
				colorReady[i].green = 0.8 * mult * (colorReady[i].red + colorReady[i - 1].green + colorReady[i + 1].green);
				colorReady[i].blue = 0.5 * mult * (colorReady[i].green + colorReady[i - 1].blue + colorReady[i + 1].blue);
		}

		ledStrip.write(colorReady, LED_COUNT);
}

void advanceActive() {
		/*Pick random LED not near edges*/
		const int SPREAD_MAX = 10;
		int spread = random(SPREAD_MAX);
		int center = random(LED_COUNT - 2 * SPREAD_MAX) + SPREAD_MAX;
		unsigned char r = random(20);
		unsigned char g = random(255);
		unsigned char b = r;
		colorReady[center] = rgb_color{ r, g, b };
		while (spread > 0) {
				colorReady[center - spread] = colorReady[center];
				colorReady[center + spread] = colorReady[center];
				spread--;
		}
		float mult = .3;
		// Propagate colors in both directions
		for (int i = 1; i < LED_COUNT; i++) {
				colorReady[i].red = mult * (colorReady[i].red + colorReady[i - 1].red + colorReady[i + 1].red);
				colorReady[i].green = 0.8 * mult * (colorReady[i].red + colorReady[i - 1].green + colorReady[i + 1].green);
				colorReady[i].blue = 0.5 * mult * (colorReady[i].green + colorReady[i - 1].blue + colorReady[i + 1].blue);
		}
		ledStrip.write(colorReady, LED_COUNT);
}

void advanceActivePulse() {

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
		// Propagate colors in both directions
		for (int i = 1; i < LED_COUNT; i++) {
				colorIdle[i].red = mult * (colorIdle[i].red + colorIdle[i - 1].red + colorIdle[i + 1].red) + idlePulseTimer / 2;
				colorIdle[i].green = 0.8 * mult * (colorIdle[i].red + colorIdle[i - 1].green + colorIdle[i + 1].green);
				colorIdle[i].blue = 0.5 * mult * (colorIdle[i].green + colorIdle[i - 1].blue + colorIdle[i + 1].blue);

				/*Check bounds*/
				if (colorIdle[i].red > 255)
						colorIdle[i].red = 255;
				if (colorIdle[i].green > 255)
						colorIdle[i].green = 255;
				if (colorIdle[i].blue > 255)
						colorIdle[i].blue = 255;
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
						//		/*Reset pulse colors*/
						//		colorIdlePulse[i] = { 0, 0, 0 };
						//}
				}				
				idlePulseTimer--;
		}


		ledStrip.write(colorIdle, LED_COUNT);
}

void advanceIdlePulse() {

}
