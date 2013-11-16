
#include <termios.h>
//#include <string>

class SerialPort{
public:
  SerialPort();
  virtual ~SerialPort();
  void openPort(const char* device, int baudrate);
  void closePort();
  int putChar(unsigned char c);
  int putBytes(const unsigned char* bytes, int num);
  int getChar();
  int getBytes(unsigned char* bytes, int num);
private:
  struct termios m_oldTio;
  int m_fd;
};
