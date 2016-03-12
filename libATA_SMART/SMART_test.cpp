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

/** The following definitions are needed for compiling and from
 *  https://github.com/Rupan/libatasmart/blob/master/atasmart.c */
typedef enum SkDiskType {
  /* These three will be autotested for: */
  SK_DISK_TYPE_ATA_PASSTHROUGH_12, /* ATA passthrough over SCSI transport, 12-byte version */
  SK_DISK_TYPE_ATA_PASSTHROUGH_16, /* ATA passthrough over SCSI transport, 16-byte version */
  SK_DISK_TYPE_LINUX_IDE,          /* Classic Linux /dev/hda ioctls */

  /* These three will not be autotested for */
  SK_DISK_TYPE_SUNPLUS,            /* SunPlus USB/ATA bridges */
  SK_DISK_TYPE_JMICRON,            /* JMicron USB/ATA bridges */
  SK_DISK_TYPE_BLOB,               /* From a file */
  SK_DISK_TYPE_NONE,               /* No access method */
  SK_DISK_TYPE_AUTO,               /* We don't know yet */
  _SK_DISK_TYPE_MAX,
  _SK_DISK_TYPE_TEST_MAX = SK_DISK_TYPE_SUNPLUS /* only auto test until here */
} SkDiskType;

struct SkDisk {
  char *name;
  int fd;
  SkDiskType type;

  uint64_t size;

  uint8_t identify[512];
  uint8_t smart_data[512];
  uint8_t smart_thresholds[512];

  SkBool smart_initialized:1;

  SkBool identify_valid:1;
  SkBool smart_data_valid:1;
  SkBool smart_thresholds_valid:1;

  SkBool blob_smart_status:1;
  SkBool blob_smart_status_valid:1;

  SkBool attribute_verification_bad:1;

  SkIdentifyParsedData identify_parsed_data;
  SkSmartParsedData smart_parsed_data;

  /* cache for commonly used attributes */
  SkBool attribute_cache_valid:1;
  SkBool bad_attribute_now:1;
  SkBool bad_attribute_in_the_past:1;
  SkBool reallocated_sector_count_found:1;
  SkBool current_pending_sector_found:1;
  uint64_t reallocated_sector_count;
  uint64_t current_pending_sector;

  void *blob;
};

struct attr_helper {
  uint64_t *value;
  SkBool found;
};

std::string g_stdStrAttributeName;

static void any_attribute_callback(
  SkDisk *d,
  const SkSmartAttributeParsedData *a,
  struct attr_helper *ah)
{
//  std::cout << "a->name: " << a->name << " " << g_stdStrAttributeName.c_str() << std::endl;
//  if (a->pretty_unit != SK_SMART_ATTRIBUTE_UNIT_NONE)
//  {
//    std::cout << "a->pretty_unit != SK_SMART_ATTRIBUTE_UNIT_NONE" << std::endl;
//    return;
//  }

  if ( ! strcmp(a->name, g_stdStrAttributeName.c_str() ) ) {
//    std::cout << "a->name matches " << g_stdStrAttributeName.c_str() << std::endl;
    if (!ah->found || a->pretty_value > *ah->value)
            *ah->value = a->pretty_value;

    ah->found = TRUE;
  }
}

int readAttribute(const char attributeName [], SkDisk * p_skDisk, uint64_t & value)
{
  g_stdStrAttributeName = attributeName;
  struct attr_helper ah;

//   assert(d);
//   assert(kelvin);
   ah.found = FALSE;
   ah.value = & value;
   //TODO "sk_disk_smart_parse_attributes" traverses all attributes and calls
   // the callback function
  if(sk_disk_smart_parse_attributes(
       p_skDisk,
       (SkSmartAttributeParseCallback) any_attribute_callback,
       &ah) < 0)
   return -1;
  if (!ah.found) {
    errno = ENOENT;
    return -2;
  }
  return 0;
}

void ReadSMARTValuesForAllDrives(uint8_t id)
{
  SkDisk skDisk;
  SkDisk * p_skDisk;
//  sk_disk_check_power_mode();
  SkBool SMARTavailable;

  sk_disk_smart_is_available(& skDisk, & SMARTavailable);
//  sk_disk_smart_is_enabled(& skDisk);
  int i = sk_disk_open("/dev/sda", /* SkDisk **_d*/ & p_skDisk);
  std::cout << "result of sk_disk_open:" << i << std::endl;
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
      }
      uint64_t milliSeconds;
      i = sk_disk_smart_get_power_on(p_skDisk, & milliSeconds);
      if( i == 0)
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
          << (float)mkelvin/1000.0f - 273.1f << "°C" << std::endl;
      else
        std::cout << "failed to get temperature of drive:" << i << " errno:" << errno << std::endl;
//    const SkSmartAttributeInfo * p = lookup_attribute(& skDisk, id);
      uint64_t gSenseErrorRate;
      char * attr = "udma-crc-error-count";
      i = readAttribute(attr, p_skDisk, gSenseErrorRate);
      if( i == 0)
        std::cout << attr << ":" << gSenseErrorRate << std::endl;
      else
        std::cout << "Failed to get attribute value for \"" << attr << "\":"
          << strerror(errno) << std::endl;
    }
    else
      std::cout << "failed to read SMART data:" << i << " errno:" << errno << std::endl;
    sk_disk_free( p_skDisk);
  }
  else
    std::cout << "failed to open disk for SMART:" << i << " error:" <<
    /*errno*/ strerror(errno) << std::endl;
}

int main(void)
{
  ReadSMARTValuesForAllDrives(7);
}
