/**
*\file fonction_client.h
*\brief Définition de prototypes
*\date 2 janvier 2015
*
*Fichier de définition des prototypes des fonctions intervenant sur la base de données cliente
*/


#ifndef FONCTION_CLIENT_H
#define FONCTION_CLIENT_H

#include "client_row.h"


//Fonction de selection
/**
*\fn void Select_All(tabClient *tab_C)
*\brief Fonction de selection de tous les clients de la base
*
*\param *tab_C Adresse d'une structure tabClient où insérer la liste
*/
void Select_All(tabClient *tab_C);
/**
*\fn Client Search_Client_By_Name(char *str_search)
*\brief Fonction de recherche d'un client par son nom
*\param *str_search Chaine de caractère représentant le nom du client à rechercher
*\return Client Structure représentant un client: Cl_aux.idClient == 0 en cas de recherche infructueuse, sinon Cl_aux.idClient > 0
*/
Client Search_Client_By_Name(char *str_search);
/**
*\fn Client Search_Client_By_Id(int id_search)
*\brief Fonction de recherche d'un client par son id
*\param id_search Entier représentant l'id du client à rechercher
*\return Client Structure représentant un client: Cl_aux.idClient == 0 en cas de recherche infructueuse, sinon Cl_aux.idClient > 0
*/
Client Search_Client_By_Id(int id_search);

//Fonction d'affichage
/**
*\fn void Afficher_Ami(T_Amis *A)
*\brief Fonction d'affichage d'une structure T_Amis
*\param *A Pointeur sur la structure T_Amis à afficher
*/
void Afficher_Ami(T_Amis *A);
/**
*\fn void Afficher_Client(Client *C)
*\brief Fonction d'affichage d'une structure Client
*\param *C Pointeur sur la structure Client à afficher
*/
void Afficher_Client(Client *C);
/**
*\fn void Afficher_All_Client()
*\brief Fonction d'affichage de tous les clients de la base de données
*/
void Afficher_All_Client();
/**
 *\fn void Afficher_All_Ami(char *nom_client) 
 *\brief Afficher tous les amis d'un client
 *\param *nom_client Chaine contenant le nom du client à afficher 
 * Afficher une erreur en cas de mauvais nom, sinon une liste d'ami, préviens si la liste est vide
 */
void Afficher_All_Ami(char *nom_client);

// fonction de conversion de string vers une struct
/**
*\fn Client Str2Clt(char *m )
*\brief Fonction de conversion d'une chaine de caractère en structure Client
*\param *m Chaine de caractère à convertir
*\return Client structure Client
*/
Client Str2Clt(char *m );
/**
*\fn strToTamis(T_Amis *ta, char *strIdAmis)
*\brief Fonction de conversion d'une chaine de caractère en structure T_Amis
*\param *ta Pointeur sur la structure T_Amis à qui affecter le résultat de conversion, *strId Chaine de caractère à convertir
*/
void strToTamis(T_Amis *ta, char *strIdAmis);

//fonction de conversion de struct vers string
/**
*\fn void CltToStr(char *chaine,Client *C)
*\brief Fonction de conversion d'une structure Client vers une chaine de caractère
*\param *chaine Pointeur sur la chaine à qui affecter le résultat de la conversion, *C Structure Client à convertir
*/
void CltToStr(char *chaine,Client *C); //Utilise la concatÃ©nation


//fonction modification de base
/**
*\fn int Ajouter_Client(char *nom_client,char *mdp_client)
*\brief Fonction d'ajout de client dans la base de données
*\param *nom_client Chaine de caractère contenant le nom du client, *mdp_client Chaine de caractère contenant le mot de passe du client
*\return 1 : Le client a été ajouté, 0 : Le client existe déjà, -1 : Une erreur est survenue.
*/
int Ajouter_Client(char *nom_client,char *mdp_client);
/**
*\fn int Supprimer_Client(char *nom_client)
*\brief Fonction de suppression de client de la base de données
*\param *nom_client Chaine de caractère contenant le nom du client à supprimer
*\return 1 : Le client a été supprimé, 0 : Le client n'existe pas, -1 : Une erreur est survenue.
*/
int Supprimer_Client(char *nom_client);
/**
*\fn int Ajouter_Ami(char *nom_client,char *nom_ami)
*\brief Fonction d'ajout d'un ami dans la liste d'ami d'un client
*\param *nom_client Chaine de caractère contenant le nom du client, *nom_ami Chaine de caractère contenant le nom de l'ami
*\return 1 : L'ami a bien été ajouté, 0 : Le client ou l'ami n'existe pas, -2 : La personne est déjà dans ses amis, -1 : Une erreur est survenue.
*/
int Ajouter_Ami(char *nom_client,char *nom_ami);
/**
*\fn int Supprimer_Ami(char *nom_client,char *nom_ami)
*\brief Fonction de suppression d'un ami de la liste d'ami d'un client
*\param *nom_client Chaine de caractère contenant le nom du client, *nom_ami Chaine de caractère contenant le nom de l'ami
*\return 1 : L'ami a bien été supprimé, 0 : Le client ou l'ami n'existe pas, -2 : La personne n'est pas dans ses amis, -1 : Une erreur est survenue.
*/
int Supprimer_Ami(char *nom_client,char *nom_ami);




#endif
