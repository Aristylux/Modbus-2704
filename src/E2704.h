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

typedef enum typerequest
{
    REQUEST_READ = 1,
    REQUEST_WRITE,
    REQUEST_QUIT,
    NO_REQUEST
} TypeRequest;

typedef enum typeval
{
    TYPE_SHORT = 0,
    TYPE_INT,
    TYPE_FLOAT,
    NO_TYPE
} TypeVal;

// E2704 API
typedef struct E2704_config
{
    int port;
    int baud;
    short bits;
    short bit_parity;
    short bit_stop;
} T_E2704_config;

// Main program for E2704
void E2704_main(HANDLE hPort);

// Debug, like TP3
void E2704_debug(HANDLE hPort);

HANDLE connectionSerialPort();

// Used for debug
int createRequestTrame(TypeRequest i_requestType, char *i_trameSend, TypeVal *i_typeVal);

// Used for debug
ErrorComm parseModbusResponse(char *i_trameReceive, int i_lengthTrameReceived, TypeRequest i_requestType, TypeVal i_typeVal);

// Print State of the connection
void printState(ErrorComm codret);

// Print a trame in Hex
void printTrame(char *type, char trame[100], int lenghtTrame);

// Print help
void printHelp(void);

#endif
