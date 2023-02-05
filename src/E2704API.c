#include "E2704API.h"

// -- List --

/**
 * @brief initialize list
 * 
 * @return t_E2704_parameter_list* list
 */
t_E2704_parameter_list *initParameter(void){
    t_E2704_parameter_list *paramList = malloc(sizeof(t_E2704_parameter_list));
    paramList->parameterList = NULL;
    paramList->num_params = 0;
    paramList->len_param = 0;
    paramList->len_row_ch = 5;
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
    paramList->num_params = 0;
    free(paramList);
    paramList = NULL;
}


/**
 * @brief Get the Value object
 * 
 * @param paramList 
 * @param channel 
 */
/*
void getValue(t_E2704_parameter_list *paramList, E2704_Channel channel){
    // CH1: off=0, CH2: off=1024, CH3: off=2048
    int offsetAddress = (channel - 1) * 1024;

    t_E2704_parameter *current = paramList->parameterList;
    while (current != NULL)
    {
        // Ask & Retrive info
        int value = rand() % 100; // simulation

        current->value = value;

        t_E2704_parameter *next = current->next;
        current = next;
    }
}
*/

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

    printLine(paramList->len_param + 2, 0, "┌");
    printf("│ Parameters\n");
    printLine(paramList->len_param + 2, 0, "├");

    while (params != NULL)
    {
        printf("│ %s\n", params->name);
        params = params->next;
    }

    printLine(paramList->len_param + 2, 0, "└");
}

/**
 * @brief print a channel
 *
 * @param paramList list
 * @param channel selected channel
 */
void printChannel(t_E2704_parameter_list *paramList, E2704_Channel channel)
{
    clearChannel(paramList, channel);
    t_E2704_parameter *params = paramList->parameterList;

    printf("\033[%dA", paramList->num_params + 4);

    int offset = paramList->len_param + 3 + ((channel - 1) * 8);

    printLine(paramList->len_row_ch + 2, offset, "┬");
    printf("\033[%dC│ CH%d\n", offset, channel);
    printLine(paramList->len_row_ch + 2, offset, "┼");

    while (params != NULL)
    {
        printf("\033[%dC│ %5d\n", offset, params->value);
        params = params->next;
    }
    printLine(paramList->len_row_ch + 2, offset, "┴");
}

/**
 * @brief clear value writhen for a channel
 *
 * @param paramList list
 * @param channel selected channel
 */
void clearChannel(t_E2704_parameter_list *paramList, E2704_Channel channel){
    t_E2704_parameter *params = paramList->parameterList;

    printf("\033[%dA", paramList->num_params + 2);

    int offset = paramList->len_param + 3 + ((channel - 1) * 8);

    char *blank = (char *)malloc(paramList->len_row_ch * sizeof(char));
    memset(blank, ' ', paramList->len_row_ch);

    printLine(paramList->len_row_ch + 2, offset, "┼");

    while (params != NULL)
    {
        printf("\033[%dC│ %s\n", offset, blank);
        params = params->next;
    }
    printLine(paramList->len_row_ch + 2, offset, "┴");
}

/**
 * @brief print delimitaion like "──────"
 *
 * @param len length
 * @param offset offset from the right edge
 * @param firstChar first char of the ligne
 */
void printLine(const int len, const int offset, char *firstChar){
    if (offset != 0)
        printf("\033[%dC%s", offset, firstChar);
    else
        printf("%s", firstChar);
    for (int i = 0; i < len; i++)
        printf("─");
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

    printf("\033[%dC┐\n", offset);
    printf("\033[%dC│\n", offset);
    printf("\033[%dC┤\n", offset);

    while (params != NULL)
    {
        printf("\033[%dC│\n", offset);
        params = params->next;
    }
    printf("\033[%dC┘\n", offset);
}