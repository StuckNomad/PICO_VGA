**Welcome to PICO VGA!**
***
This is a bare-bones VGA library for RP2040 MCU.

**Details**
  1. Resolution 640x480.
  2. 2 bits per RGB channel, 64 total possible colours.
  3. Stores 5 pixels in one 32 bit integer. 245kb of RAM is used to store the image.
  4. Posssibility to use all 6 bits/pixel to display up-to 64 shades of grey with a modified R2R DAC.

**Helper Scripts**
  |Script|Description|
  |------|-----------|
  |image_formatter.py|Compresses and converts input images into arrays, stores them into pixel_data.h ready for display.|
  |bit_helper.py|Use this script to encode Active, Front Porch, Sync pulse and back porch delays into a 32 bit integer. This encoded data can be used to modify VSYNC and HSYNC timings in VGA.h file.|
***
**Get Started**
- Make the connections as described below. You can use the first one (to display colour) to get started.
- Make sure you connect all the 5 grounds of VGA cable properly. 
- Compile the build and flash .uf2 file. Once the display is turned on, you should see my Cat!
- If you want to change the image, run image_formatter.py on the image you want to display (edit line 70), re-compile and re-flash. 

***
**Circuit Diagrams**

For Colour:

![colour](https://github.com/user-attachments/assets/099d034e-4036-4e2f-a5af-6022b92bd5de)

For BW:

![bw](https://github.com/user-attachments/assets/fc036431-00d5-4ef7-bfc5-32a597be5d0e)
