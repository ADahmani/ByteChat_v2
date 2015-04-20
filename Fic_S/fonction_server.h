/**
*\file fonction_server.h
*\brief Prototypes des fonctions liés à la liste chainée de client connectés
*/

#ifndef FONCTION_SERVER_H
#define FONCTION_SERVER_H

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "structure_serveur.h"
#include "client_row.h"


#define CLIENTS 10




/**
*\fn int compare(struct THREADINFO *a, struct THREADINFO *b)
*\brief Fonction pour comparer deux sockets, la socket actuel du client connecté et les sockets des clients connecté
*on l'utilise pour ne pas envoyer à nous meme le message broadcasté 
*\param *a : une structure qui contient le numero d'une socket, *b une structure qui contient le numero d'une socket
*\return la difference entre les deux numero de socket 
*/
int compare(struct THREADINFO *a, struct THREADINFO *b);
/**
*\fn void list_init(struct LLIST *ll)
*\brief initialisation de notre liste chainée
*\param *ll : une liste chainée LLIST
*/
void list_init(struct LLIST *ll);
/**
*\fn int list_insert(struct LLIST *ll, struct THREADINFO *thr_info)
*\brief insertion d'un client dans la liste des clients connecté
*\param *ll : notre liste chainé des clients connecté, *thr_info une structure qui contiens les informations d'un client
*/
int list_insert(struct LLIST *ll, struct THREADINFO *thr_info);
/**
*\fn int list_delete(struct LLIST *ll, struct THREADINFO *thr_info)
*\brief suppresion d'un client de la liste des clients connecté
*\param *ll : notre liste chainé des clients connecté, *thr_info une structure qui contiens les informations d'un client
*/
int list_delete(struct LLIST *ll, struct THREADINFO *thr_info);
/**
*\fn void list_dump(struct LLIST *ll)
*\brief affichage des clients connecté
*\param *ll : notre liste chainé des clients connecté

*/
void list_dump(struct LLIST *ll);



#endif
