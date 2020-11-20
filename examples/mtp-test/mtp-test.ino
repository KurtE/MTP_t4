#include "Arduino.h"

#include "SD.h"
#include "MTP.h"
#include "usb1_mtp.h"

#define HAVE_LITTLEFS 1 // set to zero if no LtttleFS is existing or to be used

/****  Start device specific change area  ****/

  // edit SPI to reflect your configuration (following is for T4.1)
  #define SD_MOSI 11
  #define SD_MISO 12
  #define SD_SCK  13

  #define SPI_SPEED SD_SCK_MHZ(16)  // adjust to sd card 

// SDClasses
  const char *sd_str[]={"sdio","sd6"}; // edit to reflect your configuration
  const int cs[] = {BUILTIN_SDCARD,38}; // edit to reflect your configuration
  const int nsd = sizeof(cs)/sizeof(int);

SDClass sdx[nsd];

//LittleFS classes
#if HAVE_LITTLEFS==1
  #include "LittleFS.h"
  const char *lfs_str[]={"RAM1","RAM2"};     // edit to reflect your configuration
  const int lfs_size[] = {2'000'000,4'000'000};
  const int nfs = sizeof(lfs_size)/sizeof(int);

  LittleFS_RAM ramfs[nfs]; // needs to be declared if LittleFS is used in storage.h
#endif

MTPStorage_SD storage;
MTPD       mtpd(&storage);


void storage_configure()
{
    #if defined SD_SCK
      SPI.setMOSI(SD_MOSI);
      SPI.setMISO(SD_MISO);
      SPI.setSCK(SD_SCK);
    #endif

    for(int ii=0; ii<nsd; ii++)
    { if(cs[ii] == BUILTIN_SDCARD)
      {
        if(!sdx[ii].sdfs.begin(SdioConfig(FIFO_SDIO))) {Serial.println("No storage"); while(1);};
        storage.addFilesystem(sdx[ii], sd_str[ii]);
      }
      else if(cs[ii]<BUILTIN_SDCARD)
      {
        pinMode(cs[ii],OUTPUT); digitalWriteFast(cs[ii],HIGH);
        if(!sdx[ii].sdfs.begin(SdSpiConfig(cs[ii], SHARED_SPI, SPI_SPEED))) {Serial.println("No storage"); while(1);}
        storage.addFilesystem(sdx[ii], sd_str[ii]);
      }
        uint64_t totalSize = sdx[ii].totalSize();
        uint64_t usedSize  = sdx[ii].usedSize();
        Serial.printf("Storage %d %d %s ",ii,cs[ii],sd_str[ii]); Serial.print(totalSize); Serial.print(" "); Serial.println(usedSize);
    }
    #if HAVE_LITTLEFS==1
    for(int ii=0; ii<nfs;ii++)
    {
      { if(!ramfs[ii].begin(lfs_size[ii])) { Serial.println("No storage"); while(1);}
        storage.addFilesystem(ramfs[ii], lfs_str[ii]);
      }
      uint64_t totalSize = ramfs[ii].totalSize();
      uint64_t usedSize  = ramfs[ii].usedSize();
      Serial.printf("Storage %d %s ",ii,lfs_str[ii]); Serial.print(totalSize); Serial.print(" "); Serial.println(usedSize);

    }
    #endif
}
/****  End of device specific change area  ****/

  // Call back for file timestamps.  Only called for file create and sync(). needed by SDFat-beta
   #include "TimeLib.h"
  void dateTime(uint16_t* date, uint16_t* time, uint8_t* ms10) 
  { *date = FS_DATE(year(), month(), day());
    *time = FS_TIME(hour(), minute(), second());
    *ms10 = second() & 1 ? 100 : 0;
  }

void logg(uint32_t del, const char *txt)
{ static uint32_t to;
  if(millis()-to > del)
  {
    Serial.println(txt); 
    to=millis();
  }
}

void setup()
{ 
  while(!Serial); 
  Serial.println("MTP_test");
  
  // Set Time callback // needed for SDFat-beta
  FsDateTime::callback = dateTime;

  usb_mtp_configure();
  storage_configure();

  #if HAVE_LITTLEFS==1
  // store some files into disks (but only once)
  for(int ii=0; ii<10;ii++)
  { char filename[80];
    sprintf(filename,"test_%d.txt",ii);
    if(!ramfs[0].exists(filename))
    {
      File file=ramfs[0].open(filename,FILE_WRITE_BEGIN);
        file.println("This is a test line");
      file.close();
    }
  }
  #endif
  
  const char *str = "test1.txt";
  if(sdx[0].exists(str)) sdx[0].remove(str);
  File file=sdx[0].open(str,FILE_WRITE_BEGIN);
      file.println("This is a test line");
  file.close();

  Serial.println("\n**** dir of sd[0] ****");
  sdx[0].sdfs.ls();
  Serial.println();

  Serial.println("Setup done");
  Serial.flush();
}

void loop()
{ 
  mtpd.loop();

  //logg(1000,"loop");
  //asm("wfi"); // may wait forever on T4.x
}
