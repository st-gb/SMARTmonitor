/*
 * SkDisk.h
 *
 *  Created on: 30.07.2016
 *      Author: sg
 */

#ifndef LIBATA_SMART_SKDISK_H_
#define LIBATA_SMART_SKDISK_H_

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

#endif /* LIBATA_SMART_SKDISK_H_ */
