/*
 * rx621writer.cpp
 *
 *  Created on: 2013/11/17
 *      Author: airwalker
 */

#include "rx621writer.h"

rx621writer::rx621writer() {
}

rx621writer::~rx621writer() {
}

bool rx621writer::openPort(const char* device, int baudrate) {
  return m_serial.openPort(device, baudrate);
}
