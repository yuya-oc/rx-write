
#include "rx621writer.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>

int main(int argc, char** argv){
  if(argc < 3){
    std::cout << "Usage: " << argv[0] << " DEVICE FILE [BAUDRATE]" << std::endl;
    return 1;
  }
  std::string device(argv[1]);
  std::string file(argv[2]);
  int baudrate = 38400;
  if(argc == 4){
    baudrate = atoi(argv[3]);
  }

  rx621writer writer;
  printf("Opening port\n");
//  serial.openPort("/dev/tty.usbserial-A3T9JF74", 19200);
//  if(writer.openPort("/dev/cu.usbserial-A3T9JF74", 9600) == false){
  if(writer.openPort(device.c_str(), 9600) == false){
    printf("Can't open serial port, exit\n");
    exit(1);
  }

  if(writer.adjustBitrate()==false){
    exit(1);
  }

  std::vector<rx621device> deviceList;
  if(writer.querySupportedDevices(deviceList)==false){
    exit(1);
  }
  std::cout << deviceList[0].m_deviceCode << std::endl;
  std::cout << deviceList[0].m_deviceName << std::endl;

  if(writer.selectDevice(deviceList[0])==false){
    exit(1);
  }

  std::vector<char> clockModeList;
  if(writer.queryClockMode(clockModeList)==false){
    exit(1);
  }

  writer.selectClockMode(clockModeList[0]);
  printf("clockModeList[0]=%d\n",clockModeList[0]);
  writer.queryClockMultiplier();
  writer.queryClockFrequency();
  writer.queryStatus();
  if(writer.selectNewBitrate(baudrate, 12000000) == false){
//  if(writer.selectNewBitrate(38400, 12000000) == false){
    printf("Bitrate error!\n");
//    return 1;
  }
  writer.queryStatus();
  writer.queryUserBootMatInfo();
  writer.queryUserMatInfo();
  writer.queryEraseBlockInfo();
  writer.queryWriteSize();

  writer.transitToWriteEraseStatus();
  printf("[!] User Flash have been deleted\n");

  if(writer.writeToUserMat(file.c_str())==true){
    printf("\nComplete!!!\n\n");
  }
  else{
    printf("\nFailed.....\n\n");
    return 1;
  }

  return 0;
}
