/*
 * rx621writer.cpp
 *
 *  Created on: 2013/11/17
 *      Author: airwalker
 */

#include "rx621writer.h"
#include "DebugPrint.h"
#include "MotSRecordFile.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <cassert>
#include <fstream>

rx621writer::rx621writer() {
}

rx621writer::~rx621writer() {
}

bool rx621writer::openPort(const char* device, int baudrate) {
  m_serialDevice.assign(device);
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
  DebugFuncStart();
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

  for(int i=0;i<size+2;i++){
    printf("%02xh ",(unsigned int)response[i]);
  }
  printf("%02xh ",(unsigned int)sum);
  printf("\n");

  int num_of_clk_mode = size;
  DebugPrint(num_of_clk_mode);
  for(int i=0;i<size;i++){
    std::cout << (int)response[2+i] << std::endl;
    clockModeList.push_back(response[2+i]);
  }
  DebugFuncEnd();
  return true;
}

bool rx621writer::selectClockMode(char clockMode) {
  DebugFuncStart();
  DebugPrint((unsigned int)clockMode);
  unsigned char buffer[4];
  buffer[0] = 0x11;
  buffer[1] = 1;
  buffer[2] = clockMode;
  buffer[3] = calculateSum((char*)buffer, 3);
  m_serial.putBytes(buffer, 4);

  unsigned char res;
  m_serial.getChar(&res);
  switch(res){
  case 0x06:
    std::cout << "OK" << std::endl;
    DebugFuncReturn(true);
    return true;
  case 0x91:
    m_serial.getChar(&res);
    printf("Error: %xh\n", res);
    return false;
  default:
    std::cout << "Unknown error" << std::endl;
    return false;
  }
}

bool rx621writer::queryClockMultiplier() {
  DebugFuncStart();
  m_serial.putChar(0x22);
  unsigned char res;
  m_serial.getChar(&res);
  DebugPrint((int)res);
  if(res!=0x32){
    return false;
  }
  unsigned char size;
  m_serial.getChar(&size);
  DebugPrint((int)size);

  char buffer[size+2];
  m_serial.getBytes((unsigned char*)buffer+2, size);
  buffer[0] = res;
  buffer[1] = size;

  unsigned char sum;
  m_serial.getChar(&sum);
  checkCorrectSum(sum, buffer, size+2);

  int num_of_clock = buffer[2];
  std::vector<std::vector<char> > clockList;
  int counter = 3;
  for(int i=0;i<num_of_clock;i++){
    std::vector<char> clocks;
    DebugPrint(i);
    for(int j=0;j<buffer[counter];j++){
      DebugPrint(j);
      DebugPrint((int)buffer[counter+j+1]);
      clocks.push_back(buffer[counter+j+1]);
    }
    clockList.push_back(clocks);

    counter += buffer[counter] + 1;
  }

  DebugFuncReturn(true);
  return true;
}

bool rx621writer::queryClockFrequency() {
  DebugFuncStart();

  m_serial.putChar(0x23);
  unsigned char res;
  m_serial.getChar(&res);
  DebugPrint((int)res);
  if(res != 0x33){
    DebugFuncReturn(false);
    return false;
  }

  unsigned char size;
  m_serial.getChar(&size);
  DebugPrint((int)size);
  unsigned char buffer[size+2];
  m_serial.getBytes(buffer+2, size);
  buffer[0] = res;
  buffer[1] = size;

  unsigned char sum;
  m_serial.getChar(&sum);
  checkCorrectSum(sum, (char*)buffer, size+2);

  int num_of_clock = buffer[2];
  DebugPrint(num_of_clock);
  std::vector<int> maxFreqList;
  std::vector<int> minFreqList;
  int counter = 3;
  for(int i=0;i<num_of_clock;i++){
    minFreqList.push_back((buffer[counter] << 8) + buffer[counter+1]);
    maxFreqList.push_back((buffer[counter+2] << 8) + buffer[counter+3]);
    DebugPrint(i);
    DebugPrint(minFreqList[i]);
    DebugPrint(maxFreqList[i]);
    counter+=4;
  }

  DebugFuncReturn(true);
  return true;
}

