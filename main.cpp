
#include "serial.h"
#include <cstdio>
#include <cstdlib>

int main(int argc, char** argv){
  SerialPort serial;
  printf("Opening port\n");
//  serial.openPort("/dev/tty.usbserial-A3T9JF74", 19200);
  if(serial.openPort("/dev/cu.usbserial-A3T9JF74", 19200) == false){
    printf("Can't open serial port, exit\n");
    exit(1);
  }
  printf("Auto adjustment of bitrate");
  bool is_success = false;
  for(int i=0;i<30;i++){
    serial.putChar(0);
    printf(".");
  }
  printf("\n");

  unsigned char res;
  if(serial.getChar(&res)==false){
    exit(1);
  }
  if(res == 0x00){
    is_success = true;
  }

  if(is_success == false){
    printf("Auto adjustment failed, exit\n");
    exit(1);
  }

  serial.putChar(0x55);
  serial.getChar(&res);
  printf("%d\n",res);
  switch(res){
  case 0xe6:
    printf("Auto adjustment finished\n");
    break;
  case 0xff:
    printf("Auto adjustment error, exit\n");
    exit(1);
  default:
    printf("Undefined response, exit\n");
    exit(1);
  }

  serial.putChar(0x20);
  serial.getChar(&res);
  if(res==0x30){
    unsigned char size=0;
    serial.getChar(&size);
    unsigned char devNum=0;
    serial.getChar(&devNum);
    printf("devNum=%d\n",devNum);
    for(int i=0;i<devNum;i++){
      unsigned char charNum;
      serial.getChar(&charNum);
      printf("charNum=%d\n",charNum);
      int32_t devcode;
      serial.getBytes((unsigned char*)&devcode, 4);
      printf("devcode=%s\n",(unsigned char*)&devcode);
      unsigned char* name = new unsigned char[charNum+1-4];
      serial.getBytes(name,charNum-4);
      name[charNum-4]='\0';
      printf("name=%s\n",name);
      delete name;
    }
    unsigned char sum;
    serial.getChar(&sum);
  }

//  serial.putChar(0x);


  return 0;
}
