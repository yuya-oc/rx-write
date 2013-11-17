/*
 * MotSRecordFile.h
 *
 *  Created on: 2013/11/17
 *      Author: airwalker
 */

#ifndef MOTSRECORDFILE_H_
#define MOTSRECORDFILE_H_

#include <string>
#include <vector>

class MotSRecordFile {
public:
  MotSRecordFile();
  virtual ~MotSRecordFile();
  bool openMotFile(const std::string &motFile);
  unsigned int getStartAddress(){
    return m_startAddress;
  }
  bool getRawImage(unsigned char* buff, int length);
  int getImageSize(){
    return m_rawImage.size();
  }
private:
  std::vector<unsigned char> m_rawImage;
  unsigned int m_startAddress;
};

class AddressAndData{
public:
  unsigned int address;
  unsigned char val;
};

#endif /* MOTSRECORDFILE_H_ */
