/*--------------------------------------------------------------------------------------

 dmd_latin_chars

 This LIibrary (DMD32) and example are  fork of original DMD library  was
modified to work on ESP32 Modified by: Khudhur Alfarhan  // Qudoren@gmail.com
1/Oct./2020


   Demo and example Latin-1 encoding project for the Freetronics DMD, a 512 LED
matrix display panel arranged in a 32 x 16 layout.

See http://www.freetronics.com/dmd for resources and a getting started guide.

This example code is in the public domain. It demonstrates using the ISO-8859-1
(Latin-1) extended character set. Thanks to Xavier Seignard for contributing
Latin-1 support.

*******************************
HOW TO ENTER Latin-1 CHARACTERS
*******************************

Unfortunately entering Latin-1 characters like Ã or è is not as simple as just
typing them. Arduino Sketches are saved in Unicode UTF-8 format, but the DMD
library does not understand Unicode (it's too complex.)

To enter the characters as Latin-1, look at the codepage layout here and look
for the hexadecimal digit that corresponds to the character you want.
https://en.wikipedia.org/wiki/ISO/IEC_8859-1#Codepage_layout

For example, á (lowercase a, rising diacritical mark) has hex value 00E0 in
Latin-1. To translate this into a constant string, replace the leading 00 with
\x - so the string could be "The Portugese for rapid is r\xE0pido".

To be safe, the string may also need to be separated in its own quote marks - ie
"The Portugese for rapid is r""\xE0""pido"

When you compile the sketch, the compiler will join all these strings up into
one long string - however without the quotes around the \x it may try to include
additional characters as part of the hexadecimal sequence.

/*--------------------------------------------------------------------------------------
  Includes
--------------------------------------------------------------------------------------*/

#include "fonts/Arial_Black_16_ISO_8859_1.h"
#include "fonts/SystemFont5x7.h"
#include <DMD32.h>

// Fire up the DMD library as dmd
#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN 1
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

// Timer setup for ESP32 Core v3.x and later
hw_timer_t *timer = NULL;

/*--------------------------------------------------------------------------------------
  Interrupt Service Routine (ISR) to scan the display
--------------------------------------------------------------------------------------*/
void IRAM_ATTR triggerScan() { dmd.scanDisplayBySPI(); }

/*--------------------------------------------------------------------------------------
  setup
  Called by the Arduino architecture before the main loop begins
--------------------------------------------------------------------------------------*/
void setup(void) {
  // Configure the timer. The ESP32 has 4 hardware timers.
  // timerBegin takes the timer frequency in Hz. Let's aim for a high refresh
  // rate. A frequency of 40000 Hz gives a 25us period.
  timer = timerBegin(40000); // Returns a pointer to the timer object on success

  if (timer) {
    // Attach the ISR function to the timer
    timerAttachInterrupt(timer, &triggerScan);

    // Set the alarm period. The value is in timer ticks.
    // With a 40000 Hz frequency, 12 ticks gives an alarm every 12/40000 s = 300
    // us. The third boolean parameter 'true' makes it autoreload, and 0 means
    // unlimited reloads (periodic alarm).
    timerAlarm(timer, 12, true, 0);
  }

  // clear/init the DMD pixels held in RAM
  dmd.clearScreen(true); // true is normal (all pixels off), false is negative
                         // (all pixels on)
  Serial.begin(115200);
}

/*--------------------------------------------------------------------------------------
  loop
  Arduino architecture main loop
--------------------------------------------------------------------------------------*/
void loop(void) {
  dmd.clearScreen(true);
  dmd.selectFont(Arial_Black_16_ISO_8859_1);
  // Français, Österreich, Magyarország
  const char *MSG = "Fran"
                    "\xe7"
                    "ais, "
                    "\xd6"
                    "sterreich, Magyarorsz"
                    "\xe1"
                    "g";
  dmd.drawMarquee(MSG, strlen(MSG), (32 * DISPLAYS_ACROSS) - 1, 0);
  long start = millis();
  long timer = start;
  while (1) {
    if ((timer + 30) < millis()) {
      dmd.stepMarquee(-1, 0);
      timer = millis();
    }
  }
}
