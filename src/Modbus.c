/*
 *  Modbus.c
 *  commsLib
 *
 *  Created by Audrey SIEPRAWSI on 11/03/2015.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Modbus.h"
#include "String.h"

static BOOL _ModBusCalculCRCStd = TRUE;

void ModBusCalculCRCStd(BOOL std)
{
    _ModBusCalculCRCStd = std;
}

float ModBusFloatAsciiToIeee( char* ascii, EModBusProcessorCompatibility processorCompatibility /*= INTEL*/ )
{
    float result = 0;
    IsenUIntFloatBits	value;
    int ivalue      = 0;
    int i_bit       = 0;

    if ( processorCompatibility == INTEL )
    {
        for ( i_bit = 0 ; i_bit < 4 ; i_bit++ )
        {
            ivalue = ivalue << 8 | ( short )ascii[i_bit] & 0x00FF;
        }
    }
    else
    {
        int ivalue1     = 0;
        int ivalue2     = 0;
        int ivalueMotorola1 = 0;
        int ivalueMotorola2 = 0;

        for ( i_bit = 0 ; i_bit < 2 ; i_bit++ )
        {
            ivalue1 = ivalue1 << 8 | ( short )ascii[i_bit] & 0x00FF;
        }
        for ( i_bit = 2 ; i_bit < 4; i_bit++ )
        {
            ivalue2 = ivalue2 << 8 | ( short )ascii[i_bit] & 0x00FF;
        }
        // on inverse msb / lsb
        ivalueMotorola1 = ( ivalue1 >> 8 & 0x00FF ) ;
        ivalueMotorola1     = ivalueMotorola1 | ( ivalue1 << 8 & 0xFF00 );
        ivalueMotorola2 = ( ivalue2 >> 8 & 0x00FF ) ;
        ivalueMotorola2     = ivalueMotorola2 | ( ivalue2 << 8 & 0xFF00 );

        ivalue = ( ivalueMotorola2 << 16 & 0xFFFF0000 ) | ( ivalueMotorola1 & 0x0000FFFF );
    }
    value._ivalue = ivalue;
    result=value._fvalue;

    return result;
}

int ModBusIntAsciiToIeee( char* ascii, EModBusProcessorCompatibility processorCompatibility /*= INTEL*/ )
{
    int result = 0;

    IsenUIntFloatBits	value;
    int ivalue      = 0;
    int i_bit       = 0;

    if ( processorCompatibility == INTEL )
    {
        for ( i_bit = 0 ; i_bit < 4 ; i_bit++ )
        {
            ivalue = ivalue << 8 | ( short )ascii[i_bit] & 0x00FF;
        }
    }
    else
    {
        int ivalue1     = 0;
        int ivalue2     = 0;
        int ivalueMotorola1 = 0;
        int ivalueMotorola2 = 0;

        for ( i_bit = 0 ; i_bit < 2 ; i_bit++ )
        {
            ivalue1 = ivalue1 << 8 | ( short )ascii[i_bit] & 0x00FF;
        }
        for ( i_bit = 2 ; i_bit < 4 ; i_bit++ )
        {
            ivalue2 = ivalue2 << 8 | ( short )ascii[i_bit] & 0x00FF;
        }
        // on inverse msb / lsb
        ivalueMotorola1 = ( ivalue1 >> 8 & 0x00FF);
        ivalueMotorola1     = ivalueMotorola1 | ( ivalue1 << 8 & 0xFF00 );
        ivalueMotorola2 = ( ivalue2 >> 8 & 0x00FF);
        ivalueMotorola2     = ivalueMotorola2 | ( ivalue2 << 8 & 0xFF00 );

        ivalue = ( ivalueMotorola2 << 16 & 0xFFFF0000 ) | ( ivalueMotorola1 & 0x0000FFFF );
    }

    value._ivalue =ivalue;
    result=value._ivalue;

    return result;
}

short ModBusShortAsciiToIeee(char* ascii, EModBusProcessorCompatibility processorCompatibility/*= INTEL*/)
{
    short result = 0;
    IsenUIntFloatBits value;
    int ivalue=0;

    int i_bit = 0;
    for (i_bit=0;i_bit < 2; i_bit++)
        ivalue = ivalue<<8 | (short)ascii[i_bit]& 0x00FF;

    if (processorCompatibility==MOTOROLA)
    {
        // on inverse msb / lsb
        int ivalueMotorola = (ivalue>>8 & 0x00FF) ;
        ivalueMotorola =   ivalueMotorola | (ivalue<<8 & 0xFF00);
        ivalue = ivalueMotorola;
    }

    value._ivalue = ivalue;
    result = value._svalue;

    return result;
}

