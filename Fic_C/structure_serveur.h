/**
*\file structure_serveur.h
*\brief Définition des structures
*\date 2 janvier 2015
*
*Fichier de définition des structures utilisées pour et par le système
*
*/


#ifndef STRUCTURE_SERVEUR_H
#define STRUCTURE_SERVEUR_H

#define BUFFSIZE 1024
#define ALIASLEN 32
#define OPTLEN 16


/**
*\struct T_AmiConnecteY
*\brief Structure représentant un ami connecté
*Contient un nomAmis pour stocker le nom de cet ami et un flag, online pour savoir s'il est connecté ou non
*
*/
struct T_AmiConnecteY
{
    char nomAmis[24];
    int online;
};

/**
*\struct T_FriendList
*\brief Structure représentant une liste d'ami 
*Contient un entier nbAmis qui représente n-1 amis et un tableau de structure d'ami
*
*/
struct T_FriendList
{
    int nbAmis;
    struct T_AmiConnecteY amiconnecte[10];
};

/**
*\struct PACKET
*\brief Structure représentant les données échangées bidirectionnellement par le serveur et le client
*Contient une chaine "option" pour l'instruction, une chaine alias qui contient le nom du client et une chaine buff qui représente les données envoyées.
*Cette structure est le pivot du protocole du programme.
*
*/
struct PACKET {
    char option[OPTLEN]; 
    char alias[ALIASLEN]; 
    char buff[BUFFSIZE]; 
}; 
 
/**
*\struct THREADINFO
*\brief Structure contenant toutes les références d'un client sur le serveur
*Contient thread_ID, un pointeur sur le thread se chargeant du client, sockfd la référence sur le socket client du client et alias le nom du client
*
*/
struct THREADINFO {
    pthread_t thread_ID; 
    int sockfd; 
    char alias[ALIASLEN]; 
};

/**
*\struct LLNODE 
*\brief Structure représentant une maille de la liste chainée de client connectés
*Contient une structure THREADINFO et une référence sur la maille suivante
*
*/
struct LLNODE {
    struct THREADINFO threadinfo;
    struct LLNODE *next;
};

/**
*\struct LLIST
*\brief Structure représentant la liste chainée de client connectés
*Contient un entier size qui représente la taille de la chaine, un pointeur sur la tete de la chaine et un autre sur la queue de la chaine
*
*/
struct LLIST {
    struct LLNODE *head, *tail;
    int size;
};

/**
*\struct USER
*\brief Structure représentant un client, elle contiens son numero du socket son nom et son mot de passe
*elle est utilisé pour l'échange de connexion qui se fait entre le serveur et le client
*
*/
struct USER {
        int sockfd; 
        char alias[ALIASLEN]; 
        char pwd[12];
};

#endif
