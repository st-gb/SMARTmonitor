/*
 * SMART.cpp
 *
 *  Created on: 11.03.2016
 *      Author: sg
 * compile: "g++ SMART_test.cpp -latasmart -o SMART_test.elf"
 */

#include <stddef.h> //size_t in atasmart.h
#include <atasmart.h>
#include <iostream> //std::cout
#include <errno.h>
#include <string.h> //strerror(...)
#include "SkDisk.h" //struct SkDisk
#include <libATA_SMART/SMARTaccess.hpp> // libatasmart::readAttribute
#include <SMARTvalueFormatter.hpp>

//void ReadSMARTValuesForAllDrives();

void outputPowerOnHours(const uint64_t & milliSeconds)
{
  const float seconds = (float) milliSeconds/1000.0f;
  const float hours = seconds/3600.0f;
  const float days = hours/24.0f;
  std::cout << "successfully got power on hours:"
    << milliSeconds << "ms="
    << seconds << "s="
    << hours << "h="
    << days << "d"
    << std::endl;
}

void ReadSMARTValues(/*uint8_t id*/ const char device [])
{
  SkDisk skDisk;
  SkDisk * p_skDisk;
//  sk_disk_check_power_mode();
  SkBool SMARTavailable;

  sk_disk_smart_is_available(& skDisk, & SMARTavailable);
//  sk_disk_smart_is_enabled(& skDisk);
  int i = sk_disk_open(device, /* SkDisk **_d*/ & p_skDisk);
  std::cout << "result of sk_disk_open for " <<  device << ":" << i << std::endl;
  if( i != -1)
  {
//    i = sk_init_smart( & skDisk);
    i = sk_disk_smart_read_data(p_skDisk);
    if( i == 0)
    {
      SkBool available;
      i = sk_disk_smart_is_available(p_skDisk, & available);
      if( i == 0)
        std::cout << "successfully got SMART avail.:" << available << std::endl;

      i = sk_disk_identify_is_available(p_skDisk, & available);
      if(i == 0 && available)
      {
        std::cout << "disk_identify_is_available:" << std::endl;
        const SkIdentifyParsedData * p_SkIdentifyParsedData;
        i = sk_disk_identify_parse(p_skDisk, & p_SkIdentifyParsedData);
        if(i == 0)
          std::cout << "disk identify:"
            << "model:" << p_SkIdentifyParsedData->model
            << " serial #:" << p_SkIdentifyParsedData->serial
            << " firmware:" << p_SkIdentifyParsedData->firmware << std::endl;
        else
          std::cout << "FAILED to get disk identify (mode,ser.#,firmware)" << std::endl;
      }
      uint64_t milliSeconds;
      i = sk_disk_smart_get_power_on(p_skDisk, & milliSeconds);
      if( i == 0)
        SMARTvalueFormatter::OutputPowerOnTimeAssumingMilliS(milliSeconds);

      uint64_t sectors;
      i = sk_disk_smart_get_bad(p_skDisk, & sectors);
      if( i == 0)
        std::cout << "successfully got # bad sectors:" << sectors << std::endl;
  //      i = sk_disk_smart_status(SkDisk *d, SkBool *good);
      uint64_t mkelvin;
      i = sk_disk_smart_get_temperature(p_skDisk,  & mkelvin);
      if( i == 0)
        std::cout << "successfully got temperature of drive:"
          << mkelvin << "mK=" << mkelvin/1000 << "K="
          << SMARTvalueFormatter::OutputTemperatureInCelsius(mkelvin) << std::endl;
      else
        std::cout << "failed to get temperature of drive:" << i << " errno:" << errno << std::endl;
//    const SkSmartAttributeInfo * p = lookup_attribute(& skDisk, id);
      uint64_t gSenseErrorRate;
      char * attributName = "udma-crc-error-count";
      i = libatasmart::readAttribute(attributName, p_skDisk, gSenseErrorRate);
      if( i == 0)
        std::cout << attributName << ":" << gSenseErrorRate << std::endl;
      else
        std::cout << "Failed to get attribute value for \"" << attributName << "\":"
          << strerror(errno) << std::endl;
    }
    else
      std::cout << "failed to read SMART data:" << i << " errno:" << errno
        << " " << strerror(errno) << std::endl;
    sk_disk_free( p_skDisk);
  }
  else
    std::cout << "failed to open disk for SMART:" << i << " error:" <<
    /*errno*/ strerror(errno) << std::endl;
}

int main(int argCount, char ** arguments)
{
  std::cout << "pass device as cmd line arg (else /dev/sda is chosen)" << std::endl;
  if( argCount > 1 )
    ReadSMARTValues(arguments[1]);
  else
    ReadSMARTValues("/dev/sda");
}
