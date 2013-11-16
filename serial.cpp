
#include "serial.h"
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>

SerialPort::SerialPort() {
  m_fd = -1;
}

SerialPort::~SerialPort() {
  closePort();
}

void SerialPort::openPort(const char* device, int baudrate) {
  open(device, O_RDWR | O_NOCTTY);
  tcgetattr(m_fd, &m_oldTio); //退避

  struct termios tio;
  memset(&tio, 0, sizeof(tio));
  tio.c_cflag = CS8 | CLOCAL | CREAD;
  tio.c_iflag=IGNPAR;          /* IGNPAR:パリティエラーの文字は無視 */

  tio.c_oflag=0;               /* rawモード */
  tio.c_lflag=0;               /* 非カノニカル入力 */

  tio.c_cc[VTIME]=0;           /* キャラクタ間タイマは未使用 */
  tio.c_cc[VMIN]=1;            /* 1文字受け取るまでブロックする */

  cfsetispeed(&tio, baudrate);
  cfsetospeed(&tio, baudrate);

  tcflush(m_fd,TCIFLUSH);           /* ポートのクリア */
  tcsetattr(m_fd, TCSANOW, &tio); /* ポートの設定を有効にする */
}

void SerialPort::closePort(){
  if(m_fd >= 0){
    tcsetattr(m_fd, TCSANOW, &m_oldTio);
    close(m_fd);
  }
}

int SerialPort::putChar(unsigned char c) {
  return write(m_fd,&c,1);
}

int SerialPort::putBytes(const unsigned char* bytes, int num) {
  return write(m_fd, bytes, num);
}

int SerialPort::getChar() {
  unsigned char c;
  return read(m_fd, &c, 1);
}

int SerialPort::getBytes(unsigned char* bytes, int num) {
  return read(m_fd, bytes, num);
}
