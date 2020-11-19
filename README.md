# MTP_t4

MTP Responder for Teensy 3.x and 4.x

Uses SD interface and Bill Greiman's SdFat-beta as distributed via Teenyduino supporting exFAT and SDIO

code is based on https://github.com/yoonghm/MTP with modification by WMXZ

see also https://forum.pjrc.com/threads/43050-MTP-Responder-Contribution for discussions

files in different copy-to directories contain modifications of cores and need to be copied to cores/teensy4, cores/teensy3 and hardware/avr, respectively. These files are only necessary until Teensyduino has integrated full MTP into cores functionality

needs USB2 https://github.com/WMXZ-EU/USB2 for T4.x. (uses here usb1.h and usb1.c)


## Features
Supports multiple storages (SDIO, multiple SPI disks, LittleFS_xxx disks)

copying files from Teensy to PC  and from PC to Teensy is working

disk I/O is buffered to get some speed-up overcoming uSD latency issues

both Serialemu and true Serial may be used- True Serial port is, however, showing up as Everything in Com port. This is a workaround to get Serial working.

Tested on T4.1 up to 450 MHz CPU speed

## Reset of Session
Modification of disk content (directories and Files) by Teensy is only be visible on PC when done before mounting the MTP device. To refresh disk content it is necessary to unmount and remount Teensy MTP device. On Windows this can be done by using device manager and disable and reanable Teensy (found under portable Device). On Linux this is done with standard muount/unmount commands.

## Examples
 - mtp-test:   basic MTP test program
 - mtp-logger: basic data logger with MTP access
 
## Installation:
 - If you wanted to use USB_MTP_SERIAL  
   - T4.x edit teensy/avr/cores/teensy4/usb_desc.h with content of 'modifications_for_cores_teensy4' (insert after MTP_DISK)
   - T3.x edit teensy/avr/cores/teensy3/usb_desc.h with content of 'modifications_for_cores_teensy3' (insert after MTP_DISK)
   - edit teensy/avr/boards.txt with content of 'modifications_for_teensy_avr' (copy to end of file)
 - install also USB2 from WMXZ github
 - install LittleFS from https://github.com/PaulStoffregen/LittleFS 

 ## Known Issues
  - move of multiple files cross different storages will block MTP 
  - moving files within storage should work
  - moving single files cross different storages should work (if files can be opened and read)
  - only LittlteFS_RAM implemented and tested
  
 ## ToBeDone
 - copy files 