bool rx621writer::queryUserBootMatInfo() {
  DebugFuncStart();

  m_serial.putChar(0x24);
  unsigned char res;
  m_serial.getChar(&res);
  DebugPrint((int)res);
  if(res != 0x34){
    DebugFuncReturn(false);
    return false;
  }

  unsigned char size;
  m_serial.getChar(&size);
  DebugPrint((int)size);
  unsigned char buffer[size+2];
  m_serial.getBytes(buffer+2, size);
  buffer[0] = res;
  buffer[1] = size;

  unsigned char sum;
  m_serial.getChar(&sum);
  checkCorrectSum(sum, (char*)buffer, size+2);

  int num_of_area = buffer[2];
  DebugPrint(num_of_area);
  int counter = 3;
  std::vector<unsigned int> startList;
  std::vector<unsigned int> lastList;
  for(int i=0;i<num_of_area;i++){
    startList.push_back((buffer[counter]<<24)+(buffer[counter+1]<<16)+(buffer[counter+2]<<8)+buffer[counter+3]);
    lastList.push_back((buffer[counter+4]<<24)+(buffer[counter+5]<<16)+(buffer[counter+6]<<8)+buffer[counter+7]);
    printf("startList[i]=0x%8X\n", startList[i]);
    printf("lastList[i]=0x%8X\n", lastList[i]);
    counter+=8;
  }

  DebugFuncReturn(true);
  return true;
}

bool rx621writer::queryUserMatInfo() {
  DebugFuncStart();

  m_serial.putChar(0x25);
  unsigned char res;
  m_serial.getChar(&res);
  DebugPrint((int)res);
  if(res != 0x35){
    DebugFuncReturn(false);
    return false;
  }

  unsigned char size;
  m_serial.getChar(&size);
  DebugPrint((int)size);
  unsigned char buffer[size+2];
  m_serial.getBytes(buffer+2, size);
  buffer[0] = res;
  buffer[1] = size;

  unsigned char sum;
  m_serial.getChar(&sum);
  checkCorrectSum(sum, (char*)buffer, size+2);

  int num_of_area = buffer[2];
  DebugPrint(num_of_area);
  int counter = 3;
  std::vector<unsigned int> startList;
  std::vector<unsigned int> lastList;
  for(int i=0;i<num_of_area;i++){
    startList.push_back((buffer[counter]<<24)+(buffer[counter+1]<<16)+(buffer[counter+2]<<8)+buffer[counter+3]);
    lastList.push_back((buffer[counter+4]<<24)+(buffer[counter+5]<<16)+(buffer[counter+6]<<8)+buffer[counter+7]);
    printf("startList[i]=0x%8X\n", startList[i]);
    printf("lastList[i]=0x%8X\n", lastList[i]);
    counter+=8;
  }

  DebugFuncReturn(true);
  return true;
}

bool rx621writer::queryEraseBlockInfo() {
  DebugFuncStart();

  m_serial.putChar(0x26);
  unsigned char res;
  m_serial.getChar(&res);
  DebugPrint((int)res);
  if(res != 0x36){
    DebugFuncReturn(false);
    return false;
  }

  unsigned char sizebuf[2];
  m_serial.getBytes(sizebuf, 2);
  int size = (sizebuf[0] << 8) + sizebuf[1];
  DebugPrint(size);
  unsigned char buffer[size+3];
  m_serial.getBytes(buffer+3, size);
  buffer[0] = res;
  buffer[1] = sizebuf[0];
  buffer[2] = sizebuf[1];

  for(int i=0;i<3+size;i++){
    printf("%08xh ", (char)buffer[i]);
    if((i+5)%8==0) printf("\n");
  }

  unsigned char sum;
  m_serial.getChar(&sum);
  if(checkCorrectSum(sum, (char*)buffer, size+3)==false){
    std::cout << "CheckSum error" << std::endl;
//    return false;
  };

  int num_of_area = buffer[3];
  DebugPrint(num_of_area);
  int counter = 4;
  std::vector<unsigned int> startList;
  std::vector<unsigned int> lastList;
  for(int i=0;i<num_of_area;i++){
    DebugPrint(i);
    unsigned int start = (buffer[counter]<<24) + (buffer[counter+1]<<16) + (buffer[counter+2]<<8) + (buffer[counter+3]);
    counter+=4;
    unsigned int last = (buffer[counter]<<24) + (buffer[counter+1]<<16) + (buffer[counter+2]<<8) + (buffer[counter+3]);
    counter+=4;
    DebugPrint(start);
    DebugPrint(last);
    startList.push_back(start);
    lastList.push_back(last);
  }

  DebugFuncReturn(true);
  return true;
}

