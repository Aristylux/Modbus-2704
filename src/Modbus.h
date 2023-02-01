/*
 *  Modbus.h
 *  commsLib
 *
 *  Created by Audrey SIEPRAWSKI on 11/03/2015
 */

// Sous Windows : linker avec -lwsock32

#ifndef MODBUS_H
#define MODBUS_H

#include "Common.h"

#include <windows.h>

typedef struct _bits
{
    unsigned sh0:1;
    unsigned sh1:1;
    unsigned sh2:1;
    unsigned sh3:1;
    unsigned sh4:1;
    unsigned sh5:1;
    unsigned sh6:1;
    unsigned sh7:1;
    unsigned sh8:1;
    unsigned sh9:1;
    unsigned sh10:1;
    unsigned sh11:1;
    unsigned sh12:1;
    unsigned sh13:1;
    unsigned sh14:1;
    unsigned sh15:1;
    unsigned sh16:1;
    unsigned sh17:1;
    unsigned sh18:1;
    unsigned sh19:1;
    unsigned sh20:1;
    unsigned sh21:1;
    unsigned sh22:1;
    unsigned sh23:1;
    unsigned sh24:1;
    unsigned sh25:1;
    unsigned sh26:1;
    unsigned sh27:1;
    unsigned sh28:1;
    unsigned sh29:1;
    unsigned sh30:1;
    unsigned sh31:1;
} IsenBits;

// Entrees et Sorties
typedef union _IsenUIntFloatBits
{
    float			_fvalue;
    int				_ivalue;
    short			_svalue;
    char			_cvalue;
    IsenBits		_bits;
} IsenUIntFloatBits;


// lecture N  bits
#define MODBUS_FUNCTION_READ_BITS			0x01 // 01 ou 02
// lecture N mots dans les "holding registers"
#define MODBUS_FUNCTION_READ_NWORDS			0x03 // 03
// lecture N mots dans les "input registers"
#define MODBUS_FUNCTION_READ_INPUT_NWORDS	0x04 // 04
// écriture 1 bit
#define MODBUS_FUNCTION_WRITE_BIT			0x05 // 05
// écriture 1 mot
#define MODBUS_FUNCTION_WRITE_WORD			0x06 // 06
// lecture rapide etat
#define MODBUS_FUNCTION_READ_STATE			0x07 // 07
// bouclage
#define MODBUS_FUNCTION_LOOP				0x08 // 08
// écriture n bits
#define MODBUS_FUNCTION_WRITE_BITS			0x0F // 15
// écriture n mots
#define MODBUS_FUNCTION_WRITE_WORDS			0x10 // 16

typedef enum enum_modbus_processorCompatibility {
    MOTOROLA=0,
    INTEL
} EModBusProcessorCompatibility;


#define MODBUS_ENCODING					0
#define MODBUS_TRANSAC_ID				0

unsigned short	ModBusCalculCRC(char* trame, int len);

void  ModBusWrite(char* trame, int i_index, unsigned short mot, EModBusProcessorCompatibility processorCompatibility);
void  ModBusRead(char* trame, int i_index, unsigned char* lsb, unsigned char* msb, const EModBusProcessorCompatibility processorCompatibility);

int makeTrameLecModBusForState( int address, int codeFunction,char* trame, EModBusProcessorCompatibility /* = INTEL ou MOTOROLA*/ );
int makeTrameLecModBus( int address, int codeFunction, int startAddress, int nbParams, char* trame, EModBusProcessorCompatibility /* = INTEL ou MOTOROLA */ );

int makeTrameEcrModBusFromShort( int address, int codeFunction, int startAddress, short         value   , char* trame, EModBusProcessorCompatibility /* = INTEL ou MOTOROLA */ );
int makeTrameEcrModBusFromInt  ( int address, int codeFunction, int startAddress, int           value   , char* trame, EModBusProcessorCompatibility /* = INTEL ou MOTOROLA */ );
int makeTrameEcrModBusFromFloat( int address, int codeFunction, int startAddress, float         value   , char* trame, EModBusProcessorCompatibility /* = INTEL ou MOTOROLA */ );
int makeTrameEcrModBusFromShortTab( int address, int codeFunction, int startAddress, short*		values  , int nbMots,   char* trame, EModBusProcessorCompatibility /* = INTEL ou MOTOROLA */ );
int makeTrameEcrModBusFromIntTab  ( int address, int codeFunction, int startAddress, int*		values  , int nbValues, char* trame, EModBusProcessorCompatibility /* = INTEL ou MOTOROLA */ );
int makeTrameEcrModBusFromFloatTab( int address, int codeFunction, int startAddress, float*		values  , int nbValues, char* trame, EModBusProcessorCompatibility /* = INTEL ou MOTOROLA */ );

ErrorComm parseTrameModBus(char* trame, int lengthTrameReceive, char* value, int* nbValues, int* address, int* codeFunction, EModBusProcessorCompatibility processorCompatibility /* = INTEL */);

float			ModBusFloatAsciiToIeee( char* ascii, EModBusProcessorCompatibility);
int				ModBusIntAsciiToIeee( char* ascii, EModBusProcessorCompatibility);
short			ModBusShortAsciiToIeee( char* ascii, EModBusProcessorCompatibility);
unsigned short	ModBusUShortAsciiToIeee( char* ascii, EModBusProcessorCompatibility);
char			ModBusCharAsciiToIeee( char* ascii, EModBusProcessorCompatibility);

void  ModBusCalculCRCStd(BOOL);

// pour TCPIP/modbus
//char* ModbusSerialToTCPIP(char* trameIn, EModBusProcessorCompatibility processorCompatibility /* = INTEL */ );
int ModbusSerialToTCPIP(char* trameIn, int lengthTrameIn, EModBusProcessorCompatibility processorCompatibility /* = INTEL */ );
int ModbusTCPIPToSerial(char* trameIn, int lengthTrameIn, EModBusProcessorCompatibility processorCompatibility /* = INTEL */ );



#endif
