#include "Arduino.h"
#include "SD.h"

#include "MTP.h"
#include "usb1_mtp.h"

#define HAVE_LITTLEFS 1 // set to zero if no LtttleFS is existing or to be used

#if HAVE_LITTLEFS==1        // is defined in storage.h
  #define DO_LITTLEFS 1     // set to zero if not wanted // needs LittleFS installed as library
  #define RAM_DISK_STORAGE (8'000'000)

  #include "LittleFS.h"
#else
  #define DO_LITTLEFS 0
#endif


/****  Start device specific change area  ****/

  // edit SPI to reflect your configuration (following is fot T4.1)
  #define SD_MOSI 11
  #define SD_MISO 12
  #define SD_SCK  13

  #define SPI_SPEED SD_SCK_MHZ(16)  // adjust to sd card 

#if DO_LITTLEFS==1
  const char *sd_str[]={"sdio","RAM"};     // edit to reflect your configuration
  const int cs[] = {BUILTIN_SDCARD, 256};  // edit to reflect your configuration
#else
  const char *sd_str[]={"sdio","sd6"}; // edit to reflect your configuration
  const int cs[] = {BUILTIN_SDCARD,38}; // edit to reflect your configuration
#endif
  const int nsd = sizeof(cs)/sizeof(int);

// classes need to be declared here (in storage.h there are declared external)
SDClass sdx[nsd];
#if HAVE_LITTLEFS==1
  LittleFS_RAM ramfs; // needs to be declared if LittleFS is used in storage.h
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
        storage.addFilesystem(sdx[ii],cs[ii],sd_str[ii]);
      }
      else if(cs[ii]<BUILTIN_SDCARD)
      {
        pinMode(cs[ii],OUTPUT); digitalWriteFast(cs[ii],HIGH);
        if(!sdx[ii].sdfs.begin(SdSpiConfig(cs[ii], SHARED_SPI, SPI_SPEED))) {Serial.println("No storage"); while(1);}
        storage.addFilesystem(sdx[ii],cs[ii],sd_str[ii]);
      }
      #if DO_LITTLEFS==1
        else if(cs[ii]==256) // LittleFS_RAM
        { if(!ramfs.begin(RAM_DISK_STORAGE)) { Serial.println("No storage"); while(1);}
          storage.addFilesystem(ramfs,cs[ii],sd_str[ii]);
        }
      #endif
      if(cs[ii]<256)
      {
        uint64_t totalSize = sdx[ii].totalSize();
        uint64_t usedSize  = sdx[ii].usedSize();
        Serial.printf("Storage %d %d %s ",ii,cs[ii],sd_str[ii]); Serial.print(totalSize); Serial.print(" "); Serial.println(usedSize);
      }
      #if DO_LITTLEFS==1
        else if(cs[ii]==256) // LittleFS_RAM
        {
        uint64_t totalSize = ramfs.totalSize();
        uint64_t usedSize  = ramfs.usedSize();
        Serial.printf("Storage %d %d %s ",ii,cs[ii],sd_str[ii]); Serial.print(totalSize); Serial.print(" "); Serial.println(usedSize);
        }
      #endif
    }
}
/****  End of device specific change area  ****/

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
  while(!Serial && millis()<3000); 
  Serial.println("MTP_test");
  
  usb_mtp_configure();
  storage_configure();

  #if DO_LITTLEFS==1
  // store some files into disks (but only once)
  for(int ii=0; ii<10;ii++)
  { char filename[80];
    sprintf(filename,"test_%d.txt",ii);
    if(!ramfs.exists(filename))
    {
      File file=ramfs.open(filename,FILE_WRITE_BEGIN);
      file.println("This is a test line");
      file.close();
    }
  }
  #endif

  Serial.println("Setup done");
  Serial.flush();
}

void loop()
{ 
  mtpd.loop();

  //logg(1000,"loop");
  //asm("wfi"); // may wait forever on T4.x
}
