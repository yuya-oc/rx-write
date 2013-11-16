/*
 * rx621writer.h
 *
 *  Created on: 2013/11/17
 *      Author: airwalker
 */

#ifndef RX621WRITER_H_
#define RX621WRITER_H_

#include "serial.h"
#include <string>
#include <vector>

class rx621device{
public:
  std::string m_deviceCode;
  std::string m_deviceName;
};

class rx621writer {
public:
  rx621writer();
  virtual ~rx621writer();
  bool openPort(const char* device, int baudrate);
  bool adjustBitrate();
  bool querySupportedDevices(std::vector<rx621device> &deviceList);
  bool selectDevice(const rx621device &device);
  bool queryClockMode(std::vector<char> &clockModeList);
  bool checkCorrectSum(const char sum, const char* data, const int num) const;
  char calculateSum(const char* data, const int num) const;
private:
  SerialPort m_serial;
};

#endif /* RX62WRITER_H_ */
