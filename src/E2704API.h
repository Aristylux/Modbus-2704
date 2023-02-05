#ifndef E_API_H
#define E_API_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Common.h"
#include "E2704.h"

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

t_E2704_parameter_list *initParameter(void);
t_E2704_parameter *newParameter(char *paramName, int address);
void addParameter(t_E2704_parameter_list *paramList, char *paramName, int address);
void printList(t_E2704_parameter_list *paramList);
void freeList(t_E2704_parameter_list *paramList);
ErrorComm getValue(HANDLE hPort, t_E2704_parameter_list *paramList, E2704_Channel channel);

// -- Print --

void printParameterRow(t_E2704_parameter_list *paramList);
void printChannel(t_E2704_parameter_list *paramList, E2704_Channel channel);
void clearChannel(t_E2704_parameter_list *paramList, E2704_Channel channel);
void printLine(const int len, const int offset, char firstChar);
void printEnd(t_E2704_parameter_list *paramList, E2704_Channel lastChannel);


#endif