unsigned short
ModBusUShortAsciiToIeee(char* ascii, EModBusProcessorCompatibility processorCompatibility/*= INTEL*/)
{
    unsigned short result = 0;
    IsenUIntFloatBits value;
    int ivalue=0;

    int i_bit=0;
    for (i_bit=0;i_bit < 2; i_bit++)
        ivalue = ivalue<<8 | (unsigned short)ascii[i_bit]& 0x00FF;

    if (processorCompatibility==MOTOROLA)
    {
        // on inverse msb / lsb
        int ivalueMotorola = (ivalue>>8 & 0x00FF) ;
        ivalueMotorola =   ivalueMotorola | (ivalue<<8 & 0xFF00);
        ivalue = ivalueMotorola;
    }


    value._ivalue =ivalue;
    result=value._svalue;

    return result;
}

char
ModBusCharAsciiToIeee(char* ascii, EModBusProcessorCompatibility processorCompatibility/*= INTEL*/)
{
    char result;

    IsenUIntFloatBits value;
    int ivalue=0;
    ivalue = ivalue<<8 | (short)ascii[0]& 0x00FF;

    if (processorCompatibility==MOTOROLA)
    {
        // on inverse msb / lsb
        int ivalueMotorola = (ivalue>>8 & 0x00FF) ;
        ivalueMotorola =   ivalueMotorola | (ivalue<<8 & 0xFF00);
        ivalue = ivalueMotorola;
    }


    value._ivalue =ivalue;
    result=value._cvalue;

    return result;
}

unsigned short
ModBusCalculCRC(char* trame, int len)
{
    int i_c = 0;
    int bit = 0;
    unsigned short crc		= 0xffff;
    unsigned short next		= 0;
    unsigned short report	= 0;

    for (i_c = 0; i_c < len; i_c++)
    {
        next  = (unsigned short)(unsigned char)(trame[i_c]);
        crc  ^= next;
        for (bit = 0; bit < 8; bit++)
        {
            report=0;
            if ((crc%(unsigned short)2)!=0)
                report=1;
            crc >>= 1;

            // attention le decalage met un bit a 1 sur le  bit de poid fort
            // on annule par un and
            crc &= 0x7fff;


            if (report)
                crc ^= 0xA001;
        }
    }
    // on inverse
    if (_ModBusCalculCRCStd)
    {
        unsigned short msb = crc<<8;
        unsigned short lsb = (crc >> 8);
        crc = msb | lsb;
    }
    //on se remet toujours en mode par d�faut
    _ModBusCalculCRCStd=TRUE;

    return crc;
}


void
ModBusWrite(char* trame, int i_index, unsigned short mot, EModBusProcessorCompatibility processorCompatibility)
{
    if (processorCompatibility==INTEL)
    {
        trame[i_index] = (char)(mot >> 8);
        i_index++;
        trame[i_index] = (char)(mot);
    }
    else
    {
        trame[i_index] = (char)(mot);
        i_index++;
        trame[i_index] = (char)(mot >> 8);
    }
    return;
}

void
ModBusRead(char* trame, int i_index, unsigned char* lsb, unsigned char* msb, const EModBusProcessorCompatibility processorCompatibility)
{
    if (processorCompatibility==INTEL)
    {
        *msb = trame[i_index];
        *lsb = trame[i_index+1];
    }
    else
    {
        *lsb = trame[i_index];
        *msb = trame[i_index+1];
    }

    return ;
}

