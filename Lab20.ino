#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "LIS3DHTR.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Accelerometer
LIS3DHTR<TwoWire> LIS;

// Struct for Answer
struct Answer {
  const char* text;
  int confidence;  // Confidence score (higher = more confident)
};

// Categories of answers
Answer positiveAnswers[] = {
  {"For Sure", 10},
  {"Definitely!", 9},
  {"Yes", 8},
  {"Go for it!", 10},
  {"You can do it!", 9}
};

Answer neutralAnswers[] = {
  {"Maybe", 5},
  {"Not Sure...", 4},
  {"Ask Later", 6},
  {"Time will tell", 5},
  {"Who knows?", 6}
};

Answer negativeAnswers[] = {
  {"No Way!", 1},
  {"No", 2},
  {"Nope", 3},
  {"I wouldn't bet on it", 2},
  {"Not likely", 3}
};

Answer humorousAnswers[] = {
  {"Ask your cat", 5},
  {"Outlook fuzzy, try again", 4},
  {"In another life...", 3},
  {"My magic ball needs charging", 2},
  {"Better ask your fortune cookies", 3}
};

Answer inspirationalAnswers[] = {
  {"Believe in yourself", 10},
  {"The sky's the limit", 9},
  {"You got this", 10},
  {"Dream big", 9},
  {"Your future is bright", 10}
};

float previousX = 0, previousY = 0, previousZ = 0; // Store last values
float threshold = 0.5; // Lower movement threshold for more sensitive shake
unsigned long shakeStartTime = 0; // Timer for shake duration
unsigned long shakeThreshold = 1000; // 1 second shake time for strong shake

void setup() {
  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Display init failed"));
    while (true);  // Stop if display fails
  }
  LIS.begin(Wire, 0x19);  // Initialize accelerometer
  display.clearDisplay();  // Clear screen
  pinMode(LED_BUILTIN, OUTPUT); // LED for feedback
}

void loop() {
  display.setTextSize(2);      // Set larger text size for readability
  display.setTextColor(SSD1306_WHITE); // Set text color (white)
  display.setCursor(0, 20);    // Set cursor position

  // Get accelerometer values
  float x = LIS.getAccelerationX();
  float y = LIS.getAccelerationY();
  float z = LIS.getAccelerationZ();

  // Print accelerometer values for debugging
  Serial.print("X: "); Serial.print(x);
  Serial.print(" Y: "); Serial.print(y);
  Serial.print(" Z: "); Serial.println(z);

  // Detect if board is shaken (movement in X, Y, or Z axis)
  if (abs(x - previousX) > threshold || abs(y - previousY) > threshold || abs(z - previousZ) > threshold) {
    // Record shake start time if it is the first shake
    if (shakeStartTime == 0) {
      shakeStartTime = millis();  // Start timer for shake
    }

    int randomIndex = 0;

    // Determine shake strength (weak or strong shake)
    if (millis() - shakeStartTime < shakeThreshold) {
      // Weak shake (less than 1 second)
      randomIndex = random(0, sizeof(neutralAnswers) / sizeof(neutralAnswers[0]));
      displayAnswer(neutralAnswers[randomIndex]);
    } else {
      // Strong shake (more than 1 second)
      randomIndex = random(0, sizeof(positiveAnswers) / sizeof(positiveAnswers[0]));
      displayAnswer(positiveAnswers[randomIndex]);
    }

    // Flash the LED 3 times
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED_BUILTIN, HIGH);  // Turn on LED
      delay(500);                       // Wait for 500ms
      digitalWrite(LED_BUILTIN, LOW);   // Turn off LED
      delay(500);                       // Wait for 500ms
    }
    
    delay(2000);  // Wait 2 seconds
    display.clearDisplay();  // Clear screen after a message is shown
    shakeStartTime = 0;  // Reset shake timer
  }

  // Save current accelerometer values for next loop
  previousX = x;
  previousY = y;
  previousZ = z;

  delay(50);  // Short delay before checking again
}

// Display answer on the OLED screen
void displayAnswer(Answer ans) {
  display.clearDisplay();  // Clear screen
  display.setCursor(0, 20);
  display.print(ans.text);  // Display answer text
  display.display();  // Update OLED display
}








