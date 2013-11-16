/*
 * rx621writer.cpp
 *
 *  Created on: 2013/11/17
 *      Author: airwalker
 */

#include "rx621writer.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <cassert>

rx621writer::rx621writer() {
}

rx621writer::~rx621writer() {
}

bool rx621writer::openPort(const char* device, int baudrate) {
  return m_serial.openPort(device, baudrate);
}

bool rx621writer::adjustBitrate() {
  printf("Auto adjustment of bitrate");
  bool is_success = false;
  for(int i=0;i<30;i++){
    m_serial.putChar(0);
    printf(".");
  }
  printf("\n");

  unsigned char res;
  if(m_serial.getChar(&res)==false){
    return false;
  }
  if(res == 0x00){
    is_success = true;
  }

  if(is_success == false){
    printf("Auto adjustment failed\n");
    return false;
  }

  m_serial.putChar(0x55);
  m_serial.getChar(&res);
  printf("%d\n",res);
  switch(res){
  case 0xe6:
    printf("Auto adjustment finished\n");
    return true;
  case 0xff:
    printf("Auto adjustment error\n");
    return false;
  default:
    printf("Undefined response\n");
    return false;
  }
}

bool rx621writer::querySupportedDevices(std::vector<rx621device> &deviceList) {
  unsigned char res;
  m_serial.putChar(0x20);
  m_serial.getChar(&res);
  if(res!=0x30){
    return false;
  }

  unsigned char size=0;
  m_serial.getChar(&size);

  int response_length = size+2;
  unsigned char response[response_length];
  response[0] = res;
  response[1] = size;

  m_serial.getBytes(response+2, size);
  unsigned char sum;
  m_serial.getChar(&sum);
  unsigned char devNum = response[2];
  if(checkCorrectSum(sum, (char*)response, response_length)==false){
    return false;
  }

  printf("devNum=%d\n",devNum);
  int counter = 2;
  for(int i=0;i<devNum;i++){
    counter++;
    char length = response[counter];

    rx621device device;
    counter++;
    device.m_deviceCode.assign((char*)response+counter, 4);
    device.m_deviceName.assign((char*)response+counter+4, length-4);
    deviceList.push_back(device);

    counter+=length;
  }
  return true;
}

bool rx621writer::checkCorrectSum(const char sum, const char* data,
    const int num) const {
  char sigma = 0;
  for(int i=0;i<num;i++){
    sigma += data[i];
  }
  sigma += sum;
  printf("sigma=%d\n",sigma);
  if(sigma == 0x00){
    return true;
  }
  else{
    return false;
  }
}

bool rx621writer::selectDevice(const rx621device& device) {
  m_serial.putChar(0x10);
  m_serial.putChar(4);
  m_serial.putBytes((unsigned char*)(device.m_deviceCode.c_str()), 4);
  char sum = calculateSum(device.m_deviceCode.c_str(), 4);
  m_serial.putChar(sum-4-0x10);
  unsigned char res;
  m_serial.getChar(&res);
  switch(res){
  case 0x06:
    std::cout << "OK" << std::endl;
    return true;
  case 0x90:
    m_serial.getChar(&res);
    printf("Error: %xh\n", res);
    return false;
  default:
    std::cout << "Unknown error" << std::endl;
    return false;
  }
}

bool rx621writer::queryClockMode(std::vector<char> &clockModeList) {
  m_serial.putChar(0x21);
  unsigned char res;
  m_serial.getChar(&res);
  if(res != 0x31){
    return false;
  }

  unsigned char size;
  m_serial.getChar(&size);
  printf("size=%d\n",size);

  int response_length = size+2;
  unsigned char response[response_length];
  response[0] = res;
  response[1] = size;
  m_serial.getBytes(response+2, size);
  unsigned char sum;
  m_serial.getChar(&sum);
  if(checkCorrectSum(sum, (char*)response, response_length)==false){
    return false;
  }

  for(int i=0;i<size;i++){
    clockModeList.push_back(response[2+i]);
  }
  return true;
}

char rx621writer::calculateSum(const char* data, const int num) const {
  char sigma = 0;
  for(int i=0;i<num;i++){
    sigma += data[i];
  }
  char sum = 0x00 - sigma;
  assert(checkCorrectSum(sum, data, num)==true);
  return sum;
}
