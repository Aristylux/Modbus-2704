#ifndef E_API_H
#define E_API_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum E2704_channel
{
    CH1 = 1,
    CH2,
    CH3
} E2704_Channel;

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
} t_E2704_parameter_list;


// -- List --

t_E2704_parameter_list *initParameter(void);
t_E2704_parameter *newParameter(char *paramName, int address);
void addParameter(t_E2704_parameter_list *paramList, char *paramName, int address);
void printList(t_E2704_parameter_list *paramList);
void freeList(t_E2704_parameter_list *paramList);
void getValue(t_E2704_parameter_list *paramList, E2704_Channel channel);

// -- Print --

void printParameterRow(t_E2704_parameter_list *paramList);
void printChannel(t_E2704_parameter_list *paramList, E2704_Channel channel);
void clearChannel(t_E2704_parameter_list *paramList, E2704_Channel channel);
void printLine(const int len, const int offset, char *firstChar);
void printEnd(t_E2704_parameter_list *paramList, E2704_Channel lastChannel);


#endif