uLC - microcontroller Live Coding
=============

Live coding of algorithmic music on an AVR.
Based on the work of VIznut: http://countercomplex.blogspot.jp/2011/10/algorithmic-symphonies-from-one-line-of.html.

This version makes use of at least two microcontroller (connected serially):
- One for interface (keyboard, LCD): arduino MEGA 1280.
- At least one other microcontroller (ATTINY85) for computation and sound generation.
- Support for multiple ATTINY.

The library ATSerial is identical to SoftwareSerial with the unused baud rates (all except 2400) commented out.
