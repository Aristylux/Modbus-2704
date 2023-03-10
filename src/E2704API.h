#ifndef E_API_H
#define E_API_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> // files

#include "Common.h"
#include "Modbus.h"
#include "E2704.h"

// Files Names

#define F_CONFIG_SERIAL     (char*) "config_serial.json"
#define F_CONFIG_PARAM_R    (char*) "config_parameter_to_read.json"
#define F_CONFIG_PARAM_W    (char*) "config_parameter_to_write.json"
#define F_CONFIG_CONSIGNE   (char*) "config_consigne.json"

// Characters for box

#define CORNER_T_L  (char) 218  // Top left corner              (┌)
#define CORNER_T_R  (char) 191  // Top right corner             (┐)
#define CORNER_B_L  (char) 192  // Bottom left corner           (└)
#define CORNER_B_R  (char) 217  // Bottom right corner          (┘)

#define LINE_H      (char) 179  // Line horizonal               (│)
#define LINE_V      (char) 196  // Line vertical                (─)

#define LINE_V_R    (char) 195  // Line vertical right          (├)
#define LINE_V_L    (char) 180  // Line vertical left           (┤)

#define LINE_H_D    (char) 194  // Line horizontal down         (┬)
#define LINE_H_U    (char) 193  // Line horizontal up           (┴)

#define LINE_V_H    (char) 197  // Line vertical & horizontal   (┼)

typedef enum E2704_channel
{
    CH1 = 1,
    CH2,
    CH3
} E2704_Channel;

// -- Struct of config serial port --

typedef struct E2704_config
{
    int port;
    int baud;
    short bits;
    short bit_parity;
    short bit_stop;
} t_E2704_config;

// -- List of parameters --

typedef struct E2704_parameter
{
    char *name;
    int address;
    short value;
    struct E2704_parameter *next;
} t_E2704_parameter;

typedef struct E2704_parameter_list
{
    t_E2704_parameter *parameterList;
    int num_params;
    int len_param;
    int len_row_ch;
    char *blank;
} t_E2704_parameter_list;


// -- List --

t_E2704_parameter_list *initParameterList(void);
t_E2704_parameter *newParameter(char *paramName, int address);
void setParameterValue(t_E2704_parameter_list *paramList, char *paramName, short value);
void addParameter(t_E2704_parameter_list *paramList, char *paramName, int address);
void printList(t_E2704_parameter_list *paramList);
void freeList(t_E2704_parameter_list *paramList);

// -- E2704 functions --

ErrorComm E2704_getValue(HANDLE hPort, t_E2704_parameter_list *paramList, E2704_Channel channel);
void E2704_write_consigne(HANDLE hPort, t_E2704_parameter_list *paramList, char *paramName, E2704_Channel channel);
void E2704_setConsigne(HANDLE hPort, t_E2704_parameter_list *paramList, E2704_RegulationMode mode, short consigne, E2704_Channel channel);
void E2704_setParametersRead(t_E2704_parameter_list *paramList);
void E2704_setParametersWrite(t_E2704_parameter_list *paramList);
int E2704_setServiceUser(HANDLE hPort, t_E2704_parameter_list *paramList);

ErrorComm E2704_sendRequest(HANDLE hPort, TypeRequest requestType, short data, int address, char *trameOut, int *lenTrameOut);
void E2704_write(HANDLE hPort, short data, int address);
short E2704_read(HANDLE hPort, int _address);
int E2704_createRequestTrame(TypeRequest i_requestType, char *i_trameSend, short value, int address);

// -- Print --

void printParameterRow(t_E2704_parameter_list *paramList);
void printChannel(t_E2704_parameter_list *paramList, E2704_Channel channel);
void printValueChannel(t_E2704_parameter_list *paramList, t_E2704_parameter *parameter, E2704_Channel channel);
void clearChannel(t_E2704_parameter_list *paramList, E2704_Channel channel);
void printLine(const int len, const int offset, char firstChar);
void printEnd(t_E2704_parameter_list *paramList, E2704_Channel lastChannel);

// -- Files --

int config_file_exist(const char *configFileName);
t_E2704_config E2704_getSerialPortConfig(const char *configFileName);
void E2704_getParameterConfig(const char *configFileName, t_E2704_parameter_list *paramList);
int E2704_getConsigneConfig(HANDLE hPort, t_E2704_parameter_list *paramList, const char *configFileName);
void removeChar(char * str, char charToRemmove);
void removeCharStart(char * str, char charToRemmove);

#endif