/*--------------------------------------------------------------------------------------

 dmd_clock_readout.cpp
   Demo and example project for the Freetronics DMD, a 512 LED matrix display
   panel arranged in a 32 x 16 layout.

This LIibrary (DMD32) and example are  fork of original DMD library  was
modified to work on ESP32 Modified by: Khudhur Alfarhan  // Qudoren@gmail.com
1/Oct./2020

 See http://www.freetronics.com/dmd for resources and a getting started guide.

 Note that the DMD32 library uses the VSPI port for the fastest, low overhead
writing to the display. Keep an eye on conflicts if there are any other devices
running from the same SPI port, and that the chip select on those devices is
correctly set to be inactive when the DMD is being written to.

 USAGE NOTES
 -----------
 - Place the DMD library folder into the "arduino/libraries/" folder of your
Arduino installation.
 - Restart the IDE.
 - In the Arduino IDE, go to Tools > Board > and choose any ESP32 board
 - In the Arduino IDE, you can open File > Examples > DMD > dmd_demo, or
dmd_clock_readout, and get it running straight away!

 * See the documentation on Github or attached images to find the pins that
should be connected to the DMD LED display


 This example code is in the public domain.
 The DMD32 library is open source (GPL), for more see DMD32.cpp and DMD32.h

--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------
  Includes
--------------------------------------------------------------------------------------*/

#include "fonts/Arial_black_16.h"
#include <DMD32.h> //

// Fire up the DMD library as dmd
DMD dmd(1, 1);

// Timer setup for ESP32 Core v3.x and later
hw_timer_t *timer = NULL;

/*--------------------------------------------------------------------------------------
  Interrupt Service Routine (ISR) to scan the display
--------------------------------------------------------------------------------------*/
void IRAM_ATTR triggerScan() { dmd.scanDisplayBySPI(); }

/*--------------------------------------------------------------------------------------
  Show clock numerals on the screen from a 4 digit time value, and select
whether the flashing colon is on or off
--------------------------------------------------------------------------------------*/
void ShowClockNumbers(unsigned int uiTime, byte bColonOn) {
  dmd.clearScreen(true);
  dmd.drawChar(1, 3, '0' + ((uiTime % 10000) / 1000),
               GRAPHICS_NORMAL); // thousands
  dmd.drawChar(8, 3, '0' + ((uiTime % 1000) / 100),
               GRAPHICS_NORMAL);                                     // hundreds
  dmd.drawChar(17, 3, '0' + ((uiTime % 100) / 10), GRAPHICS_NORMAL); // tens
  dmd.drawChar(25, 3, '0' + (uiTime % 10), GRAPHICS_NORMAL);         // units
  if (bColonOn)
    dmd.drawChar(15, 3, ':', GRAPHICS_OR); // clock colon overlay on
  else
    dmd.drawChar(15, 3, ':', GRAPHICS_NOR); // clock colon overlay off
}

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
  dmd.selectFont(Arial_Black_16);
}

/*--------------------------------------------------------------------------------------
  loop
  Arduino architecture main loop
--------------------------------------------------------------------------------------*/
void loop(void) {
  unsigned int ui;

  // 10 x 14 font clock, including demo of OR and NOR modes for pixels so that
  // the flashing colon can be overlayed
  ui = 1234;
  ShowClockNumbers(ui, true);
  delay(1000);
  ShowClockNumbers(ui, false);
  delay(1000);
  ShowClockNumbers(ui, true);
  delay(1000);
  ShowClockNumbers(ui, false);
  delay(1000);

  ui = 2345;
  ShowClockNumbers(ui, true);
  delay(1000);
  ShowClockNumbers(ui, false);
  delay(1000);
  ShowClockNumbers(ui, true);
  delay(1000);
  ShowClockNumbers(ui, false);
  delay(1000);
}
