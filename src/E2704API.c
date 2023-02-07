#include "E2704API.h"

// -- List --

/**
 * @brief initialize list
 * 
 * @return t_E2704_parameter_list* list
 */
t_E2704_parameter_list *initParameterList(void){
    t_E2704_parameter_list *paramList = malloc(sizeof(t_E2704_parameter_list));
    paramList->parameterList = NULL;
    paramList->num_params = 0;
    paramList->len_param = 0;
    paramList->len_row_ch = 5;

    char *blank = (char *)malloc((paramList->len_row_ch+1) * sizeof(char));
    for (int i = 0; i < paramList->len_row_ch; i++)
        blank[i] = ' ';
    blank[paramList->len_row_ch] = '\0';

    paramList->blank = blank;

    return paramList;
}

/**
 * @brief create new parameter
 *
 * @param paramName parameter name
 * @param address E2704 parameter address
 * @return t_E2704_parameter*
 */
t_E2704_parameter *newParameter(char *paramName, int address)
{
    t_E2704_parameter *param;
    param = (t_E2704_parameter *)malloc(sizeof(t_E2704_parameter));
    if (param != NULL)
    {
        param->name = strdup(paramName);
        param->address = address;
        param->value = 0;
        param->next = NULL;
    }
    return param;
}

/**
 * @brief Set the Parameter Value object
 * 
 * @param paramList list
 * @param paramName parameter name to put the value
 * @param value value of the parameter
 */
void setParameterValue(t_E2704_parameter_list *paramList, char *paramName, short value){   
    t_E2704_parameter *params = paramList->parameterList;

	while (params != NULL)
	{
		if(strcasecmp(params->name, paramName) == 0){
			params->value = value;
            break;
        }
		params = params->next;
	}
}

/**
 * @brief Add new parameter at the end of the list
 *
 * @param paramList list
 * @param paramName parameter name
 * @param address parameter address
 */
void addParameter(t_E2704_parameter_list *paramList, char *paramName, int address)
{
    // Create new parameter
    t_E2704_parameter *param = newParameter(paramName, address);

    // If list empty
    if (paramList->parameterList == NULL)
    {
        paramList->parameterList = param;
        paramList->num_params++;
        paramList->len_param = strlen(param->name);
    }
    // Add at the end of the list
    else
    {
        t_E2704_parameter *it = paramList->parameterList;
        paramList->num_params++;

        int len = strlen(param->name);
        if (len > paramList->len_param)
            paramList->len_param = len;

        // Add at the end
        for (; it->next != NULL; it = it->next)
            ;
        it->next = param;
        param->next = NULL;
    }
}

/**
 * @brief print content of the list
 *
 * @param paramList list
 */
void printList(t_E2704_parameter_list *paramList)
{
    if (paramList->num_params == 0)
        return;
    printf("Number of parameters = %d:\n\n", paramList->num_params);
    t_E2704_parameter *current = paramList->parameterList;
    if (current == NULL)
        return;
    while (current != NULL)
    {
        printf("%s\t(%d)\t(%d)\n", current->name, current->address, current->value);
        current = current->next;
    }
}

/**
 * @brief free list from memory
 *
 * @param paramList list
 */
void freeList(t_E2704_parameter_list *paramList)
{
    t_E2704_parameter *current = paramList->parameterList;
    while (current != NULL)
    {
        t_E2704_parameter *next = current->next;
        free(current->name);
        free(current);
        current = next;
    }
    free(paramList->blank);
    paramList->num_params = 0;
    free(paramList);
    paramList = NULL;
}

// -- E2704 function --

/**
 * @brief Get the Value object, clear, and print new value
 * 
 * @param hPort handle port comm
 * @param paramList list
 * @param channel selected channel
 * @return ErrorComm communication state
 */
ErrorComm E2704_getValue(HANDLE hPort, t_E2704_parameter_list *paramList, E2704_Channel channel){
    static short value = 0;
    // CH1: off=0, CH2: off=1024, CH3: off=2048
    int offsetAddress = (channel - 1) * 1024;

    t_E2704_parameter *current = paramList->parameterList;
    while (current != NULL)
    {
        // Ask & Retrive info
        //int value = rand() % 100; // simulation
        current->value = E2704_read(hPort, current->address + offsetAddress);

        // If the return is a negative ERRORCOMM_INTERRUPT, exit
        if(current->value == -ERRORCOMM_INTERRUPT) return ERRORCOMM_INTERRUPT;

        // Else, clear value and write new value.
        printValueChannel(paramList, current, channel);

        t_E2704_parameter *next = current->next;
        current = next;
    }
}

