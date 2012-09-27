uLC - microcontroller Live Coding
=============

Live coding of algorithmic music on an AVR.
Based on the work of VIznut: http://countercomplex.blogspot.jp/2011/10/algorithmic-symphonies-from-one-line-of.html.

This version makes use of two microcontroller (connected serially):
- One for interface (keyboard, LCD): arduino MEGA 1280.
- Another microcontroller (ATTINY85) for computation and sound generation.

The library ATSerial is identical to SoftwareSerial with the unused baud rates (all except 2400) commented out.
