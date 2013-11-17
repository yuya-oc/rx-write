
#ifndef SERIAL_H_
#define SERIAL_H_

#include <termios.h>
#include <unistd.h>

class SerialPort{
public:
  SerialPort();
  virtual ~SerialPort();
  bool openPort(const char* device, int baudrate);
  bool changeBaudrate(int baudrate);
  void closePort();
  int putChar(unsigned char c);
  int putBytes(const unsigned char* bytes, int num);
  bool getChar(unsigned char* c);
  bool getBytes(unsigned char* bytes, int num);
private:
  struct termios m_oldTio;
  int m_fd;
  struct timeval m_timeout;
  fd_set m_readfs;
};

#endif
