//------------------------------------
//SimpleMaker Arduino Technical Serial
//Written by Thomas Williams
//(C) Thomas Williams
//Beta 1 completed on 7/13/17
//------------------------------------

#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include <SoftwareSerial.h>
// Set up SD Card
const int chipSelect = 10;

// Set up Serial
#define rxPin 6
#define txPin 7
SoftwareSerial port =  SoftwareSerial(rxPin, txPin);
// Set up display
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

//Declare program vars
bool _run;
char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ-";
char pad[10];
char fin[10];
char filename[20];
int pos;
int filestemints[5] = {0, 0, 0, 0, 0};
char filestem[10];
int state = 0;
File file;
int lcdcounter = 16;
void setup() {

  //setup display
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("SMArTSerial");
  lcd.setCursor(0, 1);
  lcd.print(" (C) T. Williams");
  lcd.setBacklight(0x4); //Sets display color to blue


  // setup debug serial:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //setup serial for rs232 port
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  // _____ __  __ _____   ____  _____ _______       _   _ _______
  //|_   _|  \/  |  __ \ / __ \|  __ \__   __|/\   | \ | |__   __|
  //  | | | \  / | |__) | |  | | |__) | | |  /  \  |  \| |  | |
  //  | | | |\/| |  ___/| |  | |  _  /  | | / /\ \ | . ` |  | |
  // _| |_| |  | | |    | |__| | | \ \  | |/ ____ \| |\  |  | |
  //|_____|_|  |_|_|     \____/|_|  \_\ |_/_/    \_\_| \_|  |_|
  // THIS NUMBER CONTROLS THE RS232 SERIAL PORT BAUD! CHANGE WHEN NECESSARY!
  // VVV THAT ONE VVV
  port.begin(9600);  // <<< THIS ONE  <<<
  // ^^^ YOU MISSED IT ^^^

  //setup SD
  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("card failure");
    state = 13;
    return;
  }

  delay(2000);
  lcd.clear();
  pos = 0;

}

void loop() {
  if (state == 0) { // Start State; Collect filename
    lcd.setCursor(0, 0);
    lcd.print("Filename? <^V>");
    for (int i = 0; i < 5; i++) {
      filestem[i] = chars[filestemints[i]];
    }
    filestem[5] = '\0';
    lcd.setCursor(0, 1);
    lcd.print(filestem);

    uint8_t buttons = lcd.readButtons();

    if (buttons) {
      lcd.clear();
      lcd.setCursor(0, 0);
      if (buttons & BUTTON_UP) {
        filestemints[pos] += 1;
        if (filestemints[pos] == 27) {
          filestemints[pos] = 0;
        }
      }
      if (buttons & BUTTON_DOWN) {

        if (filestemints[pos] == 0) {
          filestemints[pos] = 26;
        } else {
          filestemints[pos] -= 1;
        }
      }
      if (buttons & BUTTON_LEFT) {
        if (pos == 0) {
          pos = 4;
        } else {
          pos -= 1;
        }
      }
      if (buttons & BUTTON_RIGHT) {
        pos += 1;
        if (pos == 5) {
          pos = 0;
        }
      }
      if (buttons & BUTTON_SELECT) {
        state = 1;
        lcd.clear();
      }
    }
  }
  if (state == 1) { //State determines the next filename to use
    strcpy(filename, filestem);
    strcat(filename, "000.csv");
    int i = 1;
    while (SD.exists(filename)) {
      strcpy(filename, filestem);
      sprintf(pad, "%03d", i);
      strcat(filename, pad);
      strcat(filename, ".csv");
      i++;
    }
    file = SD.open(filename, FILE_WRITE);
    state = 2;
  }
  if (state == 2) {

    char c = port.read();
    if (c != -1) {
      file.print(c);
      if (lcdcounter > 0) {
        lcd.print(c);
        lcdcounter--;
      } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(c);
        lcdcounter = 15;
      }
    }

    uint8_t buttons = lcd.readButtons();
    if (buttons) {
      if (buttons & BUTTON_SELECT) {
        state = 3; //Enter potential exit state
        lcd.clear();
      }
    }
  }
  if (state == 3) {
    lcd.setCursor(0, 0);
    lcd.print("Exit      Return");
    lcd.setCursor(0, 1);
    lcd.print("<<<          >>>");
    char c = port.read();
    if (c != -1) {
      file.print(c);
    }

    uint8_t buttons = lcd.readButtons();
    if (buttons) {
      if (buttons & BUTTON_RIGHT) {
        state = 2; //Exit potential exit state
        lcd.clear();
      }
      if (buttons & BUTTON_LEFT) {
        file.close();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("File saved.");
        state = 13; //Exit state
        return;

      }
    }
  }
}