/*===============================================================
<fonction>		makeTrameLecModBus
<entree(s)>
<sortie(s)>
<specification>
        Fabrication des trames de lecture du type MODBUS
        Trame de la forme:
        [adresse][code fonction][adresse debut][nb. param][CRC]
        adresse			= 1 octet
        code fonction	= 1 octet
        adresse debut   = 2 octets
        nb. param		= 2 octets
        CRC				= 2 octets

  exemple : lecture sur l'appareil d'adresse 19, lire 14 bits en
            comman�ant � l'adresse 2

      char* trame;
      makeTrameLecJBus(19,MODBUS_FUNCTION_READ_BITS,2,14, trame)

 => trame = [ (adresse=)13h (code fonction=)01h (adresse debut=)00h (nb. param=)02h (CRC=)1Fh 7Ch ]
===============================================================*/
int
makeTrameLecModBus(int address,  int codeFunction, int startAddress, int  nbParams, char* trame , EModBusProcessorCompatibility processorCompatibility /* = INTEL */)
{
    int indexTrame = 0;
    unsigned short crc = 0;

    // Ajout de l'adresse
    trame[indexTrame] = (char)(address);
    indexTrame++;

    // Ajout du code fonction
    trame[indexTrame] = (char)(codeFunction);
    indexTrame++;

    // adresse du 1er parametre
    ModBusWrite(trame,indexTrame,startAddress,processorCompatibility);
    indexTrame = indexTrame+2;

    // nb. de parametres a lire
    ModBusWrite(trame,indexTrame,nbParams,processorCompatibility);
    indexTrame = indexTrame+2;

    // calcul du crc
    crc = ModBusCalculCRC(trame,indexTrame);
    ModBusWrite(trame,indexTrame,crc,processorCompatibility);
    indexTrame = indexTrame+2;

    return indexTrame;
}

/*===============================================================
<fonction>		makeTrameEcrModBus
<entree(s)>
<sortie(s)>
<specification>
        Fabrication des trames d'ecriture du type MODBUS
        Trame de la forme:
        [adresse][code fonction][adresse debut][nb. param][CRC]
        adresse			= 1 octet
        code fonction	= 1 octet
        adresse debut   = 2 octets
        nb. param		= 2 octets (=1 par defaut)
        CRC				= 2 octet

  exemple : ecrire sur l'appareil d'adresse 2, le bit d'adresse 2
            avec la valeur 1

      char* trame;
      makeTrameLecJBus(2,MODBUS_FUNCTION_WRITE_BIT,2,1,1, trame)

 => trame = [ (adresse=)02h (code fonction=)05h (adresse debut=)00h 002h (CRC=)6Dh A9h ]
===============================================================*/
int
makeTrameEcrModBusFromShort(int address,  int codeFunction, int startAddress, short value,  char* trame, EModBusProcessorCompatibility processorCompatibility /* = INTEL */)
{
    int indexTrame = 0;
    unsigned short crc = 0;

    // Ajout de l'adresse
    trame[indexTrame] = (char)(address);
    indexTrame++;

    // Ajout du code fonction
    trame[indexTrame] = (char)(codeFunction);
    indexTrame++;


    // adresse du 1er parametre
    ModBusWrite(trame,indexTrame,startAddress,processorCompatibility);
    indexTrame = indexTrame+2;

    // valeur du seul parametre a affecter
    ModBusWrite(trame,indexTrame,value,processorCompatibility);
    indexTrame = indexTrame+2;

    // calcul du crc
    crc = ModBusCalculCRC(trame,indexTrame);
    ModBusWrite(trame,indexTrame,crc,processorCompatibility);
    indexTrame = indexTrame+2;

    return indexTrame;
}

/*===============================================================
<fonction>		makeTrameEcrModBus
<entree(s)>
<sortie(s)>
<specification>
        Fabrication des trames d'ecriture du type MODBUS
        Trame de la forme:
        [adresse][code fonction][adresse debut][nb. param][CRC]
        adresse			= 1 octet
        code fonction	= 1 octet
        adresse debut   = 2 octets
        nb. param		= 2 octets (=1 par defaut)
        CRC				= 2 octet

  exemple : ecrire sur l'appareil d'adresse 2, le bit d'adresse 2
            avec la valeur 1

      char* trame;
      makeTrameLecJBus(2,MODBUS_FUNCTION_WRITE_BIT,2,1,1, trame)

 => trame = [ (adresse=)02h (code fonction=)05h (adresse debut=)00h 002h (CRC=)6Dh A9h ]
===============================================================*/
int
makeTrameEcrModBusFromFloat(int address, int codeFunction,  int startAddress, float value,  char* trame, EModBusProcessorCompatibility processorCompatibility /* = INTEL */)
{
    int indexTrame = 0;
    IsenUIntFloatBits consigne;
    short mot1 = 0;
    short mot2 = 0;
    short values[2];

    consigne._fvalue = value;

    mot1 = consigne._ivalue >>16;
    mot2 = consigne._ivalue;

    values[0] = mot1;
    values[1] = mot2;

    indexTrame = makeTrameEcrModBusFromShortTab(address, codeFunction, startAddress, values, 2, trame,processorCompatibility);

    return indexTrame;
}

