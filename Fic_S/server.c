/**
*\file server.c
*\brief Fichier contenant le code du serveur
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "client_row.h"
#include "structure_serveur.h"
#include "fonction_client.h"
#include "fonction_server.h"
#include "fonction_ncurses.h" 


#define CHECK(sts,msg) if ((sts) == -1){ perror(msg); exit(-1);  }
#define IP "127.0.0.1" // ip du serveur
#define PORT 5555 // port utilisé
#define BACKLOG 10 // nombre max des clients connecté



//GLOBAL VARIABLE
int sockfd, newfd;
struct THREADINFO thread_info[CLIENTS];
struct LLIST client_list;
pthread_mutex_t clientlist_mutex; 
 
 

/**
*\fn void *io_handler(void *param)
*\brief Fonction lié au thread pour capturer les commandes tappé par l'utilisateur (list, exit) 
*\param *param :
*/
void *io_handler(void *param);


/**
*\fn void *client_handler(void *fd)
*\brief Fonction lié aux thread pour établir une communication et capturer les commandes d'un client
*\param *fd : une struct threadinfo qui contient le numero de socket du client et son pseudo
*/
void *client_handler(void *fd);


/**
*\fn struct T_FriendList getAmi(char *nom)
*\brief Fonction retournant la liste des amis connectés d'un client dont le nom est *nom
*\param *nom : Nom du client dont on cherche la liste d'amis connectés
*\return ( struct T_FriendList ) la list d'amis d'un client passé en arg
*/
struct T_FriendList getAmi(char *nom);


/**
*\fn void affichermenu() 
*\brief Fonction qui affiche le menu et les commandes possible
*/
void affichermenu() 
{ 
            
printf("\n"); 
printf("\tBBBBB   YY   YY TTTTTTT EEEEEEE  CCCCC  HH   HH   AAA   TTTTTTT \n"); 
printf("\tBB   B  YY   YY   TTT   EE      CC    C HH   HH  AAAAA    TTT   \n"); 
printf("\tBBBBBB   YYYYY    TTT   EEEEE   CC      HHHHHHH AA   AA   TTT   \n"); 
printf("\tBB   BB   YYY     TTT   EE      CC    C HH   HH AAAAAAA   TTT   \n"); 
printf("\tBBBBBB    YYY     TTT   EEEEEEE  CCCCC  HH   HH AA   AA   TTT \n"); 
           
           
           
            printf("\t**************\\ ACHRAF DAHMANI - REMI DUMINY / **************\n\n"); 
            printf("\t\t********     BIENVENUE SUR BYTECHAT     ********\n"); 
            printf("\t| COMMANDES - \n"); 
            printf("\tlist : pour voir les clients connecté sur le serveur\n"); 
            printf("\texit pour quitter le programe.\n"); 
           
} 

int main(int argc, char **argv) {
    int err_ret, sin_size;
    struct sockaddr_in serv_addr, client_addr;
    pthread_t interrupt;
	affichermenu();

    /* initialisation du linked list qui contiendra la liste des clients connecté */
    list_init(&client_list);
 
    /* inistialisation du mutex qui protegera la liste  */
    pthread_mutex_init(&clientlist_mutex, NULL);
 
    /* ouverture socket */
    CHECK(sockfd = socket(AF_INET, SOCK_STREAM, 0),"erreur création du socket");
 
    /* initialisation des valeurs */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr(IP);
    memset(&(serv_addr.sin_zero), 0, 8);
    
    /* bind address with socket */
     CHECK(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) , "erreur bind()");
 
     CHECK(listen(sockfd, BACKLOG),"erreur listen()");
    
    printf("\n(*) Lancement d'interface d'administration...\n");

    /* initialisation d'interrupt handler pour le controle d'entrée/sortie */
    CHECK(pthread_create(&interrupt, NULL, io_handler, NULL),"erreur creation thread");
 
    printf("\n(*) Le Serveur est prêt à accepter des clients ! \n\n");

    while(1) {
        sin_size = sizeof(struct sockaddr_in);

        newfd = accept(sockfd, (struct sockaddr *)&client_addr, (socklen_t*)&sin_size);
        
            if(client_list.size == CLIENTS) {
                fprintf(stderr, "Serveur full, requete rejeté...\n");
                continue;
            }
            printf("Demmande de connexion recu...\n");
            struct THREADINFO threadinfo;
            threadinfo.sockfd = newfd;
            strcpy(threadinfo.alias, "Anonymous");
            pthread_mutex_lock(&clientlist_mutex);
            list_insert(&client_list, &threadinfo);
            pthread_mutex_unlock(&clientlist_mutex);
            pthread_create(&threadinfo.thread_ID, NULL, client_handler, (void *)&threadinfo);
        
    }
 
    return 0;
}
 


