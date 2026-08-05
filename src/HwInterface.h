#define TOGGLE_LED_PIN D0

// D1 = SDL for OLED display
// D2 = SDA for OLED display


// D4 is reserved for build In led -> show http activity

#define KEY1_PIN D7
#define KEY2_PIN D8

#define FORCE_DOUT_PIN D6
#define FORCE_SCK_PIN D5


void hwSetup(void);
void hwLoop(void);

bool hwSecoundTick(void);
bool hwCentiSecoundTick(void);
unsigned long hwGetMillis(void);

//bool keyPressed(int keyNumber);
int keyPressedCounter(int keyNumber);

void pollKeyPressed(void);
