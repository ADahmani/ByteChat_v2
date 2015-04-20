/**
*\file client_row.h
*\brief Définition des structures
*\date 2 janvier 2015
*
*Fichier de définition des structures utilisées pour les fonctions de base de données cliente
*Sert aussi de fichier d'inclusion des bibliothèques
*/



#ifndef CLIENT_ROW_H
#define CLIENT_ROW_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>
#include <unistd.h>
#include <ctype.h>



#define MAX_CLIENT 250
#define NomLength 24
#define MdpLength 24
#define AmisLength 1024
#define MSG_LENGTH 512
#define IP_LENGTH 16
#define PORT_LENGTH 5
#define MAX_AMIS 25


/**
*\struct T_Amis
*\brief Structure représentant la liste d'amis
*Contient un tableau contenant la liste des id amis et un entier nbAmis représentant n-1 amis.
*
*/
typedef struct
{
	int nbAmis;
	int idAmis[MAX_AMIS];
} T_Amis;


/**
*\struct Client
*\brief Structure représentant un Client
*Contient un entier idClient qui est l'id du client, *nomClient son nom, *mdpClient son mot de passe et *amis sa liste d'amis
*
*/
typedef struct
{
	int idClient;
	char *nomClient;
	char *mdpClient;
	T_Amis *amis;

} Client;


/**
*\struct tabClient
*\brief Structure représentant un tableau de Client
*Contient un tableau de Client et un entier représentant les n-1 clients du tableaux
*/
typedef struct
{
	Client tabC[MAX_CLIENT];
	int nbC;

}tabClient;




#endif