int
makeTrameEcrModBusFromInt(int address, int codeFunction, int startAddress, int value,  char* trame, EModBusProcessorCompatibility processorCompatibility /* = INTEL */)
{
    int indexTrame = 0;
    IsenUIntFloatBits consigne;
    short mot1 = 0;
    short mot2 = 0;
    short values[2];

    consigne._ivalue = value;

    mot1 = consigne._ivalue >>16;
    mot2 = consigne._ivalue;

    values[0] = mot1;
    values[1] = mot2;

    indexTrame = makeTrameEcrModBusFromShortTab(address, codeFunction, startAddress, values, 2, trame,processorCompatibility);

    return indexTrame;
}

/*===============================================================
<fonction>		makeTrameEcrModBus
<entree(s)>
<sortie(s)>
<specification>
        Fabrication des trames d'ecriture de plusieurs
        valeurs simulatnement du type MODBUS
        Trame de la forme:
        [adresse][code fonction][adresse debut][[nb. mots a ecrire][nb octets de donnees][donnees][CRC]
        adresse			= 1 octet
        code fonction	= 1 octet
        adresse debut   = 2 octets
        nb. mots a ecrire = 2 octets
        nb octets de donnees = 1 octet
        donnees			= nb octets
        CRC				= 2 octets

===============================================================*/
int
makeTrameEcrModBusFromShortTab(int address,  int codeFunction, int startAddress, short* values, int nbMots, char* trame, EModBusProcessorCompatibility processorCompatibility /* = INTEL */)
{
    unsigned short crc = 0;
    int i_value = 0;
    int indexTrame = 0;

    // Ajout de l'adresse
    trame[indexTrame] = (char)(address);
    indexTrame++;

    // Ajout du code fonction
    trame[indexTrame] = (char)(codeFunction);
    indexTrame++;

    // adresse du 1er mot
    ModBusWrite(trame,indexTrame,startAddress,processorCompatibility);
    indexTrame = indexTrame+2;

    // nombre de mots a ecrire
    ModBusWrite(trame,indexTrame,nbMots,processorCompatibility);
    indexTrame = indexTrame+2;

    // nombre d'octets de donn�es
    trame[indexTrame] = (char)(nbMots*2);
    indexTrame++;

    // boucle sur les mots a ecrire
    if (processorCompatibility==INTEL)
    {
        for (i_value = 0; i_value < nbMots; i_value++)
        {
            short value = values[i_value];
            // valeur du mot
            ModBusWrite(trame,indexTrame,value,processorCompatibility);
            indexTrame = indexTrame+2;
        }
    }
    else
    {
        for (i_value = nbMots-1; i_value >=0; i_value--)
        {
            short value = values[i_value];
            // valeur du mot
            ModBusWrite(trame,indexTrame,value,processorCompatibility);
            indexTrame = indexTrame+2;
        }
    }

    // calcul du crc
    crc = ModBusCalculCRC(trame,indexTrame);
    ModBusWrite(trame,indexTrame,crc,processorCompatibility);
    indexTrame = indexTrame+2;

    return indexTrame;
}


/*===============================================================
<fonction>		makeTrameEcrModBus
<entree(s)>
<sortie(s)>
<specification>
        Fabrication des trames d'ecriture de plusieurs
        valeurs simultanement du type MODBUS
        Trame de la forme:
        [adresse][code fonction][adresse debut][[nb. mots a ecrire][nb octets de donnees][donnees][CRC]
        adresse			= 1 octet
        code fonction	= 1 octet
        adresse debut   = 2 octets
        nb. mots a ecrire = 2 octets
        nb octets de donnees = 1 octet
        donnees			= nb octets
        CRC				= 2 octets

===============================================================*/
int
makeTrameEcrModBusFromIntTab(int address,int codeFunction, int startAddress, int* values, int nbValues, char* trame, EModBusProcessorCompatibility processorCompatibility /* = INTEL */)
{
    int indexTrame = 0;
    IsenUIntFloatBits consigne;

    int length = nbValues*2;
    short* tabShort = (short*)( malloc(length * sizeof(short)));

    int i,i_value=0;
    for (i= 0; i < nbValues; i++)
    {
        short mot1 = 0;
        short mot2 = 0;

        consigne._ivalue = values[i];

        mot1 = consigne._ivalue>>16;
        mot2 = consigne._ivalue;

        tabShort[i_value++]=mot1;
        tabShort[i_value++]=mot2;
    }

    indexTrame = makeTrameEcrModBusFromShortTab(address, codeFunction, startAddress, tabShort, length, trame,processorCompatibility);

    free(tabShort);

    return indexTrame;
}

