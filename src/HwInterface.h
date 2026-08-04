#define TOGGLE_LED_PIN D0

// D1 = SDL for OLED display
// D2 = SDA for OLED display

// D3 = WS2812 data line (LEDS need 5V, signal only 3.3V works)

// D4 is reserved for build In led -> show http activity

//KEY1 moved to D5 to avoid fast overload for IR detection
//KEY3 moved to D7 to used for IR detection
#define KEY1_PIN D5
#define KEY2_PIN D6
#define KEY3_PIN D7

//GPIO15/D8 for flash & boot don't pull high
#define LDR_PIN  D8
#define IR_PRESENCE_PIN  D7

#define NOISE_PIN A0

void hwSetup(void);
void hwLoop(void);

bool hwSecoundTick(void);
bool hwCentiSecoundTick(void);
unsigned long hwGetMillis(void);

//bool keyPressed(int keyNumber);
int keyPressedCounter(int keyNumber);

void pollKeyPressed(void);
