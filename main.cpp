
#include "rx621writer.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>

int main(int argc, char** argv){
  rx621writer writer;
  printf("Opening port\n");
//  serial.openPort("/dev/tty.usbserial-A3T9JF74", 19200);
  if(writer.openPort("/dev/cu.usbserial-A3T9JF74", 19200) == false){
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
  return 0;
}