/**
 * @brief write desired value of selected parameter
 * 
 * @param hPort handle port comm
 * @param paramList list of parameter
 * @param paramaterName parameter name to select 
 * @param channel selected channel
 */
void E2704_write_consigne(HANDLE hPort, t_E2704_parameter_list *paramList, char *paramName, E2704_Channel channel){
    t_E2704_parameter *params = paramList->parameterList;

	int offsetAddress = (channel - 1) * 1024;

	while (params != NULL)
	{
		if(strcasecmp(params->name, paramName) == 0){
			E2704_write(hPort, params->value, params->address + offsetAddress);
            break;
        }
		params = params->next;
	}
}

/**
 * @brief 
 * 
 * @param hPort handle port comm
 * @param paramList list of parameter
 * @param mode Auto or Manual
 * @param consigne power or temperature 
 * @param channel selected channel
 */
void E2704_setConsigne(HANDLE hPort, t_E2704_parameter_list *paramList, E2704_RegulationMode mode, short consigne, E2704_Channel channel){
    setParameterValue(paramList, "Regulation Mode", (short) mode);
	E2704_write_consigne(hPort, paramList, "Regulation Mode", channel);

	if(mode == E2704_MODE_AUTO){
        setParameterValue(paramList, "Target Set Point", consigne);
        E2704_write_consigne(hPort, paramList, "Target Set Point", channel);
        setParameterValue(paramList, "Output Power", 0);
        E2704_write_consigne(hPort, paramList, "Output Power", channel);
    } else if (mode = E2704_MODE_MANUAL) {
        setParameterValue(paramList, "Output Power", consigne);
        E2704_write_consigne(hPort, paramList, "Output Power", channel);
        setParameterValue(paramList, "Target Set Point", 0);
        E2704_write_consigne(hPort, paramList, "Target Set Point", channel);
    }
}

/**
 * @brief set parameter automaticly or in read json file named
 *  "config_parameter_to_read.json" 
 * 
 * @param paramList the list filled of parameters 
 */
void E2704_setParametersRead(t_E2704_parameter_list *paramList){
    if(config_file_exist(F_CONFIG_PARAM_R)){
        E2704_getParameterConfig(F_CONFIG_PARAM_R, paramList);
    } else {
        addParameter(paramList, "Measured Value (PV)", 1);
        addParameter(paramList, "Set Point (SP)", 5);
        addParameter(paramList, "Regulation Mode", 273);
        addParameter(paramList, "OutPut Power", 3);
        addParameter(paramList, "P", 351);
        addParameter(paramList, "I", 352);
        addParameter(paramList, "D", 353);
    }
}

/**
 * @brief set parameter automaticly or in write json file named
 *  "config_parameter_to_write.json" 
 * 
 * @param paramList the list filled of parameters 
 */
void E2704_setParametersWrite(t_E2704_parameter_list *paramList){
    if(config_file_exist(F_CONFIG_PARAM_W)){
        E2704_getParameterConfig(F_CONFIG_PARAM_W, paramList);
    } else {
        addParameter(paramList, "Regulation Mode", 273);
        addParameter(paramList, "Target Set Point", 2);
	    addParameter(paramList, "Output Power", 3);
    }
}

/**
 * @brief set the consignes and mode for each channel
 * 
 * @param hPort handle connection
 * @param paramList list of parameter
 * @return int number of channels
 */
int E2704_setServiceUser(HANDLE hPort, t_E2704_parameter_list *paramList){
    if(config_file_exist(F_CONFIG_CONSIGNE)){
        return E2704_getConsigneConfig(hPort, paramList, F_CONFIG_CONSIGNE);
    } 

    E2704_RegulationMode mode = E2704_MODE_MANUAL;
	short consigne = 10;
    int max_channel = 1;
    
    // Ask number of channels
	char buffer[3];
	printf("Nombre de channel: ");
	fgets(buffer, sizeof(buffer), stdin);
	sscanf(buffer, "%d", &max_channel);

	for (int channel = 1; channel <= max_channel; channel++){
		// Ask user modes & consigne
		E2704_ask_service(&mode, &consigne);
		
		// Send consigne & mode to E2704
		E2704_setConsigne(hPort, paramList, mode, consigne, channel);
	}
    return max_channel;
}

/**
 * @brief create and write a value at the address
 * 
 * @param hPort serial communication
 * @param data value to write
 * @param address at this address
 */