int
makeTrameEcrModBusFromFloatTab(int address,int codeFunction,  int startAddress, float* values, int nbValues,  char* trame, EModBusProcessorCompatibility processorCompatibility /* = INTEL */)
{
    int indexTrame = 0;
    IsenUIntFloatBits consigne;

    int length = nbValues*2;
    short* tabShort = (short*)( malloc(length * sizeof(short)));

    int i, i_value=0;
    for (i= 0; i < nbValues; i++)
    {
        short mot1 = 0;
        short mot2 = 0;

        consigne._fvalue = values[i];

        mot1 = consigne._ivalue>>16;
        mot2 = consigne._ivalue;

        tabShort[i_value++]=mot1;
        tabShort[i_value++]=mot2;
    }

    indexTrame = makeTrameEcrModBusFromShortTab(address, codeFunction, startAddress, tabShort, length, trame,processorCompatibility);

    free(tabShort);
    return indexTrame;
}

/*===============================================================
<fonction>		parseTrameModBus
<entree(s)>
<sortie(s)>
<specification>
    parse d'une trame de lecture ou d'ecriture MODBUS
        [adresse][codeFonction] [nbOctetsLus][octet1]...[octetn][crc]
===============================================================*/
ErrorComm parseTrameModBus(char* trame, int lengthTrameReceive, char* values,  int* nbValues, int* address, int* codeFunction, EModBusProcessorCompatibility processorCompatibility /* = INTEL */)
{
    ErrorComm codeRetour = ERRORCOMM_NOERROR;

    unsigned char lsb,msb;
    unsigned int i_trame = 0;

    unsigned short readCrc, crc = 0;

    // la trame a analyser a une longueur  >= 3 [adresse][codeFonction]...[crc]
    if (lengthTrameReceive < 4)
    {
        return ERRORCOMM_BCC;
    }

    // adresse du peripherique
    *address = (int)trame[i_trame++];

    // le code fonction
    *codeFunction = (int)trame[i_trame++];

    switch (*codeFunction)
    {
        case MODBUS_FUNCTION_READ_BITS:{
            // nombre d'octets lus
            int i_octet = 0;
            char car = trame[i_trame++];
            int nbOctetsLus = car & 0x00FF;

            if (lengthTrameReceive < nbOctetsLus)
                return ERRORCOMM_ERROR;

            // boucle sur les octets � lire
            for (i_octet = 0; i_octet < nbOctetsLus; i_octet++)
                values[i_octet] = trame[i_trame++];

            *nbValues = nbOctetsLus;
            break;}

        case MODBUS_FUNCTION_READ_NWORDS:
        case MODBUS_FUNCTION_READ_INPUT_NWORDS:{
            // nombre d'octets lus
            char car = trame[i_trame++];
            unsigned int nbOctetsLus = car & 0x00FF;
            int nbMots = nbOctetsLus/2;

            if (lengthTrameReceive < nbOctetsLus)
                return ERRORCOMM_BCC;

            if (processorCompatibility==INTEL)
            {
                int i_mot = 0;
                for (i_mot = 0; i_mot < nbMots; i_mot++)
                {
                    ModBusRead(trame, i_trame, &lsb, &msb, processorCompatibility);

                    values[i_mot*2] = (char)msb;
                    values[i_mot*2+1] = (char)lsb;
                    i_trame = i_trame + 2;
                }
            }
            else
            {
/*
                char* valueTmp;
                // boucle sur les mots � lire
                int i_mot = 0;
                for (i_mot = 0; i_mot < nbMots; i_mot++)
                {
                    if(!ModBusRead(trame, i_trame, &lsb, &msb, processorCompatibility))
                        return ERRORCOMM_BCC;

                    addCharToTrame(valueTmp, (char)msb);
                    addCharToTrame(valueTmp, (char)lsb);
                    i_trame+=2;
                }
                for (i_mot = nbMots-1; i_mot >=0; i_mot--)
                {
                    values+=valueTmp[i_mot*2];
                    values+=valueTmp[i_mot*2+1];
                    addCharToTrame(values, valueTmp[i_mot*2]);
                    addCharToTrame(values, valueTmp[i_mot*2+1]);
                }
            }
            */}
            *nbValues = nbMots;
            break;}

        case MODBUS_FUNCTION_WRITE_BIT:
        case MODBUS_FUNCTION_WRITE_BITS:
        case MODBUS_FUNCTION_WRITE_WORD:
        case MODBUS_FUNCTION_WRITE_WORDS:{
            if (lengthTrameReceive < 2)
                return ERRORCOMM_BCC;

            // Adresse mot
            ModBusRead(trame, i_trame, &lsb, &msb, processorCompatibility);
            i_trame+=2;
            ModBusRead(trame, i_trame, &lsb, &msb, processorCompatibility);
            i_trame+=2;

            values[0] = (char)msb;
            values[1] = (char)lsb;

            *nbValues = 1;
            break;}

        case MODBUS_FUNCTION_READ_STATE:{
            values[0] = (char)trame[i_trame++];
            *nbValues = 1;
            break;}

        case MODBUS_FUNCTION_LOOP:{
            break;}

            // traitement des erreurs
            // erreur sur ecriture
            case (MODBUS_FUNCTION_WRITE_BIT   - 128):;
            case (MODBUS_FUNCTION_WRITE_BITS   - 128):;
            case (MODBUS_FUNCTION_WRITE_WORD  - 128):;
            case (MODBUS_FUNCTION_WRITE_WORDS - 128):;
            case (MODBUS_FUNCTION_READ_BITS   - 128):;
            case (MODBUS_FUNCTION_READ_NWORDS - 128):;
            case (MODBUS_FUNCTION_READ_INPUT_NWORDS - 128):;
            case (MODBUS_FUNCTION_READ_STATE  - 128):;
            case (MODBUS_FUNCTION_LOOP - 128): {
                char codeErreur = (int)trame[i_trame++];
                values[0] =codeErreur;
                codeRetour = ERRORCOMM_ERROR;
                *nbValues = 1;
                break;}

    }

    // CRC present dans la trame
    ModBusRead(trame, i_trame, &lsb, &msb,processorCompatibility);

    readCrc = ModBusCalculCRC(trame,i_trame);
    crc = (((char)msb << 8)&0xFF00)+((char)lsb&0x00FF);

    // verification du crc lu et du crc theorique
    if (readCrc != crc)
        return ERRORCOMM_BCC;

    return codeRetour;
}


