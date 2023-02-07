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
        printf("%s (%d)\n", current->name, current->address);
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


/**
 * @brief Get the Value object, clear, and print new value
 * 
 * @param hPort handle port comm
 * @param paramList list
 * @param channel selected channel
 * @return ErrorComm communication state
 */
ErrorComm getValue(HANDLE hPort, t_E2704_parameter_list *paramList, E2704_Channel channel){
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

// -- Print --

/**
 * @brief print the first row with paramaters
 *
 * @param paramList list
 */
void printParameterRow(t_E2704_parameter_list *paramList)
{
    t_E2704_parameter *params = paramList->parameterList;
    // int len = maxCharRow(params);

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
    //clearChannel(paramList, channel);
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