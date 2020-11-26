 

## Arduino Assetto Corsa Dashboard

  3 dashboards for Assetto Corsa and Assetto Corsa Competizione

  - With LCDKeypad (found on a chinese seller website)
  - With TFT 2.4" Shield (https://www.amazon.fr/gp/product/B0798N3JWD/ref=ppx_yo_dt_b_asin_title_o05_s00?ie=UTF8&psc=1)
  - With TM1638 display (https://www.amazon.fr/gp/product/B0797PLVTX/ref=ppx_yo_dt_b_asin_title_o09_s00?ie=UTF8&psc=1)
  - With TFT 3.2" OpenSmart Shield (only on Assetto Corsa Competizione)

  2 parts :

  1 server written in C# run in PC where running Assetto Corsa
  1 arduino wired on USB on the PC communicate with server within serial port 

  Display Speed,Gear,Fuel , Fuel estimated autonomy after 2 laps, best lap , last lap ( also performance meter in tmp1638)
  For ACC no Tyre Wear and Tyre Temp

## Installation

- Clone this repo on your server or download release
- Copy Keypad folder in Arduino/Libraries in your Arduino libraries folder in your computer (only for LCDKeypad dashboard)
- Copy MCUFRIEND_kbv folder in Arduino/Libraries in your Arduino libraries folder in your computer (only for TFT 3.2" dashboard)
- Upload specific .ino file in your Arduino (tested with Uno R3)
- Launch AC
- Launch your server in Server\AC_Arduino\Bin (after compiling with vs2019 or vs2017 community edition)
- On lcd keypad with up / down button you change screen
- On Tft by touch you change screen
- On TM1638 by pushing button 1 to 6 change screen
 
## Video

[TM1638] (https://youtu.be/Oq1Vjo7tcMA)
[TFT] (https://youtu.be/T2sANoxOq0E)
[LCD Keypad] (https://youtu.be/X3urU8Btajs)

## Wiring

![TM1638 Wiring](https://www.kletellier.ovh/dashboard/tm1638_bb.png) 

The other dashboard work with shields

## Thanks

mdjarv for component for reading shared memory in AC (https://github.com/mdjarv/assettocorsasharedmemory)
 
## License

MIT License

Copyright (c) [2020] [Arduino Assetto Corsa Dashboards]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 
