// modbusTP.c

#include <string.h>

#include "debug.h"
#include "Modbus.h"
#include "E2704.h"

typedef struct E2704_config
{
	int port;
	int baud;
	int bits;
	int bit_parity;
	int bit_stop;
} T_E2704_config ;


int GlobaleAdresseRegulatorModbus = 1;

void printState(ErrorComm codret)
{
	switch(codret)
	{
		case ERRORCOMM_BCC:{
			printf("\nError BCC\n");
		break;}

		case ERRORCOMM_TIMEOUT:{
			printf("\nError Timeout\n");
		break;}

		case ERRORCOMM_NOERROR:{
			printf("\nNo error\n");
		break;}

		case ERRORCOMM_ERROR:{
			printf("\nError\n");
		break;}

		default:{
			printf("\nError\n");
		break;}
	}
	return;
}

HANDLE connectionSerialPort()
{
    BOOL connexionOk = FALSE;
    HANDLE handleSerialPort = NULL;

    // A COMPLETER
	printDebug("connectionSerialPort", "");

	T_E2704_config config = {2, 9600, 8, 0, 0};
	/*
	config.port = 2;
	config.baud = 9600;
	config.bits = 8;
	config.bit_parity = 0;
	config.bit_stop = 0;
	*/
/*
	printf("Entrer le numero de port : ");
	scanf("%d", &config.port); //2

	puts("*********** Parametrage du port serie ***********");
	printf("Entrer la vitesse de transmission : ");
	scanf("%d", &config.baud); //9600

	printf("Entrer le nombre de bits de donnees? (5-8) : ");
	scanf("%d", &config.bits);	//8

	printf("Entrer la parite? 0 (pas de parite) / 1 (Parite impair) / 2 (Partie pair) : ");
	scanf("%d", &config.bit_parity); //0

	printf("Entrer le nombre de bits de stop? 0 (1 bit) / 1 (1.5 bits) / 2 (2 bits) : ");
	scanf("%d", &config.bit_stop); //0
*/
	printf("recap port: COM%d, speed=%d, bits=%d, parity=%d, stop=%d\n", config.port, config.baud, config.bits, config.bit_parity, config.bit_stop);

	handleSerialPort = createSerialPort(config.port);

	BOOL success = setParamSerialPort(handleSerialPort, config.baud, config.bits, config.bit_parity, config.bit_stop);
	if (success != TRUE) {
		printDebug("connectionSerialPort", "Com ERROR");
		puts("Verifier que le port n'est pas utilisé.");
		terminateSerialPort(handleSerialPort);
		handleSerialPort = NULL;
	} else
		printDebug("connectionSerialPort", "Com OK");

    return handleSerialPort;
}

int createRequestTrame(TypeRequest i_requestType, char* i_trameSend, TypeVal* i_typeVal)
{
	int lengthTrameSend = 0;
	int startAddress = 0;
	int nbParamsToread = 0;
	int address_slave = 1;  // 1 car liason serie (il n'y en a qu'un seul)

	switch(i_requestType)
	{
		// Demande de lecture:
		case REQUEST_READ:
		{
			printf("\n DEMANDE DE LECTURE\n");

			printf("A partir de quelle adresse souhaitez-vous lire?\n");
            scanf("%d", &startAddress);

			printf("Quel type de parametre voulez-vous lire? 0 (short) / 1 (int) / 2 (float)\n");
            scanf("%d", i_typeVal);

            // A COMPLETER (fait)
			printDebug("createRequestTrame", "LECTURE");

			if (*i_typeVal == TYPE_SHORT) nbParamsToread = 1;
			else if (*i_typeVal == TYPE_INT || *i_typeVal == TYPE_FLOAT) nbParamsToread = 2;
			
			lengthTrameSend = makeTrameLecModBus(address_slave, MODBUS_FUNCTION_READ_NWORDS, startAddress, nbParamsToread, i_trameSend, INTEL);

			break;
		}
		// Demande d'ecriture
		case REQUEST_WRITE:
		{
			printf("\n DEMANDE D'ECRITURE\n");

			printf("A partir de quelle adresse souhaitez-vous ecrire?\n");
            scanf("%d", &startAddress);
			printf("Quel type de parametre voulez-vous ecrire? 0 (short) / 1 (int) / 2 (float)\n");
            scanf("%d", i_typeVal);
			printf("Entre la valeur a ecrire?\n");

            // A COMPLETER (fait)
			printDebug("createRequestTrame", "ECRITURE");

			switch (*i_typeVal)
			{
			case TYPE_SHORT:
			// TODO Error ici
					short valueSHORT;
					scanf("%hd", &valueSHORT);
					printDebug("createRequestTrame", "ECRITURE SHORT");
					lengthTrameSend = makeTrameEcrModBusFromShort(address_slave, MODBUS_FUNCTION_WRITE_WORD, startAddress, valueSHORT, i_trameSend, INTEL);
				break;
			case TYPE_INT:
					int valueINT;
					scanf("%d", &valueINT);
					printDebug("createRequestTrame", "INT");
					lengthTrameSend = makeTrameEcrModBusFromInt(address_slave, MODBUS_FUNCTION_WRITE_WORDS, startAddress, valueINT, i_trameSend, INTEL);
				break;
			case TYPE_FLOAT:
					float valueFLOAT;
					scanf("%f", &valueFLOAT);
					printDebug("createRequestTrame", "FLOAT");
					lengthTrameSend = makeTrameEcrModBusFromFloat(address_slave, MODBUS_FUNCTION_WRITE_WORDS, startAddress, valueFLOAT, i_trameSend, INTEL);
				break;
			}
			break;
		}
	}
	return lengthTrameSend;
}

ErrorComm parseModbusResponse(char* i_trameReceive, int i_lengthTrameReceived, TypeRequest i_requestType, TypeVal i_typeVal)
{
	ErrorComm codret = ERRORCOMM_ERROR;
	
    // A COMPLETER (fait)
	printDebug("parseModbusResponse", "");

	int address = 1;
	char valuec[100];
	int nbValue;

	int codeFunction;

    codret = parseTrameModBus(i_trameReceive, i_lengthTrameReceived, valuec, &nbValue, &address, &codeFunction, INTEL);
	printState(codret);

	printf("code function: %d\n", codeFunction);

	switch (i_typeVal)
	{
	case TYPE_SHORT:
			short valueSHORT = ModBusShortAsciiToIeee(valuec, INTEL);
			printf("  Value: %d\n", valueSHORT);
		break;
	case TYPE_INT:
			int valueINT = ModBusIntAsciiToIeee(valuec, INTEL);
			printf("  Value: %d\n", valueINT);
		break;
	case TYPE_FLOAT:
			float valueFLOAT = ModBusFloatAsciiToIeee(valuec, INTEL);
			printf("  Value: %.2f\n", valueFLOAT);
		break;
	}
	return codret;
}

void printTrame(char * type, char trame[100], int lengthTrame){
	printf("\n %s trame (length = %i):", type, lengthTrame);
    for (int i = 0; i < lengthTrame; i++)
        printf("%02X ", ((unsigned char)trame[i]));
    printf("\n");
}