#include <string.h>

#include <conio.h> // keybord interruption
#include <time.h>  // clock()

#include "debug.h"
#include "Modbus.h"
#include "E2704.h"
#include "E2704API.h"

/**
 * @brief main for the E2704 visualisator
 * 
 * @param hPort serial communication
 */
void E2704_main(HANDLE hPort)
{
	// ask & set command to send to the regulator
	t_E2704_parameter_list *paramListWrite = initParameterList();
	E2704_setParametersWrite(paramListWrite);
	int max_channel = E2704_setServiceUser(hPort, paramListWrite);

	// Init & set parameters to read
	t_E2704_parameter_list *paramListRead = initParameterList();
	E2704_setParametersRead(paramListRead);

	printf("\n\tPress 'q' to quit program.\n\tExecute .\\Mod_E2704 -h for help.\n\n");

	// Print table & legend
	printParameterRow(paramListRead);
    printEnd(paramListRead, max_channel);

	// Print channels
	for (int channel = 1; channel <= max_channel; channel++)
		printChannel(paramListRead, channel);

	// While, print each values
	BOOL requestExit = FALSE;

	clock_t begin, end;
	begin = clock();
	while (1)
	{
		// Get new tick
		end = clock();
		// If total tick > 1s, update & execute
		if ((end - begin) >= 1000)
		{
			begin = end;

			// Get & print data for each channel
			for (int channel = 1; channel <= max_channel; channel++) {
				if(E2704_getValue(hPort, paramListRead, channel) == ERRORCOMM_INTERRUPT){
					requestExit = TRUE;
					break;
				}
			}
			if(requestExit == TRUE) break;
		}

		// Keybord interruption
		if (kbhit())
		{
			char key = getch();
			if (key == 'q')
				break;
		}

		// Wait 10ms to avoid over speed processor
		Sleep(10);
	}

	// Free alocated elements in memory
	freeList(paramListRead);
	freeList(paramListWrite);
}

/**
 * @brief Ask to user in wich mode the regulator is needed
 * 
 * @param regulation_mode return: 0 (Automatic) / 1 (Manual)
 * @param consigne return:
 * 				- value of the puissance (for automatic)
 *              - value of temperature (for manual)
 */
void E2704_ask_service(E2704_RegulationMode *regulation_mode, short *consigne)
{
	char buffer[10];
	short input;

	printf("-----------------------------------------------\n");
	printf("Type de regulation? 0 (automatic) / 1 (manual): ");
	while (1)
	{
		fgets(buffer, sizeof(buffer), stdin);
		sscanf(buffer, "%d", &input);
		
		// Check consigne
		if (input == E2704_MODE_AUTO)
		{
			printf("Consigne de temperature:                      : ");
			fgets(buffer, sizeof(buffer), stdin);
			sscanf(buffer, "%d", consigne);
			break;
		}
		else if (input == E2704_MODE_MANUAL)
		{
			printf("Consigne de puissance:                        : ");
			fgets(buffer, sizeof(buffer), stdin);
			sscanf(buffer, "%d", consigne);
			break;
		}
		else
			printf("Invalid mode                                  : ");
	}
	*regulation_mode = input;
}

/**
 * @brief set serial port communication,
 * if json config file exist, set the connection, 
 * else, set connection by user
 * 
 * @return HANDLE 
 */
HANDLE connectionSerialPort()
{
	BOOL connexionOk = FALSE;
	HANDLE handleSerialPort = NULL;

	printDebug("connectionSerialPort", "");
	t_E2704_config config = {0}; // = {2, 9600, 8, 0, 0};

	// Read config file if exist, else ask to the user
	if(config_file_exist(F_CONFIG_SERIAL) == TRUE){
		config = E2704_getSerialPortConfig(F_CONFIG_SERIAL);
	} else {
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
	}
	printf("Connection: COM%d, baud=%d, bits=%d, parity=%d, stop=%d\n", config.port, config.baud, config.bits, config.bit_parity, config.bit_stop);

	handleSerialPort = createSerialPort(config.port);
	connexionOk = setParamSerialPort(handleSerialPort, config.baud, config.bits, config.bit_parity, config.bit_stop);

	if (connexionOk != TRUE)
	{
		printDebug("connectionSerialPort", "Com ERROR");
		puts("Verify that the port is plugged in and not in use.");
		terminateSerialPort(handleSerialPort);
		handleSerialPort = NULL;
	}
	else
	{
		printDebug("connectionSerialPort", "Com OK");
		puts("Connection Established.");
	}
	return handleSerialPort;
}