void E2704_write(HANDLE hPort, short data, int address){
	char trameReceived[100];
	int lengthTrameReceived = 99;
	memset(trameReceived, '\0', sizeof(trameReceived));

	ErrorComm codret = E2704_sendRequest(hPort, REQUEST_WRITE, data, address, trameReceived, &lengthTrameReceived);
	if (codret != ERRORCOMM_NOERROR) printState(codret);
}

/**
 * @brief create and read a value at address 
 * 
 * @param hPort serial connection
 * @param _address address to read
 * @return short value at the address
 */
short E2704_read(HANDLE hPort, int _address){
	char trameReceived[100];
	int lengthTrameReceived = 99;
	memset(trameReceived, '\0', sizeof(trameReceived));

	ErrorComm codret = E2704_sendRequest(hPort, REQUEST_READ, 0, _address, trameReceived, &lengthTrameReceived);
	if (codret != ERRORCOMM_NOERROR) /*printState(codret);*/ return codret * -1;

	// Decodage de la trame reçue
	int address = 1;
	char valuec[100];
	int nbValue;
	int codeFunction;

	if (codret != ERRORCOMM_NOERROR || lengthTrameReceived == 0) /*printState(codret)*/ return codret * -1;
	else {
		//printTrame("Receive", trameReceived, lengthTrameReceived);
		//codret = parseModbusResponse(trameReceived, lengthTrameReceived, REQUEST_READ, TYPE_SHORT);
		codret = parseTrameModBus(trameReceived, lengthTrameReceived, valuec, &nbValue, &address, &codeFunction, INTEL);
		if (codret != ERRORCOMM_NOERROR) /*printState(codret);*/ return codret * -1;
	}
	return ModBusShortAsciiToIeee(valuec, INTEL);
}

/**
 * @brief sent a request trame
 * 
 * @param hPort serial communication
 * @param requestType READ or WRITE
 * @param data value
 * @param address at this address
 * @param trameOut result trame
 * @param lenTrameOut length of the result trame
 * @return ErrorComm communication state
 */
ErrorComm E2704_sendRequest(HANDLE hPort, TypeRequest requestType, short data, int address, char *trameOut, int *lenTrameOut){
	char trameToSend[100];
	int lengthTrameToSend = 0;

	ErrorComm codret = ERRORCOMM_ERROR;

	// Creation de la trame de requete Modbus
	lengthTrameToSend = E2704_createRequestTrame(requestType, trameToSend, data, address);

	//printTrame("Send", trameToSend, lengthTrameToSend);
	// Envoie de la requete Modbus sur le supporte de communication et reception de la trame reponse
	if (lengthTrameToSend)
		codret = sendAndReceiveSerialPort(hPort, INFINITE, trameToSend, lengthTrameToSend, trameOut, lenTrameOut);
	if (lengthTrameToSend == 0)
		codret = ERRORCOMM_ERROR;
}

/**
 * @brief create a trame of request
 * 
 * @param i_requestType type: READ or WRITE
 * @param i_trameSend trame to send
 * @param value value
 * @param address address
 * @return int length of the trame sended
 */
int E2704_createRequestTrame(TypeRequest i_requestType, char *i_trameSend, short value, int address)
{
	int address_slave = 1; // 1 car liason serie (il n'y en a qu'un seul)
	int lengthTrameSend = 0;

	if (i_requestType == REQUEST_READ)
		lengthTrameSend = makeTrameLecModBus(address_slave, MODBUS_FUNCTION_READ_NWORDS, address, 1, i_trameSend, INTEL);
	else if (i_requestType == REQUEST_WRITE)
		lengthTrameSend = makeTrameEcrModBusFromShort(address_slave, MODBUS_FUNCTION_WRITE_WORD, address, value, i_trameSend, INTEL);

	return lengthTrameSend;
}


// -- Print --

/**
 * @brief print the first row with paramaters
 *
 * @param paramList list
 */
void printParameterRow(t_E2704_parameter_list *paramList)
{
    t_E2704_parameter *params = paramList->parameterList;

    printLine(paramList->len_param + 2, 0, CORNER_T_L);
    printf("%c Parameters\n", LINE_H);
    printLine(paramList->len_param + 2, 0, LINE_V_R);

    while (params != NULL)
    {
        printf("%c %s\n", LINE_H, params->name);
        params = params->next;
    }

    printLine(paramList->len_param + 2, 0, CORNER_B_L);
}

/**
 * @brief print a channel
 *
 * @param paramList list
 * @param channel selected channel
 */
