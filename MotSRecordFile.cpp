/*
 * MotSRecordFile.cpp
 *
 *  Created on: 2013/11/17
 *      Author: airwalker
 */

#include "MotSRecordFile.h"
#include "DebugPrint.h"
#include <fstream>
#include <cstdlib>

MotSRecordFile::MotSRecordFile() {
}

MotSRecordFile::~MotSRecordFile() {
  // TODO Auto-generated destructor stub
}

bool MotSRecordFile::openMotFile(const std::string& motFile) {
  std::ifstream ifs(motFile);
  if(!ifs){
    std::cout << "Can't open " << motFile << std::endl;
    return false;
  }

  m_startAddress = 0;
  std::string header;
  ifs >> header;
  DebugPrint(header);

  std::vector<AddressAndData> dataArray;
  while(!ifs.eof()){
    bool is_eof = false;
    std::string record;
    ifs >> record;
    DebugPrint(record);
    switch(record[1]){
    case '7':
      is_eof = true;
      break;
    case '3':
      break;
    }
    if(is_eof) break;
    std::string size_string(record.c_str()+2, 2);
    int size = strtol(size_string.c_str(), NULL, 16);
    DebugPrint(size_string);
    DebugPrint(size);
    std::string address_string(record.c_str()+4, 8);
    DebugPrint(address_string);
    unsigned int address = strtol(address_string.c_str(), NULL,16);
    DebugPrint(address);
    printf("address = 0x%X\n",address);

    if(m_startAddress==0) m_startAddress = address;

    for(int i=0;i<size-5;i++){
      std::string val_string(record.c_str()+12 + i*2, 2);
      unsigned char val = strtol(val_string.c_str(), NULL, 16);
      AddressAndData data;
      data.address = address + i;
      data.val = val;
      dataArray.push_back(data);
      printf("%02x ", (unsigned int)val);
    }
    printf("\n");
  }

  unsigned int last_address = (dataArray.end()-1)->address;
  DebugPrint(last_address);
  m_rawImage.clear();
  m_rawImage.resize(last_address - m_startAddress + 1, 0xff);
  for(std::vector<AddressAndData>::const_iterator it=dataArray.begin(); it!=dataArray.end(); ++it){
    unsigned int index = it->address - m_startAddress;
    m_rawImage[index] = it->val;
  }

  return true;
}

bool MotSRecordFile::getRawImage(unsigned char* buff, int length) {
  if(length < getImageSize()){
    return false;
  }
  for(int i=0;i<getImageSize();i++){
    buff[i] = m_rawImage[i];
  }

  return true;
}
