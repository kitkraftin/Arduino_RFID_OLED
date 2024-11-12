#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Define RFID pins
#define RST_PIN 9    // Reset pin for RC522
#define SS_PIN 10    // SDA pin for RC522

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Create RFID object
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Authorized RFID card UID (replace this with your card's UID)
byte authorizedUID[4] = {0xE0, 0x6D, 0xAE, 0x10};  // Example UID (change it to your card's UID)

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  while (!Serial);
  
  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  // Initialize SPI and RFID
  SPI.begin();
  mfrc522.PCD_Init();
  
  // Welcome message
  display.setCursor(0, 10);
  display.setTextSize(2);
  display.print("Scan Your Card...");
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Get the UID of the card
  byte readUID[4];
  for (byte i = 0; i < 4; i++) {
    readUID[i] = mfrc522.uid.uidByte[i];
  }
  
  // Check if the scanned card is authorized
  if (checkUID(readUID)) {
    display.clearDisplay();
    display.setCursor(0, 20);
    display.setTextSize(2);
    display.print("Authorized");
    display.display();
    Serial.println("Access Granted");
  } else {
    display.clearDisplay();
    display.setCursor(0, 20);
    display.setTextSize(2);
    display.print("Not Authorized");
    display.display();
    Serial.println("Access Denied");
  }
  
  // Halt PICC
  mfrc522.PICC_HaltA();
  delay(3000);  // Wait for 3 seconds before clearing the screen
  display.clearDisplay();
  display.setCursor(0, 10);
  display.setTextSize(2);
  display.print("Scan Your Card...");
  display.display();
}

// Function to check if UID matches the authorized UID
bool checkUID(byte uid[]) {
  for (byte i = 0; i < 4; i++) {
    if (uid[i] != authorizedUID[i]) {
      return false;
    }
  }
  return true;
}
