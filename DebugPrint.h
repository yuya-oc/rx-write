/*
 * DebugPrint.h
 *
 *  Created on: 2013/11/17
 *      Author: airwalker
 */

#ifndef DEBUGPRINT_H_
#define DEBUGPRINT_H_

#include <iostream>

#if 1
#define DebugPrint(x) {std::cout<<__FUNCTION__<<": "<<#x<<"="<<x<<std::endl;}
#define DebugFuncStart() {std::cout<<__FUNCTION__<<" >>"<<std::endl;}
#define DebugFuncEnd() {std::cout<<__FUNCTION__<<" <<"<<std::endl;}
#define DebugFuncReturn(x) {std::cout<<__FUNCTION__<<" << "<<x<<std::endl;}
#else
#define DebugPrint(x)
#define DebugFuncStart()
#define DebugFuncEnd()
#endif

#endif /* DEBUGPRINT_H_ */