void *io_handler(void *param) {
    char option[OPTLEN];
    struct T_FriendList TFL;
    while(scanf("%s", option)==1) {
        if(!strcmp(option, "exit")) {
            /* clean up */
            printf("fermeture du server...\n");
            pthread_mutex_destroy(&clientlist_mutex);
            close(sockfd);
            exit(0);
        }
        else if(!strcmp(option, "list")) { // list des clients connecté sur le serveur
            pthread_mutex_lock(&clientlist_mutex);
            list_dump(&client_list);
            pthread_mutex_unlock(&clientlist_mutex);
        }
        else if(!strncmp(option, "getami", 6)) { // fonction de test de la liste des amis connecté et non connecté du client_1
            char *ptr = strtok(option, " ");
            ptr = strtok(0, " ");
            pthread_mutex_lock(&clientlist_mutex);
            TFL = getAmi("client_1");
            pthread_mutex_unlock(&clientlist_mutex);
    }
       
        else 
        {
            fprintf(stderr, "commande inconnu: %s...\n", option);
        }
    }
    return NULL;
}
 
void *client_handler(void *fd) {
    struct THREADINFO threadinfo = *(struct THREADINFO *)fd;
    struct PACKET packet;
    struct T_FriendList TFL;
    struct LLNODE *curr;
    Client clt1;
    int bytes, sent;
    while(1) {
        bytes = recv(threadinfo.sockfd, (void *)&packet, sizeof(struct PACKET), 0);
        if(!bytes) {
            fprintf(stderr, "Connection perdu from [%d] %s...\n", threadinfo.sockfd, threadinfo.alias);
            pthread_mutex_lock(&clientlist_mutex);
            list_delete(&client_list, &threadinfo);
            pthread_mutex_unlock(&clientlist_mutex);
            break;
        }
        
        if(!strcmp(packet.option, "alias")) {
            clt1 = Search_Client_By_Name((char *)packet.alias);
            if(clt1.idClient > 0)
            {
                if(!strcmp(clt1.mdpClient,(char *)packet.buff )) 
                {
                    printf("Logging user : [%s] avec password : %s\n", packet.alias, clt1.mdpClient);
                    pthread_mutex_lock(&clientlist_mutex);
                    for(curr = client_list.head; curr != NULL; curr = curr->next)
                    {
                        if(compare(&curr->threadinfo, &threadinfo) == 0)
                        {
                            strcpy(curr->threadinfo.alias, packet.alias);
                            strcpy(threadinfo.alias, packet.alias);
                            break;
                        }
                    }
                    pthread_mutex_unlock(&clientlist_mutex);
                    strcpy(packet.buff, "ok");
                    sent = send(threadinfo.sockfd, (void *)&packet, sizeof(struct PACKET), 0);

                }
                else{
                strcpy(packet.buff, "nok");
                sent = send(threadinfo.sockfd, (void *)&packet, sizeof(struct PACKET), 0);
                }
            }
            else{
                strcpy(packet.buff, "nok");
                sent = send(threadinfo.sockfd, (void *)&packet, sizeof(struct PACKET), 0);
                }
        }
        
        
        else if(!strcmp(packet.option, "signup")) {
            int client;
            client = Ajouter_Client((char *)packet.alias,(char *)packet.buff);
            if(client > 0)
            {
                
                    printf("user [%s] à été ajouté avec succes dans la base des données\n", packet.alias);
                    strcpy(packet.buff, "ok");
                    sent = send(threadinfo.sockfd, (void *)&packet, sizeof(struct PACKET), 0);

            }
            else{
                strcpy(packet.buff, "nok");
                sent = send(threadinfo.sockfd, (void *)&packet, sizeof(struct PACKET), 0);
                }
        }
        
        else if(!strcmp(packet.option, "addfriend")) {
            int result;
        
            //printf("moi : %s , myfriend : %s",(char *)threadinfo.alias,(char *)packet.alias);
            result = Ajouter_Ami((char *)packet.alias,(char *)packet.buff);
            if(result > 0)
            {
                    //strcpy(packet.alias, packet.buff);
                    printf("le client [%s] est maintenant amis avec [%s]\n", packet.buff,packet.alias);
                    strcpy(packet.alias, "ok");
                    sent = send(threadinfo.sockfd, (void *)&packet, sizeof(struct PACKET), 0);
                    

            }
            else{
                strcpy(packet.buff, "nok");
                sent = send(threadinfo.sockfd, (void *)&packet, sizeof(struct PACKET), 0);
                }
        }
        else if(!strcmp(packet.option, "delfriend")) {
            int result;
        
            //printf("moi : %s , myfriend : %s",(char *)threadinfo.alias,(char *)packet.alias);
            result = Supprimer_Ami((char *)packet.alias,(char *)packet.buff);
            if(result > 0)
            {
                    //strcpy(packet.alias, packet.buff);
                    printf("le client [%s] n'est plus dans les amis de [%s]\n", packet.buff,packet.alias);
                    strcpy(packet.alias, "ok");
                    sent = send(threadinfo.sockfd, (void *)&packet, sizeof(struct PACKET), 0);
                    

            }
            else{
                strcpy(packet.buff, "nok");
                sent = send(threadinfo.sockfd, (void *)&packet, sizeof(struct PACKET), 0);
                }
        }
        
        
        
        
        else if(!strcmp(packet.option, "getami")) {
            
            pthread_mutex_lock(&clientlist_mutex);
            TFL = getAmi((char *)packet.alias);
            pthread_mutex_unlock(&clientlist_mutex);
            printf("l'envoi du friendlist à %s\n", threadinfo.alias);
 
            sent = send(threadinfo.sockfd, (void *)&TFL, sizeof(struct T_FriendList), 0);
        
            
          
        }
        

        else if(!strcmp(packet.option, "whisp")) {
            int i;
            char target[ALIASLEN];
            for(i = 0; packet.buff[i] != ' '; i++); packet.buff[i++] = 0;
            strcpy(target, packet.buff);
            pthread_mutex_lock(&clientlist_mutex);
            for(curr = client_list.head; curr != NULL; curr = curr->next) {
                if(strcmp(target, curr->threadinfo.alias) == 0) {
                    struct PACKET spacket;
                    memset(&spacket, 0, sizeof(struct PACKET));
                    if(!compare(&curr->threadinfo, &threadinfo)) continue;
                    strcpy(spacket.option, "msg");
                    strcpy(spacket.alias, packet.alias);
                    strcpy(spacket.buff, &packet.buff[i]);
                    sent = send(curr->threadinfo.sockfd, (void *)&spacket, sizeof(struct PACKET), 0);
                }
            }
            pthread_mutex_unlock(&clientlist_mutex);
        }
        else if(!strcmp(packet.option, "send")) {
            pthread_mutex_lock(&clientlist_mutex);
            for(curr = client_list.head; curr != NULL; curr = curr->next) {
                struct PACKET spacket;
                memset(&spacket, 0, sizeof(struct PACKET));
                if(!compare(&curr->threadinfo, &threadinfo)) continue;
                strcpy(spacket.option, "msg");
                strcpy(spacket.alias, packet.alias);
                strcpy(spacket.buff, packet.buff);
                sent = send(curr->threadinfo.sockfd, (void *)&spacket, sizeof(struct PACKET), 0);
            }
            pthread_mutex_unlock(&clientlist_mutex);
        }
        else if(!strcmp(packet.option, "exit")) {
            printf("[%d] %s has disconnected...\n", threadinfo.sockfd, threadinfo.alias);
            pthread_mutex_lock(&clientlist_mutex);
            close(threadinfo.sockfd);
            list_delete(&client_list, &threadinfo);
            pthread_mutex_unlock(&clientlist_mutex);
            pthread_exit(&threadinfo.thread_ID);
            break;
        }
        else {
            fprintf(stderr, "Garbage : \n %s \n from [%d] %s...\n", packet.buff, threadinfo.sockfd, threadinfo.alias);
        }
        printf("[%d] %s %s %s\n", threadinfo.sockfd, packet.option, packet.alias, packet.buff);
        
    }
 
    /* clean up */
    close(threadinfo.sockfd);
 
    return NULL;
}