void printChannel(t_E2704_parameter_list *paramList, E2704_Channel channel)
{
    t_E2704_parameter *params = paramList->parameterList;

    printf("\033[%dA", paramList->num_params + 4);

    int offset = paramList->len_param + 3 + ((channel - 1) * 8);

    printLine(paramList->len_row_ch + 2, offset, LINE_H_D);
    printf("\033[%dC%c CH%d\n", offset, LINE_H, channel);
    printLine(paramList->len_row_ch + 2, offset, LINE_V_H);

    while (params != NULL)
    {
        printf("\033[%dC%c %5d\n", offset, LINE_H, params->value);
        params = params->next;
    }
    printLine(paramList->len_row_ch + 2, offset, LINE_H_U);
}

/**
 * @brief print a specific value from a parameter in a channel
 * 
 * @param paramList list
 * @param parameter paramater to print
 * @param channel channel
 */
void printValueChannel(t_E2704_parameter_list *paramList, t_E2704_parameter *parameter, E2704_Channel channel){    
    t_E2704_parameter *params = paramList->parameterList;
    
    // Determine vertical position 
    int parameter_pos = paramList->num_params;
    while (params != NULL)
    {
        if(params->address == parameter->address) break;
        parameter_pos --;
        params = params->next;
    }

    int offset = paramList->len_param + 3 + ((channel - 1) * 8);

    // Position cursor to the ligne
    printf("\033[%dA", parameter_pos + 1);
    // Clear
    printf("\033[%dC%c %s", offset, LINE_H, paramList->blank);
    // Return to the origin
    printf("\033[%dD", offset + strlen(paramList->blank) + 2);
    // Print
    printf("\033[%dC%c %5d\n", offset, LINE_H, parameter->value);
    // Reset position cursor
    printf("\033[%dB", parameter_pos + 1);
}

/**
 * @brief clear value writhen for a channel (not used)
 *
 * @param paramList list
 * @param channel selected channel
 */
void clearChannel(t_E2704_parameter_list *paramList, E2704_Channel channel){
    t_E2704_parameter *params = paramList->parameterList;

    printf("\033[%dA", paramList->num_params + 2);

    int offset = paramList->len_param + 3 + ((channel - 1) * 8);

    printLine(paramList->len_row_ch + 2, offset, LINE_V_H);

    while (params != NULL)
    {
        printf("\033[%dC%c %s\n", offset, LINE_H, paramList->blank);
        params = params->next;
    }
    printLine(paramList->len_row_ch + 2, offset, LINE_H_U);
}

/**
 * @brief print delimitaion like "──────"
 *
 * @param len length
 * @param offset offset from the right edge
 * @param firstChar first char of the ligne
 */
void printLine(const int len, const int offset, char firstChar){
    if (offset != 0)
        printf("\033[%dC%c", offset, firstChar);
    else
        printf("%c", firstChar);
    for (int i = 0; i < len; i++)
        printf("%c", LINE_V);
    printf("\n");
}

/**
 * @brief close with line the end of the array
 * 
 * @param paramList list
 * @param lastChannel last channel
 */
void printEnd(t_E2704_parameter_list *paramList, E2704_Channel lastChannel)
{
    t_E2704_parameter *params = paramList->parameterList;

    printf("\033[%dA", paramList->num_params + 4);

    int offset = paramList->len_param + 3 + (lastChannel * 8);

    printf("\033[%dC%c\n", offset, CORNER_T_R);
    printf("\033[%dC%c\n", offset, LINE_H);
    printf("\033[%dC%c\n", offset, LINE_V_L);

    while (params != NULL)
    {
        printf("\033[%dC%c\n", offset, LINE_H);
        params = params->next;
    }
    printf("\033[%dC%c\n", offset, CORNER_B_R);
}

// -- Files --

/**
 * @brief verify if a config file existe or not
 * 
 * @param configFileName file name
 * @return BOOL 1 if exist, 0 not
 */
BOOL config_file_exist(const char *configFileName){
    // F_OK : check existence
    // R_OK : chech can read
    // W_OK : check can write
    // X_OK : check can execute
    return access(configFileName, F_OK) != -1;
}

/**
 * @brief get value in json file for serial config
 * 
 * @param configFileName file name of the config
 * @return t_E2704_config structure serial configuration
 */