// -- Debug --

void E2704_debug(HANDLE hPort)
{
	TypeRequest requestType = NO_REQUEST;
	TypeVal typeVal = NO_TYPE;

	while (requestType != REQUEST_QUIT)
	{
		char trameToSend[100];
		int lengthTrameToSend = 0;
		char trameReceived[100];
		int lengthTrameReceived = 99;
		memset(trameReceived, '\0', sizeof(trameReceived));

		ErrorComm codret = ERRORCOMM_ERROR;

		printf("\n****************************************************************************\n");
		printf("\t1. Demande de lecture.\n");
		printf("\t2. Demande d'ecriture.\n");
		printf("\t3. Quitter.\n");
		printf("Que souhaitez-vous faire?\n");
		scanf("%d", &requestType);

		//*******************************************************************************
		// Creation de la trame de requete Modbus
		if (requestType == REQUEST_READ || requestType == REQUEST_WRITE)
			lengthTrameToSend = createRequestTrame(requestType, trameToSend, &typeVal);
		else
			continue;

		printTrame("Send", trameToSend, lengthTrameToSend);

		//*******************************************************************************
		// Envoie de la requete Modbus sur le supporte de communication et reception de la trame reponse
		if (lengthTrameToSend)
		{
			// A COMPLETER (fait)
			printDebug("main", "set request modbus");
			codret = sendAndReceiveSerialPort(hPort, INFINITE, trameToSend, lengthTrameToSend, trameReceived, &lengthTrameReceived);

			printf("code: %d, lengthTrameReceived: %d", codret, lengthTrameReceived);
		}

		//*******************************************************************************
		// Decodage de la trame reçue
		if (codret != ERRORCOMM_NOERROR || lengthTrameReceived == 0)
			printState(codret);
		else
		{
			printTrame("Receive", trameReceived, lengthTrameReceived);

			codret = parseModbusResponse(trameReceived, lengthTrameReceived, requestType, typeVal);
			if (codret != ERRORCOMM_NOERROR)
				printState(codret);
			//*******************************************************************************
		}
	}
}

/**
 * @brief Create a Request Trame
 * 
 * @param i_requestType type of the request: READ, WRITE
 * @param i_trameSend trame to send
 * @param i_typeVal type of val: short, int, float
 * @return int length of the trame send
 */
int createRequestTrame(TypeRequest i_requestType, char *i_trameSend, TypeVal *i_typeVal)
{
	int lengthTrameSend = 0;
	int startAddress = 0;
	int nbParamsToread = 0;
	int address_slave = 1; // 1 car liason serie (il n'y en a qu'un seul)

	switch (i_requestType)
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

		if (*i_typeVal == TYPE_SHORT)
			nbParamsToread = 1;
		else if (*i_typeVal == TYPE_INT || *i_typeVal == TYPE_FLOAT)
			nbParamsToread = 2;

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

/**
 * @brief transform data revieved in correct values
 * TP3
 * Used for debug
 * 
 * @param i_trameReceive trame recieved
 * @param i_lengthTrameReceived length
 * @param i_requestType type of the request: READ or WRITE
 * @param i_typeVal short, int, float 
 * @return ErrorComm: state of the communication
 */
ErrorComm parseModbusResponse(char *i_trameReceive, int i_lengthTrameReceived, TypeRequest i_requestType, TypeVal i_typeVal)
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

/**
 * @brief Print the state of the connection
 * 
 * @param codret code state
 */
void printState(ErrorComm codret)
{
	switch (codret)
	{
	case ERRORCOMM_BCC:
	{
		printf("\nError BCC\n");
		break;
	}

	case ERRORCOMM_TIMEOUT:
	{
		printf("\nError Timeout\n");
		break;
	}

	case ERRORCOMM_NOERROR:
	{
		printf("\nNo error\n");
		break;
	}

	case ERRORCOMM_ERROR:
	{
		printf("\nError\n");
		break;
	}

	default:
	{
		printf("\nError\n");
		break;
	}
	}
	return;
}

/**
 * @brief Print a trame in hexadecimal
 * 
 * @param type type of trame: recieve, send
 * @param trame actual trame
 * @param lengthTrame length
 */
void printTrame(char *type, char trame[100], int lengthTrame)
{
	printf("%s trame (length = %i):", type, lengthTrame);
	for (int i = 0; i < lengthTrame; i++)
		printf("%02X ", ((unsigned char)trame[i]));
	printf("\n");
}