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

typedef enum E2704_regulationMode
{
    E2704_MODE_AUTO = 0,
    E2704_MODE_MANUAL
} E2704_RegulationMode;

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

/*
 * E2704_ask_service
 * ask to user in wich mode the regulator is needed
 * return:
 *     * regulation mode , 0 (Automatic) / 1 (Manual)
 *     * consigne:  
 *              - value of the puissance (for automatic)
 *              - value of temperature (for manual)
 */
void E2704_ask_service(E2704_RegulationMode *regulation_mode, short *consigne);

void E2704_set_regulation_mode(HANDLE hPort, E2704_RegulationMode mode);

void E2704_set_consigne(HANDLE hPort, E2704_RegulationMode mode, short consigne);

void E2704_write(HANDLE hPort, short data, int address);

short E2704_read(HANDLE hPort, int _address);

int E2704_createRequestTrame(TypeRequest i_requestType, char *i_trameSend, short value, int address);

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
