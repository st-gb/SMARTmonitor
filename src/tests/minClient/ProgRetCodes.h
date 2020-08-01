#pragma once///Include guard

#ifdef __cplusplus
using namespace OperatingSystem::BSD::sockets;
#endif

enum ProgRetCodes{cnnctToSrvFailed = getHostByNameFailed + 1,
  readLessThanIntended, allocMemFailed};
