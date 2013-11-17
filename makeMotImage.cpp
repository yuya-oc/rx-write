/*
 * makeMotImage.cpp
 *
 *  Created on: 2013/11/17
 *      Author: airwalker
 */

#include "MotSRecordFile.h"

#include <iostream>

int main(){
  std::cout << __FUNCTION__ << std::endl;
  MotSRecordFile motImage;
  motImage.openMotFile("test.mot");
  int size = motImage.getImageSize();
  unsigned char image_buff[size];
  motImage.getRawImage(image_buff, size);
  return 0;
}