bool rx621writer::queryWriteSize() {
  DebugFuncStart();

  m_serial.putChar(0x27);
  unsigned char res;
  m_serial.getChar(&res);
  DebugPrint((int)res);
  if(res != 0x37){
    DebugFuncReturn(false);
    return false;
  }

  unsigned char size;
  m_serial.getChar(&size);
  DebugPrint((int)size);
  unsigned char buff[2+size];
  m_serial.getBytes(buff+2,size);
  buff[0] = res;
  buff[1] = size;

  unsigned char sum;
  m_serial.getChar(&sum);
  checkCorrectSum(sum, (char*)buff, 2+size);
  DebugPrint((int)size);

  unsigned int write_size = (buff[2]<<8) + buff[3];
  DebugPrint(write_size);

  DebugFuncReturn(true);
  return true;
}

bool rx621writer::selectNewBitrate(int bitrate, int clockHz) {
  DebugFuncStart();
  int size = 7;
  unsigned char buff[size+2];
  buff[0] = 0x3f;
  buff[1] = size;

  buff[2] = (bitrate/100) >> 8;
  buff[3] = (bitrate/100);
  int clock = clockHz / 10000;
  buff[4] = (clock >> 8);
  buff[5] = clock;
  DebugPrint(clock);
  printf("bitrate=%d\n",(buff[2]<<8) + buff[3]);
  printf("clock=%xh\n",clock);
  printf("clock>>8=%xh\n",clock>>8);
  printf("buff[4]=%xh\n",buff[4]);
  printf("buff[5]=%xh\n",buff[5]);
  printf("buff[4]<<8=%xh\n",buff[4]<<8);
  clock = 0xffff & ((buff[4]<<8) + buff[5]);
  DebugPrint((int)buff[4]);
  DebugPrint((int)(buff[4])<<8);
  DebugPrint((buff[4]<<8) + buff[5]);
  DebugPrint(clock);
  printf("clock=%xh\n",clock);

  int num_of_clock = 2;
  buff[6] = num_of_clock;
  buff[7] = 8;
  buff[8] = 4;
  char sum = calculateSum((char*)buff, size+2);

  std::cout << "sending data" << std::endl;
  m_serial.putBytes((unsigned char*)buff, size+2);
  std::cout << "sending sum" << std::endl;
  m_serial.putChar(sum);
//  m_serial.putChar(1);

  std::cout << "get res" << std::endl;
  unsigned char res=0;
  m_serial.getChar(&res);
  printf("res=%xh\n",res);
  if(res != 0x06){
    unsigned char error;
    m_serial.getChar(&error);
    printf("error=%02xh\n",error);
//    DebugFuncReturn(false);
//    return false;
  }

//  m_serial.changeBaudrate(bitrate);
  m_serial.closePort();
  m_serial.openPort(m_serialDevice.c_str(), bitrate);

//  sleep(1);
  sleep(1);
  std::cout << "send ack" << std::endl;
  m_serial.putChar(0x06);
  res = 0;
  m_serial.getChar(&res);
  //1bit待ちの処理でずれる?
  if(res>>5 != 0x06){
    printf("change error: res=%02xh\n", res);
    DebugFuncReturn(false);
    return false;
  }
  else{
    printf("change OK? res=%02xh(%02xh)\n", res, res>>5);
  }

  DebugFuncReturn(true);
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

bool rx621writer::queryStatus() {
  DebugFuncStart();

  m_serial.putChar(0x4f);
  unsigned char res;
  m_serial.getChar(&res);
  DebugPrint((int)res);
  if(res != 0x5f){
    DebugFuncReturn(false);
    return false;
  }
  unsigned char size;
  m_serial.getChar(&size);
  unsigned char status=0;
  m_serial.getChar(&status);
  unsigned char error=0;
  m_serial.getChar(&error);
  printf("status=%02xh, error%02xh\n", status, error);
  unsigned char sum=0;
  m_serial.getChar(&sum);
  DebugFuncReturn(true);
  return true;
}

bool rx621writer::transitToWriteEraseStatus() {
  DebugFuncStart();

  m_serial.putChar(0x40);
  unsigned char res = 0;
  for(int i=0;i<10;i++){
    if(m_serial.getChar(&res)==true) break;
  }

  switch(res){
  case 0x26:
    std::cout << "ID protect disabled" << std::endl;
    break;
  case 0x16:
    std::cout << "ID protect enabled, not support" << std::endl;
    DebugFuncReturn(false);
    return false;
  default:
    std::cout << "Unknown error:" << (unsigned int)res << std::endl;
    printf("Unknown error: res=%2xh\n",res);
    m_serial.getChar(&res);
    printf("Unknown error: res=%2xh\n",res);
    DebugFuncReturn(false);
    return false;
  }

  DebugFuncReturn(true);
  return true;
}

bool rx621writer::writeToUserMat(const std::string& motFile) {
  DebugFuncStart();
  //ファイルチェック
  MotSRecordFile mot;
  if(mot.openMotFile(motFile) == false){
    std::cout << "Can't open "<< motFile << std::endl;
    DebugFuncReturn(false);
    return false;
  }

  //ROM書き込みモードへ遷移
  m_serial.putChar(0x43);
  unsigned char res;
  m_serial.getChar(&res);
  DebugPrint((unsigned int)res);
  if(res != 0x06){
    DebugFuncReturn(false);
    return false;
  }

  unsigned int start_address = mot.getStartAddress();
  unsigned int address = start_address;
  int image_size = mot.getImageSize();
  if(image_size % 256 > 0){
    image_size += 256 - image_size%256;
  }
  DebugPrint(mot.getImageSize());
  DebugPrint(image_size);
  unsigned char image_buff[image_size];
  for(int i=0;i<image_size;i++){
    image_buff[i] = 0xff;
  }
  mot.getRawImage(image_buff, image_size);
  int wrote_size = 0;
  while(address-start_address < image_size){
    bool is_all_0xff = true;
    for(int i=0;i<256;i++){
      if(image_buff[address-start_address+i] != 0xff){
        is_all_0xff = false;
      }
    }
    if(is_all_0xff){
      //printf("Skip %X\n", address);
      address+=256;
      continue;
    }

    unsigned char buff[256+6];
    buff[0] = 0x50;
    buff[1] = address >> 24;
    buff[2] = address >> 16;
    buff[3] = address >> 8;
    buff[4] = address >> 0;
    printf("address= %8X\n", address);
    printf("st_addr= %8X\n", start_address);
    DebugPrint(address - start_address);
    memcpy(buff+5, image_buff+address-start_address, 256);
    buff[256+5] = calculateSum((char*)buff, 256+5);
    m_serial.putBytes((unsigned char*)buff, 256+6);

    res = 0;
    m_serial.getChar(&res);
    printf("write to %x res=%02xh\n",address, (unsigned int)res);
    if(res!=0x06){
      res = 0;
      m_serial.getChar(&res);
      printf("error=%02xh\n",(unsigned int)res);
      return false;
    }
    address += 256;
    wrote_size += 256;
  }
  DebugPrint(mot.getImageSize());
  DebugPrint(address - start_address);

  char buff[6];
  address = 0xffffffff;
  buff[0] = 0x50;
  buff[1] = address >> 24;
  buff[2] = address >> 16;
  buff[3] = address >> 8;
  buff[4] = address >> 0;
  buff[5] = calculateSum(buff, 5);
  m_serial.putBytes((unsigned char*)buff, 6);

  res = 0;
  m_serial.getChar(&res);
  printf("write end res=%02xh\n",(unsigned int)res);
  if(res!=0x06){
    res = 0;
    m_serial.getChar(&res);
    printf("error=%02xh\n",(unsigned int)res);
  }
  printf("\nActual ROM size = %d bytes!\n", wrote_size);
  return true;
}
