# Modbus

lancer le logiciel MODBUS: `Séance 2 > Simulateurs > Modbus emulator > mod_RSsim.exe`

setup on comunication serial > connect COM1
> 9600  
> ...


# Virtual serial port:

Open `HHD Virtual Serial Port Tools`

Create local bridge

COM1 and COM2


# Communication

```
****************************************************************************
*                             PROTOCOLE MODBUS                             *
****************************************************************************
Saisir le type de communication: 0 (liaison Serie) / 1 (liaison IP)  ? 0
Entrer le numero de port?
2
*********** Parametrage du port serie ***********
Entrer la vitesse de transmission?
9600
Entrer le nombre de bits de donnees? (5-8)
8
Entrer la parite? 0 (pas de parite) / 1 (Parite impair) / 2 (Partie pair)
0
Entrer le nombre de bist de stop? 0 (1 bit) / 1 (1.5 bits) / 2 (2 bits)
0

****************************************************************************
   1. Demande de lecture.
   2. Demande d'ecriture.
   3. Quitter.
Que souhaitez-vous faire?
```

```
****************************************************************************
   1. Demande de lecture.
   2. Demande d'ecriture.
   3. Quitter.
Que souhaitez-vous faire?
1

 DEMANDE DE LECTURE
A partir de quelle adresse souhaitez-vous lire?
0
Quel type de parametre voulez-vous lire? 0 (short) / 1 (int) / 2 (float)
0

 Send trame (length = 8):01 03 00 00 00 01 84 0A

 Receive trame (length = 7): 01 03 02 00 09 78 42
  Value : 9

****************************************************************************
```

```
/*
Entre la valeur a ecrire?
8

 Send trame (length = 8):01 06 00 01 00 08 D9 CC

 Receive trame (length = 8): 01 06 00 01 00 08 D9 CC
  Value[0] : 8

*/
```