struct T_FriendList getAmi(char *nom)
{
    int i,j;
    Client C;
    Client C_aux;
    struct T_FriendList TFL;
    TFL.nbAmis = -1;
    struct LLNODE *curr;
    struct THREADINFO *thr_info;
    
    
    
    C = Search_Client_By_Name(nom);
    if(C.idClient > 0)
    {
        for(j=0;j<=C.amis->nbAmis;j++)
        {
                C_aux = Search_Client_By_Id(C.amis->idAmis[j]);
                TFL.nbAmis++;
                strcpy(TFL.amiconnecte[j].nomAmis,C_aux.nomClient); 
                TFL.amiconnecte[j].online = 0 ;

                for(curr = client_list.head; curr != NULL; curr = curr->next) 
                {
                    
                    thr_info = &curr->threadinfo;
                    if(strcmp(thr_info->alias,TFL.amiconnecte[j].nomAmis)==0){
                        TFL.amiconnecte[j].online = 1 ;
                        }    
                    
                }
        
        }   
    }
    for(i=0;i<=TFL.nbAmis;i++)
    {
        printf("\n-------------------------\n");
        printf("Ami num %d | nom : %s | etat : %d \n",i, TFL.amiconnecte[i].nomAmis,TFL.amiconnecte[i].online);             
    }
    return TFL;
    
}