t_E2704_config E2704_getSerialPortConfig(const char *configFileName){
    t_E2704_config config = {0};
    char line[100];

    printf("Read %s\n", configFileName);
    // Open file
    FILE *file = fopen(configFileName, "r");
    if (file == NULL) {
        perror("fopen");
        return config;
    }

    // Read file
    while (fgets(line, sizeof(line), file)) {
        // Remove parasit char
        removeChar(line, ' ');
        removeChar(line, '\t');

        // Split & retieve values
        char *key = strtok(line, ":");
        char *value = strtok(NULL, "\n");
    
        if (value != NULL) {
            value[strlen(value) - 1] = '\0';
        }

        // Get & set values
        if (strcmp(key, "\"port\"") == 0) {
            config.port = atoi(value);
        } else if (strcmp(key, "\"baud\"") == 0) {
            config.baud = atoi(value);
        } else if (strcmp(key, "\"bits\"") == 0) {
            config.bits = (short) atoi(value);
        } else if (strcmp(key, "\"bit_parity\"") == 0) {
            config.bit_parity = (short) atoi(value);
        } else if (strcmp(key, "\"bit_stop\"") == 0) {
            config.bit_stop = (short) atoi(value);
        }
    }

    // Close & return
    fclose(file);
    return config;
}

/**
 * @brief get value in json file named:
 * "config_parameter_to_read.json"
 * 
 * @param configFileName file name of the config
 * @param paramList the list of parameters filled 
 */
void E2704_getParameterConfig(const char *configFileName, t_E2704_parameter_list *paramList){
    char line[100];

    printf("Read %s\n", configFileName);
    // Open file
    FILE *file = fopen(configFileName, "r");
    if (file == NULL) {
        perror("fopen");
        return;
    }

    // Read file
    while (fgets(line, sizeof(line), file)) {
        // Remove parasit char
        removeCharStart(line, ' ');
        removeChar(line, '\t');

        // Split & retieve values
        char *name = strtok(line, ":");
        char *address = strtok(NULL, "\n");

        // If the variable name contain '"' like '"name"', remove this char, else stop this iteration
        if(strchr(name, '"') == NULL) continue;
        removeChar(name, '"');

        // Add end of line
        //if (address != NULL) {
        //    address[strlen(address) - 1] = '\0';
        //}
        //printf("add: %s, %d\n", name, atoi(address));
        addParameter(paramList, name, atoi(address));
    }

    // Close & return
    fclose(file);
}

/**
 * @brief 
 * 
 * @param hPort connection handler
 * @param paramList list of actual parameter
 * @param configFileName name of the file configuration
 * @return int number of channels
 */
int E2704_getConsigneConfig(HANDLE hPort, t_E2704_parameter_list *paramList, const char *configFileName){
    char line[100];
    int nbr_channel = 0;

    printf("Read %s\n", configFileName);
    // Open file
    FILE *file = fopen(configFileName, "r");
    if (file == NULL) {
        perror("fopen");
        return nbr_channel;
    }

    // Read file
    while (fgets(line, sizeof(line), file)) {
        // Remove parasit char
        removeCharStart(line, ' ');
        removeChar(line, '\t');

        // Split & retieve values
        char *channel = strtok(line, ":");
        char *modeStr = strtok(NULL, ",");
        char *consigne = strtok(NULL, "\n");

        // If the variable name contain '"' like '"1"', remove this char, else stop this iteration
        if(strchr(channel, '"') == NULL) continue;
        removeChar(channel, '"');

        // Add end of line
        if (consigne != NULL) {
            consigne[strlen(consigne) - 1] = '\0';
        }

        // Format consigne
        removeChar(modeStr, '[');
        removeChar(modeStr, ' ');
        removeChar(consigne, ']');

        int mode = (strcmp(modeStr, "\"AUTOMATIC\"") == 0) ? E2704_MODE_AUTO : E2704_MODE_MANUAL;
        printf("Add: CH%d, '%s'\t, %d, consigne=%d\n", atoi(channel), modeStr, mode, atoi(consigne));

        // Send consigne & mode to E2704
		E2704_setConsigne(hPort, paramList, mode, atoi(consigne), atoi(channel));
        nbr_channel++;
    }

    // Close & return
    fclose(file);
    return nbr_channel;
}

/**
 * @brief remove a specified char
 * 
 * @param str text
 * @param charToRemove char to remove
 */
void removeChar(char * str, char charToRemove){
    int len = strlen(str);
    for(int i = 0; i < len; i++)
    {
        if(str[i] == charToRemove)
        {
            for(int j = i; j < len; j++)
                str[j] = str[j+1];
            len--;
            i--;
        }
    }
}

/**
 * @brief remove first char from str
 * 
 * @param str text
 * @param charToRemove char to remove
 */
void removeCharStart(char * str, char charToRemove){
    int len = strlen(str);
    for(int i = 0; i < len; i++)
    {
        if(str[i] == charToRemove)
        {
            for(int j = i; j < len; j++)
                str[j] = str[j+1];
            len--;
            i--;
        } else return;
    }
}