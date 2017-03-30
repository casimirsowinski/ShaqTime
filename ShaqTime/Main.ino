/*
Shaq Time LED Control
Casimir Sowinski
2016-2017
*/

#include <PololuLedStrip.h>
//#include <EEPROM.h>

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
const int activePulseDuration = 20;
const int idlePulseDuration = 20;
/*Current amount of pulse time left*/
int activePulseTimer = 0;
int idlePulseTimer = 0;
/*Holds color information for active, idle, and pulses*/
rgb_color color[LED_COUNT] = { 0, 0, 0 };
//rgb_color colorReady[LED_COUNT] = { 0, 0, 0 };
//rgb_color colorActivePulse[LED_COUNT] = { 0, 0, 0 };
//rgb_color colorIdle[LED_COUNT] = { 0, 0, 0 };
//rgb_color colorIdlePulse[LED_COUNT] = { 0, 0, 0 };

//bool g_primaryState = 0;
//bool g_secondaryState = 0;

float theta_r, theta_g, theta_b;
float delta_theta_r = 0.03;
float delta_theta_g = 0.11;
float delta_theta_b = 0.07;
float const MAX_THETA = 6.2831;

/*Function prototypes*/
void advanceReady();
void advanceActive();
void advanceActivePulse();
void advanceIdle();
void advanceIdlePulse();
bool debounce(int pin);
bool debounce2(int pin);

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
				if (debounce2(KEY_PIN)) {
						currentState = ready;
				}
		}
		else {
				if (!debounce2(KEY_PIN)) {
						currentState = idle;
				}
		}

		/*Handle states, check for button presses to start pulses*/
		if (currentState == ready) {
				advanceReady();
				/*Check for button press, transition to active*/
				if (debounce2(BUT_PIN)) {
						currentState = active;
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

void advanceReady() {
		/*Pick random LED not near edges*/
		const int SPREAD_MAX = 10;
		int spread = random(SPREAD_MAX);
		int center = random(LED_COUNT - 2 * SPREAD_MAX) + SPREAD_MAX;
		unsigned char r = random(20);
		unsigned char g = r;
		unsigned char b = random(255);
		color[center] = rgb_color{ r, g, b };
		while (spread > 0) {
				color[center - spread] = color[center];
				color[center + spread] = color[center];
				spread--;
		}
		float mult = .2;
		// Propagate colors in both directions
		for (int i = 1; i < LED_COUNT; i++) {
				color[i].red = mult * (color[i].red + color[i - 1].red + color[i + 1].red);
				color[i].green = 0.8 * mult * (color[i].red + color[i - 1].green + color[i + 1].green);
				color[i].blue = 0.5 * mult * (color[i].green + color[i - 1].blue + color[i + 1].blue);
		}

		ledStrip.write(color, LED_COUNT);
}

void advanceActive() {
		unsigned int rMax = random(255);
		unsigned int gMax = random(1);
		unsigned int bMax = random(1);
		//color[LED_COUNT / 2 - 1] = rgb_color{ random(rMax), random(gMax), random(bMax) };
		//color[LED_COUNT / 2] = rgb_color{ random(rMax), random(gMax), random(bMax) };
		//color[LED_COUNT / 2 - 1] = rgb_color{ rMax, gMax, bMax };
		//color[LED_COUNT / 2] = rgb_color{ rMax, gMax, bMax };
		int omega = 2;
		/*downstream (right)*/
		color[LED_COUNT / 2] = rgb_color{ 120 + 120 * sin(theta_r), gMax, 120 + 120 * sin(theta_b + 0.2) };
		/*closest to arduino (left)*/
		color[LED_COUNT / 2 - 1] = rgb_color{ 0 + 64 * sin(theta_r), gMax, 0 + 64 * sin(theta_b) };

		//color[LED_COUNT / 2 - 1] = rgb_color{ rMax, gMax, bMax };
		//gummy worm
		//color[LED_COUNT / 2] = rgb_color{128 + 128 * sin(theta_r), 128 + 128 * sin(theta_g),  bMax};
		//robocop
		//color[LED_COUNT / 2] = rgb_color{128 + 128 * sin(theta_r), gMax, 128 + 128 * sin(theta_b)};

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

		// Propagate color from the center outwards
		for (int i = 1; i < LED_COUNT / 2 - 1; i++) {
				color[i] = color[i + 1];
		}
		for (int i = LED_COUNT; i > LED_COUNT / 2; i--) {
				color[i] = color[i - 1];
		}


		//for (int i = 0; i < LED_COUNT; i++) {
		//		color[i] = rgb_color{ 100, 0, 0 };
		//}

		// Write out color					
		ledStrip.write(color, LED_COUNT);
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
		color[center] = rgb_color{ r, g, b };
		while (spread > 0) {
				color[center - spread] = color[center];
				color[center + spread] = color[center];
				spread--;
		}
		float mult = .3;
		// Propagate colors in both directions
		for (int i = 1; i < LED_COUNT; i++) {
				color[i].red = mult * (color[i].red + color[i - 1].red + color[i + 1].red) + idlePulseTimer / 2;
				color[i].green = 0.8 * mult * (color[i].red + color[i - 1].green + color[i + 1].green);
				color[i].blue = 0.5 * mult * (color[i].green + color[i - 1].blue + color[i + 1].blue);

				/*Check bounds*/
				if (color[i].red > 255)
						color[i].red = 255;
				if (color[i].green > 255)
						color[i].green = 255;
				if (color[i].blue > 255)
						color[i].blue = 255;
				if (color[i].red < 0)
						color[i].red = 0;
				if (color[i].green < 0)
						color[i].green = 0;
				if (color[i].blue < 0)
						color[i].blue = 0;
		}

		if (idlePulseTimer > 0) {
				/*Pulse just starting*/
				if (idlePulseTimer == idlePulseDuration) {
						//for (int i = 1; i < LED_COUNT; i++) {
						//		/*Reset pulse colors*/
						//		colorPulse[i] = { 0, 0, 0 };
						//}
				}				
				idlePulseTimer--;
		}


		ledStrip.write(color, LED_COUNT);
}

void advanceIdlePulse() {

}