int ModbusSerialToTCPIP(char* trameIn, int lengthTrameIn, EModBusProcessorCompatibility processorCompatibility /* = INTEL */)
{
    int indexTrame = 0;
    int i_car = 0;
    char trame[100];

    // suppression du CRC
    short nbCar = lengthTrameIn-2;
    memcpy(&trame, trameIn, nbCar);
    memset(trameIn,'\0',nbCar+1);

    ModBusWrite(trameIn, indexTrame,	MODBUS_TRANSAC_ID	,processorCompatibility);
    indexTrame = indexTrame+2;
    ModBusWrite(trameIn, indexTrame,	MODBUS_ENCODING		,processorCompatibility);
    indexTrame = indexTrame+2;
    ModBusWrite(trameIn, indexTrame,	nbCar				,processorCompatibility);
    indexTrame = indexTrame+2;

    memcpy(trameIn+indexTrame, trame, nbCar);
    indexTrame = indexTrame+nbCar;

    return indexTrame;
}

int ModbusTCPIPToSerial(char* trameIn,  int lengthTrameIn, EModBusProcessorCompatibility processorCompatibility /* = INTEL */)
{
    char trame[100];
    unsigned short crc = 0;
    short nbCar = lengthTrameIn-6;

    if (nbCar>0)
    {
        // les 6 premiers octects correspondent � l'entete MBAP, on les supprime
        memcpy(trame, trameIn+6, nbCar);

        // et on rajoute un CRC pour retomber sur nos pattes
        crc = ModBusCalculCRC(trame,nbCar);
        ModBusWrite(trame,nbCar,crc,processorCompatibility);
        nbCar = nbCar+2;

        // Met � jour la trame en entree
        memset(trameIn,'\0',99);
        memcpy(trameIn, trame, nbCar);

    }
    return nbCar;
}

