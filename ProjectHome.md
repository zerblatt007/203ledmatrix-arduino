# What is the reason for this project #
DealExtreme in China sells some cheap LED scrolling displays that uses button batteries. It works fine by itself for what it is designed to do, but has a "USB" cable connector that does not seem to exist. As it uses a Atmel ATmega88PA it is programmable if you have a simple standard ISP programmer. The display itself has almost no other components than the ATmega, the LEDs, an EEprom, buttons and a few resistors.
This project uses the Arduino IDE and libraries, as to make it easier to hack for n00bs like me.. :)
  * The display I used: http://www.dealextreme.com/p/programmable-scrolling-led-name-message-advertising-tag-badge-red-light-1-cr2032-55066
  * Is seems to be almost the same as this one: http://www.dealextreme.com/p/tsr-mini-programmable-scrolling-led-name-message-advertising-tag-badge-red-light-1-x-cr2032-81073
  * As I see it, this display can be hacked for a number of things:
    * A Clock
    * As an external display for some project
    * With extra components: The sky is the limit.. :)

## How to upload this code to the display ##
  * Two Arduino IDE files must be altered to upload to programmer/display
  * avrdude must be upgraded to support atmega88pa (5.11) as the included avrdude with Arduino-022 are too old.
  * You have to solder on a compatible cable for your programmer
  * Upload through a ISP programmer (stk500/Dragon).
    * There are right and wrong ways to do this. I used weeks trying to upload with power from the ISP (have to use 3v, not 5v) and too high frequency on the ISP bus.

## Acknowledge ##
  * Big portion of code borrowed from https://github.com/hugokernel/203LedMatrix made by Charles Rincheval (Hugo)
  * Hugos LED Matrix hack: http://translate.google.com/translate?hl=en&sl=auto&tl=en&u=http%3A%2F%2Fwww.digitalspirit.org%2Fblog%2Findex.php%2Fpost%2F2011%2F05%2F18%2FHacking-d-un-%25C3%25A9cran-%25C3%25A0-Led-de-DealExtreme
  * This page is originally in french, but Google Translate make a good enough job that it is understandable.
  * The display uses charlieplexing and the tables made by Hugo is complex. I would probably not be able to hack this without his 203LedMatrix work.