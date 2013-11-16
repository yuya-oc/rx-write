/*
 * rx621writer.h
 *
 *  Created on: 2013/11/17
 *      Author: airwalker
 */

#ifndef RX621WRITER_H_
#define RX621WRITER_H_

#include "serial.h"

class rx621writer {
public:
  rx621writer();
  virtual ~rx621writer();
  bool openPort(const char* device, int baudrate);
private:
  SerialPort m_serial;
};

#endif /* RX62WRITER_H_ */
