/*
 *  ModbusTp.h
 *  
 *
 *  Created by SIEPRAWSKI Audrey on 18/03/2015.
 *
 */

#ifndef MODBUSTP_H
#define MODBUSTP_H

#include "Common.h"
#include "SerialPort.h"

typedef enum typerequest {
  REQUEST_READ = 1,
  REQUEST_WRITE,
  REQUEST_QUIT,
  NO_REQUEST

  } TypeRequest;


typedef enum typeval {
  TYPE_SHORT = 0,
  TYPE_INT,
  TYPE_FLOAT,
  NO_TYPE

  } TypeVal;


void printState(ErrorComm codret);

HANDLE connectionSerialPort();

int createRequestTrame(TypeRequest i_requestType, char* i_trameSend, TypeVal* i_typeVal);
ErrorComm parseModbusResponse(char* i_trameReceive, int i_lengthTrameReceived, TypeRequest i_requestType, TypeVal i_typeVal);

void printTrame(char * type, char trame[100], int lenghtTrame);

#endif
