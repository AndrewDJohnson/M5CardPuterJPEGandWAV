/**
Created by modifying M5 Stack examples.

Andrew Johnson - 24 Aug 2024

Simple and daft thing which can be adapted for more sensible use.

Reads keypresses from Cardputer keyboard and then loads 2 different JPEGS and displays and then plays various WAV files.
Assumes the 2 jpegs and WAV files are stored in the "/StarTrek/" folder of the CardPuter's SD card/

The order of "include" files here is important - the SD.H must come before the  "M5CardPuter.h" or the JPEG loading function won't work

There seems to be a delay after the jpeg loads or before the sound is played... Not sure what causes that!

 */

#include <Arduino.h>
#include "SD.h"
#include "SPI.h"
#include "M5Cardputer.h"
#include "FS.h"

//Buffer pointer - set to where the WAV file data is loaded.
char *buffer;

//Simple struct to hold a key/letter pressed and match it to a file name to be loaded!
struct key_and_filname 
{
   const char *key;
   const char *file_name;
};

//Initialise a "look up" table of keys and file names.
struct key_and_filname keys_and_files[]=
{
   {"a","ALERT.WAV"},
   {"b","BEAMUP.WAV"},
   {"c","BEEP1.WAV"},
   {"d","BEEP2.WAV"},
   {"s","BRDGSRN.WAV"},
   {"e","DISRUPTR.WAV"},
   {"o","DOOR.WAV"},
   {"h","Heart.wav"},
   {"m","MEDISCAN.WAV"},
   {"p","PHASBANK.WAV"},
   {"q","PHASER.WAV"},
   {"w","PHASER2.WAV"},
   {"t","PHOTON.WAV"},
   {"l","SHTLCRFT.WAV"},
   {"w","Warp1.wav"},
   {"f","WARP6.WAV"},
   {"z","WARPDEC.WAV"},
   {"i","Whistle.wav"},
   {"k","KIRKHERE.WAV"},
   //End with 0 so we know to stop!
   0
};
/***********************
/* From the sound-playing example - load a named wav file and play it! 
************************/
void loadWav(char *fileName)
{
    File file = SD.open(fileName);
    if (!file) 
    {
        Serial.println(fileName);
        Serial.println("Failed to open file for reading");
        return;
    }
 
    // Determine the size of the file
    size_t fileSize = file.size();
    if (fileSize > 0) 
    {
        // Allocate a buffer to hold the file contents
        // Note: Ensure there's enough memory to allocate the buffer
        buffer = new char[fileSize + 1];  // +1 for null terminator
 
        // Read the file into the buffer
        size_t bytesRead = file.readBytes(buffer, fileSize);
        Serial.printf("BytesRead: %ld\n", bytesRead);
        M5Cardputer.Speaker.playWav((const uint8_t*)buffer);
 
        // Remember to free the memory after use
        delete[] buffer;
    }
    file.close();
}
/**** Set things up before the main loop! ****/
void setup() 
{
    //Initialise a few things.
    auto cfg = M5.config();
    Serial.begin(115200);    
    M5Cardputer.begin(cfg, true);
    M5Cardputer.Speaker.begin();

    //Set up the display on the Cardputer 
    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.setTextColor(RED);
    M5Cardputer.Display.setTextDatum(middle_center);
    M5Cardputer.Display.setTextFont(&fonts::FreeSerif18pt7b);
    M5Cardputer.Display.setTextSize(1);
                                   
   //Check SD card etc 
   while (!SD.begin()){
        Serial.println("Card Mount Failed");
        //Display the error.
        M5Cardputer.Display.drawString("SD Card Error",
                                   M5Cardputer.Display.width() / 2,
                                   M5Cardputer.Display.height() / 2);
        
        M5Cardputer.Display.drawString("Please insert one?",
                                   M5Cardputer.Display.width() / 2,
                                   M5Cardputer.Display.height() / 2 + 30);
        delay (3000);
        M5Cardputer.Display.clear();
    }
    M5Cardputer.Display.drawJpgFile(SD, "/StarTrek/enterprise.jpg", 0, 0);
    delay (1500);

    M5Cardputer.Display.drawString("Press Any Key",
                                   M5Cardputer.Display.width() / 2,
                                   M5Cardputer.Display.height() / 2);
}

/* Main loop  - check keyboard and then act! */
void loop() {
  char key_pressed;
  //Set up pointer to key table etc
  struct key_and_filname *key_check;

    M5Cardputer.update();
    M5Cardputer.Display.setTextColor(GREEN);
    // Check if jkeys are pressed.
    if (M5Cardputer.Keyboard.isChange()) {
        if (M5Cardputer.Keyboard.isPressed()) 
        {
            //Find out which keys have been pressed.
            Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
            String keyStr                    = "";
            for (auto i : status.word) {
                if (keyStr != "") {
                    keyStr = keyStr + "+" + i;
                } else {
                    keyStr += i;
                }
                //Just get the first key pressed as a single character
                key_pressed = (char)*keyStr.c_str();
                //Set ready to use our lookup table.
                key_check = keys_and_files;

                //Clear display
                M5Cardputer.Display.clear();
                //Scan through table.
                while (key_check)
                {
                  //Now check if we need to load a corresponding WAV file.
                  if (key_pressed == *key_check->key)
                  {
                    //Have we pressed a particular key which causes us to load  a jpeg (choice of 2 at the moment)
                    switch (key_pressed)
                    {
                      case 'k':
                          M5Cardputer.Display.drawJpgFile(SD, "/StarTrek/kirk.jpg", 0, 0);
                          Serial.printf("Kirk loaded...\n");
                          break;
                      case 'q':
                      case 'w':
                      case 'e':
                          M5Cardputer.Display.drawJpgFile(SD, "/StarTrek/enterprise.jpg", 0, 0);
                          break;
                      case 't':
                          M5Cardputer.Display.drawJpgFile(SD, "/StarTrek/photontp.jpg", 0, 0);
                          break; 
                    }

                    String full_file_name;
                    full_file_name = "/StarTrek/" + String(key_check->file_name);
                    loadWav((char *)full_file_name.c_str());
                    break;
                  }
                  key_check++;
                }
            }
            //Display the key pressed
            M5Cardputer.Display.drawString(keyStr,
                                           M5Cardputer.Display.width() / 2,
                                           100);
        } 
        else // If not key has been pressed.
        {
            M5Cardputer.Display.drawString("Press Any Key",
                                           M5Cardputer.Display.width() / 2,
                                           M5Cardputer.Display.height() / 2);
        }
    }
}
