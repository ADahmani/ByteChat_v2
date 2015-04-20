/**
*\file client.c
*\brief Fichier contenant le code du client
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
#include "fonction_client.h"
#include "fonction_server.h"
#include "fonction_ncurses.h"

#define CHECK(sts,msg) if ((sts) == -1){ perror(msg); exit(-1);  }

#define MAXBUFF 1024
#define SERVERIP "127.0.0.1"
#define SERVERPORT 5555

#define LINEBUFF 2048


int isconnected, sockfd;
char option[LINEBUFF];
struct USER me;

//variables d'interface graphique
WINDOW *clients_conn;
WINDOW *chatbox;
WINDOW *commandeline;
int commandelinehight = 5;
int clientsconwidth = 20;



/**
*\fn void affichermenu(void)
*\brief affichage de menu principal
*/
void affichermenu(void);

/**
*\fn int connect_with_server()
*\brief Etablisement d'une connexion avec le serveur
*/
int connect_with_server(void);

/**
*\fn logout(struct USER *me)
*\brief déconnecter un client
*\param *me : structure USER qui contiens les infomation sur le client courant au thread appelent la fonction
*/
void logout(struct USER *me);
/**
*\fn sendtoall(struct USER *me, char *msg)
*\brief envoi un message en broadcast
*\param *me : le client qui à envoyé le message, *msg : le message à envoyer
*/
void sendtoall(struct USER *me, char *msg);
/**
*\fn sendtoalias(struct USER *me, char *target, char *msg)
*\brief envoi un message à un client 
*\param *me : le client source, *target : client distinataire, *msg : le message à envoyer
*/
void sendtoalias(struct USER *me, char *target, char *msg);
/**
*\fn Addfriend(struct USER *me, char *target)
*\brief ajouter un client à sa liste des amis
*\param *me : client demandant la requete d'ajoute, *target : le client à ajouter
*/
void Addfriend(struct USER *me, char *target);
/**
*\fn setalias(struct USER *me)
*\brief envoi la demande de connexion d'un client
*\param *me : une structure USER qui contien le nom et mot de passe du client à connecter
*/
void setalias(struct USER *me);
/**
*\fn signup(struct USER *me)
*\brief inscription d'un client
*\param *me : une structure USER qui contien le nom et mot de passe du client à inscrire
*/
void signup(struct USER *me);
/**
*\fn void *receiver(void *param)
*\brief Fonction lié au thread qui à 2 roles; le premier est l'ecoute en permanant le serveur et interprete les reponses
*envoyé par le serveur; et son deusieme role est la gestion de l'affichage ncurses
*/
void *receiver(void *param);
/**
*\fn void login(struct USER *me)
*\brief Demande de connection au serveur, pour permetre ensuite d'envoyer des commandes
*\param *me : une structure USER qui contien un nom anonymouse et mot de passe anonymouse
*/
void login(struct USER *me);


int connect_with_server() {
	char addr[16];
	int port = 5555;
	int servlen = 0;
	struct sockaddr_in serv;
    int newfd;

	strcpy(addr, SERVERIP);
	CHECK(newfd = socket(AF_INET, SOCK_STREAM,0), "erreur creaction socket");

	servlen = sizeof(serv);
	
	
	
    /* open a socket */
	CHECK(newfd = socket(AF_INET, SOCK_STREAM,0), "socket() error...\n");
	
    /* initialisation des valeurs */
	serv.sin_addr.s_addr = inet_addr(addr);
	serv.sin_family = AF_INET;
	serv.sin_port = htons(port);
	memset(&serv.sin_zero,0,8);
 
    /* connect avec le server */
	servlen = sizeof(serv);
	CHECK(connect(newfd,(struct sockaddr *)&serv, servlen),"connect error ...");
 
        return newfd;

}
 
 
void logout(struct USER *me) {
    int sent;
    struct PACKET packet;
    
    if(!isconnected) {
        wattron(chatbox, A_BOLD | COLOR_PAIR(1));
		mvwprintw(chatbox,LINES-commandelinehight-1,1,"vous n'êtes pas connnecté au serveur...\n");
		wattroff(chatbox, A_BOLD | COLOR_PAIR(1));
		wrefresh(chatbox);
		wclear(commandeline);
		wborder(commandeline, 0, 0, 0, 0, 0, 0, 0, 0);
        return;
    }
    
    memset(&packet, 0, sizeof(struct PACKET));
    strcpy(packet.option, "exit");
    strcpy(packet.alias, me->alias);
    
    /* send request to close this connetion */
    sent = send(sockfd, (void *)&packet, sizeof(struct PACKET), 0);
    isconnected = 0;
}
 
 void sendtoall(struct USER *me, char *msg) {
    int sent;
    struct PACKET packet;
    
	if(msg == NULL) {
        return;
    }

    if(!isconnected) {
        wattron(chatbox, A_BOLD | COLOR_PAIR(1));
		mvwprintw(chatbox,LINES-commandelinehight-1,1,"vous n'êtes pas connnecté au serveur...\n");
		wattroff(chatbox, A_BOLD | COLOR_PAIR(1));
        return;
    }
    
    msg[BUFFSIZE] = 0;
    
    memset(&packet, 0, sizeof(struct PACKET));
    strcpy(packet.option, "send");
    strcpy(packet.alias, me->alias);
    strcpy(packet.buff, msg);
    
    /* send request to close this connetion */
    sent = send(sockfd, (void *)&packet, sizeof(struct PACKET), 0);
}
 
void sendtoalias(struct USER *me, char *target, char *msg) {
    int sent, targetlen;
    struct PACKET packet;
    
    if(target == NULL) {
        return;
    }
    
    if(msg == NULL) {
        return;
    }
    
    if(!isconnected) {
        wattron(chatbox, A_BOLD | COLOR_PAIR(1));
		mvwprintw(chatbox,LINES-commandelinehight-1,1,"vous n'êtes pas connnecté au serveur...\n");
		wattroff(chatbox, A_BOLD | COLOR_PAIR(1));
		wrefresh(chatbox);
		wclear(chatbox);
		wborder(commandeline, 0, 0, 0, 0, 0, 0, 0, 0);
        return;
    }
    msg[BUFFSIZE] = 0;
    targetlen = strlen(target);
    
    memset(&packet, 0, sizeof(struct PACKET));
    strcpy(packet.option, "whisp");
    strcpy(packet.alias, me->alias);
    strcpy(packet.buff, target);
    strcpy(&packet.buff[targetlen], " ");
    strcpy(&packet.buff[targetlen+1], msg);
    
    /* send request to close this connetion */
    sent = send(sockfd, (void *)&packet, sizeof(struct PACKET), 0);
}

void Addfriend(struct USER *me, char *target) {
    int sent, targetlen;
    struct PACKET packet;
    
    if(target == NULL) {
    	wattron(chatbox, A_BOLD | COLOR_PAIR(1));
		mvwprintw(chatbox,LINES-commandelinehight-1,1,"La forme correct est : /addfriend username\n");
		wattroff(chatbox, A_BOLD | COLOR_PAIR(1));
		wrefresh(chatbox);
		wclear(commandeline);
		wborder(commandeline, 0, 0, 0, 0, 0, 0, 0, 0);
        return;
    }
    
    if(!isconnected) {
        wclear(commandeline);
		wborder(commandeline, ' ', ' ', 0, 0, 0, 0, 0, 0);
		mvwprintw(chatbox,LINES-commandelinehight-1,1,"Vous n'êtes pas connecté...\n");	
		wrefresh(chatbox);
		wclear(commandeline);
		wborder(commandeline, 0, 0, 0, 0, 0, 0, 0, 0);
        return;
    }

    memset(&packet, 0, sizeof(struct PACKET));
    strcpy(packet.option, "addfriend");
    strcpy(packet.alias, me->alias);
    strcpy(packet.buff, target);
    
    /* send request to close this connetion */
    sent = send(sockfd, (void *)&packet, sizeof(struct PACKET), 0);
}
 

void Delfriend(struct USER *me, char *target) {
    int sent, targetlen;
    struct PACKET packet;
    
    if(target == NULL) {
    	wattron(chatbox, A_BOLD | COLOR_PAIR(1));
		mvwprintw(chatbox,LINES-commandelinehight-1,1,"La forme correct est : /delfriend username\n");
		wattroff(chatbox, A_BOLD | COLOR_PAIR(1));
		wrefresh(chatbox);
		wclear(commandeline);
		wborder(commandeline, 0, 0, 0, 0, 0, 0, 0, 0);
        return;
    }
    
    if(!isconnected) {
        wclear(commandeline);
		wborder(commandeline, ' ', ' ', 0, 0, 0, 0, 0, 0);
		mvwprintw(chatbox,LINES-commandelinehight-1,1,"Vous n'êtes pas connecté...\n");	
		wrefresh(chatbox);
		wclear(commandeline);
		wborder(commandeline, 0, 0, 0, 0, 0, 0, 0, 0);
        return;
    }

    memset(&packet, 0, sizeof(struct PACKET));
    strcpy(packet.option, "delfriend");
    strcpy(packet.alias, me->alias);
    strcpy(packet.buff, target);
    
    /* send request to close this connetion */
    sent = send(sockfd, (void *)&packet, sizeof(struct PACKET), 0);
}



void setalias(struct USER *me) {
    int sent;
    struct PACKET packet;
    
    if(!isconnected) {
        wclear(commandeline);
		wborder(commandeline, ' ', ' ', 0, 0, 0, 0, 0, 0);
		mvwprintw(chatbox,LINES-commandelinehight-1,1,"Vous n'êtes pas connecté...\n");	
		wrefresh(chatbox);
		wclear(commandeline);
		wborder(commandeline, 0, 0, 0, 0, 0, 0, 0, 0);
        return;
    }
    
    memset(&packet, 0, sizeof(struct PACKET));
    strcpy(packet.option, "alias");
    strcpy(packet.alias, me->alias);
    strcpy(packet.buff, me->pwd);
    sent = send(sockfd, (void *)&packet, sizeof(struct PACKET), 0); 
}

void signup(struct USER *me) {
    int sent;
    struct PACKET packet;
    
    memset(&packet, 0, sizeof(struct PACKET));
    strcpy(packet.option, "signup");
    strcpy(packet.alias, me->alias);
    strcpy(packet.buff, me->pwd);
    sent = send(sockfd, (void *)&packet, sizeof(struct PACKET), 0);
}


 
void *receiver(void *param) {
	
	int x, y, i;
    int recvd,sent;
	struct T_FriendList TFL;
    struct PACKET packet;
    
   // printf("Waiting here [%d]...\n", sockfd);
    if(isconnected)
    {
      wattron(chatbox, A_BOLD | COLOR_PAIR(2));
      mvwprintw(chatbox,LINES-commandelinehight-1,1,"Le serveur vous écoute ! \n");
      wattroff(chatbox, A_BOLD | COLOR_PAIR(2));
    }

    //mvwprintw(chatbox,LINES-commandelinehight-1,1,"waiting here..");
	wrefresh(chatbox);
    while(isconnected) {
        
        recvd = recv(sockfd, (void *)&packet, sizeof(struct PACKET), 0);
        if(!recvd) {
            wattron(chatbox, A_BOLD | COLOR_PAIR(1));
            mvwprintw(chatbox,LINES-commandelinehight-1,1,"Connection lost from server...\n");
            wattroff(chatbox, A_BOLD | COLOR_PAIR(1));
			wrefresh(chatbox);
          
            isconnected = 0;
            close(sockfd);
            break;
        }
        if(recvd > 0) 
        {
		
		
			wrefresh(chatbox);
			scrollok(chatbox,TRUE);
			
			wrefresh(commandeline);
			scrollok(commandeline,TRUE);
			
			wclear(commandeline);
			wborder(commandeline, ' ', ' ', 0, 0, 0, 0, 0, 0);
			if(!strncmp(packet.option,"msg",3)) // un message d'un client
			{
				mvwprintw(chatbox,LINES-commandelinehight-1,1,"%s : %s\n",packet.alias,packet.buff);
			}

			else if(!strncmp(packet.option,"alias",5)) // reponse de connexion
			{
				 if(!strncmp(packet.buff,"ok",2))
				 {
					
					wattron(chatbox, A_BOLD | COLOR_PAIR(2));
					mvwprintw(chatbox,LINES-commandelinehight-1,1,"Connecté tant que [%s] !\n",packet.alias);
					wattroff(chatbox, A_BOLD | COLOR_PAIR(2));

					strcpy(packet.option,"getami");
					strcpy(packet.alias,me.alias);
					sent = send(me.sockfd, (void *)&packet, sizeof(struct PACKET), 0);
					recvd = recv(me.sockfd, (void *)&TFL, sizeof(struct T_FriendList), 0);
					x=2;
					y=2;
					wclear(clients_conn);
					
					for(i=0;i<=TFL.nbAmis;i++)
					{	
						if(TFL.amiconnecte[i].online==1)
						{
							wattron(clients_conn, A_BOLD | COLOR_PAIR(2));
							mvwprintw(clients_conn,y,x,"%s\n",TFL.amiconnecte[i].nomAmis);
							wattroff(clients_conn, A_BOLD | COLOR_PAIR(2));
						}else{
						mvwprintw(clients_conn,y,x,"%s\n",TFL.amiconnecte[i].nomAmis);
						}
						y++;	
					}
					wborder(clients_conn, 0, 0, 0, 0, 0, 0, 0, 0);
					wrefresh(clients_conn);	
					
				 }
				 else if(!strncmp(packet.buff,"nok",3))
				 {
						wattron(chatbox, A_BOLD | COLOR_PAIR(1));
						mvwprintw(chatbox,LINES-commandelinehight-1,1,"username ou mot de passe invalide !\n",packet.alias);
						wattroff(chatbox, A_BOLD | COLOR_PAIR(1));
						wrefresh(chatbox);
						wclear(commandeline);
						wborder(commandeline, 0, 0, 0, 0, 0, 0, 0, 0);
						
		   
				 }
				
			}
			
			else if(!strncmp(packet.option,"signup",6)) // reponse de connexion
			{
				 if(!strncmp(packet.buff,"ok",2))
				 {
					
					wattron(chatbox, A_BOLD | COLOR_PAIR(2));
					mvwprintw(chatbox,LINES-commandelinehight-1,1,"L'enregistrement de [%s] à été fait avec sucess!\n",packet.alias);
					wattroff(chatbox, A_BOLD | COLOR_PAIR(2));
					wrefresh(chatbox);
					wclear(commandeline);
					wborder(commandeline, 0, 0, 0, 0, 0, 0, 0, 0);
					
				 }
				 else if(!strncmp(packet.buff,"nok",3))
				 {
						wattron(chatbox, A_BOLD | COLOR_PAIR(1));
						mvwprintw(chatbox,LINES-commandelinehight-1,1,"erreur d'enregistrement de [%s]!\n",packet.alias);
						wattroff(chatbox, A_BOLD | COLOR_PAIR(1));
				 }
				
			}
			
			else if(!strncmp(packet.option,"addfriend",9)) // reponse de connexion
			{
				 if(!strncmp(packet.alias,"ok",2))
				 {
					
					wattron(chatbox, A_BOLD | COLOR_PAIR(2));
					mvwprintw(chatbox,LINES-commandelinehight-1,1,"le client [%s] à été ajouté à votre liste d'amis!\n",packet.buff);
					wattroff(chatbox, A_BOLD | COLOR_PAIR(2));
					
					strcpy(packet.option,"getami");
					strcpy(packet.alias,me.alias);
					sent = send(me.sockfd, (void *)&packet, sizeof(struct PACKET), 0);
					recvd = recv(me.sockfd, (void *)&TFL, sizeof(struct T_FriendList), 0);
					x=2;
					y=2;
					wclear(clients_conn);
					for(i=0;i<=TFL.nbAmis;i++)
					{	
						
						if(TFL.amiconnecte[i].online==1)
						{
							wattron(clients_conn, A_BOLD | COLOR_PAIR(2));
							mvwprintw(clients_conn,y,x,"%s\n",TFL.amiconnecte[i].nomAmis);
							wattroff(clients_conn, A_BOLD | COLOR_PAIR(2));
							
						}
						else
						{
							mvwprintw(clients_conn,y,x,"%s\n",TFL.amiconnecte[i].nomAmis);
							
						}
						
						y++;	
					}
					wborder(clients_conn, 0, 0, 0, 0, 0, 0, 0, 0);
					wrefresh(clients_conn);	
					
				 }
				 else if(!strncmp(packet.alias,"nok",3))
				 {
						wattron(chatbox, A_BOLD | COLOR_PAIR(1));
						mvwprintw(chatbox,LINES-commandelinehight-1,1,"erreur d'enregistrement de [%s]!\n",packet.alias);
						wattroff(chatbox, A_BOLD | COLOR_PAIR(1));
						
						
				 }
				
			}


			else if(!strncmp(packet.option,"delfriend",9)) // reponse de la requete delfriend
			{
				 if(!strncmp(packet.alias,"ok",2))
				 {
					
					wattron(chatbox, A_BOLD | COLOR_PAIR(2));
					mvwprintw(chatbox,LINES-commandelinehight-1,1,"le client [%s] à été supprimé de votre liste d'amis!\n",packet.buff);
					wattroff(chatbox, A_BOLD | COLOR_PAIR(2));
					
					strcpy(packet.option,"getami");
					strcpy(packet.alias,me.alias);
					sent = send(me.sockfd, (void *)&packet, sizeof(struct PACKET), 0);
					recvd = recv(me.sockfd, (void *)&TFL, sizeof(struct T_FriendList), 0);
					x=2;
					y=2;
					wclear(clients_conn);
					for(i=0;i<=TFL.nbAmis;i++)
					{	
						
						if(TFL.amiconnecte[i].online==1)
						{
							wattron(clients_conn, A_BOLD | COLOR_PAIR(2));
							mvwprintw(clients_conn,y,x,"%s\n",TFL.amiconnecte[i].nomAmis);
							wattroff(clients_conn, A_BOLD | COLOR_PAIR(2));
							
						}
						else
						{
							mvwprintw(clients_conn,y,x,"%s\n",TFL.amiconnecte[i].nomAmis);
							
						}
						
						y++;	
					}
					wborder(clients_conn, 0, 0, 0, 0, 0, 0, 0, 0);
					wrefresh(clients_conn);	
					
				 }
				 else if(!strncmp(packet.alias,"nok",3))
				 {
						wattron(chatbox, A_BOLD | COLOR_PAIR(1));
						mvwprintw(chatbox,LINES-commandelinehight-1,1,"erreur de suppresion de [%s]!\n",packet.alias);
						wattroff(chatbox, A_BOLD | COLOR_PAIR(1));
						
						
				 }
				
			}
        }
        wrefresh(chatbox);
        wrefresh(clients_conn);
        wrefresh(commandeline);
        memset(&packet, 0, sizeof(struct PACKET));
        memset(&TFL, 0, sizeof(struct T_FriendList));
    }
    return NULL;
}
 
  
void login(struct USER *me) {
    int recvd;
    if(isconnected) {
    	wclear(commandeline);
		wborder(commandeline, ' ', ' ', 0, 0, 0, 0, 0, 0);
		mvwprintw(chatbox,LINES-commandelinehight-1,1,"Vous êtes déja connecté au serveur at...%s:%d\n",SERVERIP, SERVERPORT);	
        wrefresh(chatbox);
		wclear(commandeline);
		wborder(commandeline, 0, 0, 0, 0, 0, 0, 0, 0);
        return;
    }
    sockfd = connect_with_server();
    if(sockfd >= 0) {
        isconnected = 1;
        me->sockfd = sockfd;
        if(strcmp(me->alias, "Anonymous")){
         setalias(me);
	 }
		
       
       // printf("Receiver started [%d]...\n", sockfd);
        struct THREADINFO threadinfo;
        pthread_create(&threadinfo.thread_ID, NULL, receiver, (void *)&threadinfo);
 
    }
    else {
    	wclear(commandeline);
		wborder(commandeline, ' ', ' ', 0, 0, 0, 0, 0, 0);
		wattron(chatbox, A_BOLD | COLOR_PAIR(1));
		mvwprintw(chatbox,LINES-commandelinehight-1,1,"Connexion échoué\n");	
		wattroff(chatbox, A_BOLD | COLOR_PAIR(1));
		wrefresh(chatbox);
		wclear(commandeline);
		wborder(commandeline, 0, 0, 0, 0, 0, 0, 0, 0);
       
    }
}




int main(int argc, char* argv[])
{
	
	char buff[MAXBUFF], addr[16];
	char pseudo[10];
	int s = 0;
	int port = 5555;
	int servlen = 0, aliaslen;	
	struct sockaddr_in serv;
	
	// User interface :
	
	//Adresse IP Locale
	strcpy(addr, "127.0.0.1");

	//
	// runtime
	
	
	
	
	
	//printf("\nL'adresse de connexion est : %s \n ",addr);	
	//printf("\nLe port est : %d \n",port);
	char choix[10]; 
	while(1)
	   {	
			system("clear");
			affichermenu();
			printf("\n");
			printf("$-: ");
			scanf("%s", choix);
			if(strcmp (choix,"/quit") == 0 || strcmp (choix,"/launch") == 0)
			{
			   break;
			}
			getchar();
	   }

		if(!strncmp(choix, "/quit", 5)) {
            exit(0);
        }


	CHECK(s = socket(AF_INET, SOCK_STREAM,0), "erreur creation socket");
	serv.sin_addr.s_addr = inet_addr(addr);
	serv.sin_family = AF_INET;
	serv.sin_port = htons(port);
	memset(&serv.sin_zero,0,8);
	servlen = sizeof(serv);	
	printf("Chargement d'interface graphique...");
	
	// l'interface graphique 
	initfenetres(); 
	refresh();
	  //creations de fenetres 
		clients_conn = create_newwin(LINES-commandelinehight, clientsconwidth, 0, 0);
		chatbox = create_newwin(LINES-commandelinehight, COLS - clientsconwidth, 0, clientsconwidth);
		commandeline = create_newwin(commandelinehight, COLS, LINES-commandelinehight, 0);
	  //parametres de fenetre de commandeline
		wsetscrreg(commandeline,1,commandelinehight-2);
		wborder(commandeline, ' ', ' ', 0, 0, 0, 0, 0, 0);
		wrefresh(commandeline);
		scrollok(commandeline,TRUE);
	  //parametres de fenetre de chatbox
		wsetscrreg(chatbox,1,LINES - commandelinehight-1);
		wborder(chatbox, ' ', ' ', 0, ' ', 0, 0, ' ', ' ');
		//login(&me);
		
	while(1)
	{
		
	
		wrefresh(chatbox);
		wrefresh(clients_conn);
		scrollok(chatbox,TRUE);
		
		wrefresh(commandeline);
		scrollok(commandeline,TRUE);
		mvwgetstr(commandeline,1,1,buff);
		
		
		
		
		  if(!strncmp(buff, "/quit", 5)) {
            logout(&me);
            break;
        }
        
        else if(!strncmp(buff, "/connect", 8)) {
			wclear(commandeline);
            wborder(commandeline, ' ', ' ', 0, 0, 0, 0, 0, 0);
            mvwprintw(chatbox,LINES-commandelinehight-1,1,"Connection en cours ...\n");
            wrefresh(chatbox);
            char *ptr = strtok(buff, " ");
            ptr = strtok(0, " ");
            memset(me.alias, 0, sizeof(char) * ALIASLEN);
            if(ptr != NULL) {
                aliaslen =  strlen(ptr);
                if(aliaslen > ALIASLEN) ptr[ALIASLEN] = 0;
                strcpy(me.alias, ptr);
            }
            else {
                strcpy(me.alias, "Anonymous");
                strcpy(me.pwd, "Anonymous");
            }
            login(&me);
        }
        else if(!strncmp(buff, "/login", 6)) {
			wclear(commandeline);
            wborder(commandeline, ' ', ' ', 0, 0, 0, 0, 0, 0);
            mvwprintw(chatbox,LINES-commandelinehight-1,1,"Identification en cours ...\n");
            wrefresh(chatbox);
            char *ptr = strtok(buff, " ");
            char temp[ALIASLEN];
            ptr = strtok(0, " ");
            memset(temp, 0, sizeof(char) * ALIASLEN);
            if(ptr != NULL) {
                aliaslen =  strlen(ptr);
                if(aliaslen > ALIASLEN) ptr[ALIASLEN] = 0;
                strcpy(temp, ptr);
                while(*ptr) ptr++; ptr++;
                while(*ptr <= ' ') ptr++;
                strcpy(me.alias,temp);
                strcpy(me.pwd,ptr);
                setalias(&me);
            }

        }
        else if(!strncmp(buff, "/signup", 7)) {
			wclear(commandeline);
            wborder(commandeline, ' ', ' ', 0, 0, 0, 0, 0, 0);
            mvwprintw(chatbox,LINES-commandelinehight-1,1,"Enregistrement en cours ...\n");
            wrefresh(chatbox);
            char *ptr = strtok(buff, " ");
            char temp[ALIASLEN];
            ptr = strtok(0, " ");
            memset(temp, 0, sizeof(char) * ALIASLEN);
            if(ptr != NULL) {
                aliaslen =  strlen(ptr);
                if(aliaslen > ALIASLEN) ptr[ALIASLEN] = 0;
                strcpy(temp, ptr);
                while(*ptr) ptr++; ptr++;
                while(*ptr <= ' ') ptr++;
                strcpy(me.alias,temp);
                strcpy(me.pwd,ptr);
                signup(&me);
            }

        }
        else if(!strncmp(buff, "/whisp", 6)) {
            char *ptr = strtok(buff, " ");
            char temp[ALIASLEN];
            ptr = strtok(0, " ");
            memset(temp, 0, sizeof(char) * ALIASLEN);
            if(ptr != NULL) {
                aliaslen =  strlen(ptr);
                if(aliaslen > ALIASLEN) ptr[ALIASLEN] = 0;
                strcpy(temp, ptr);
                while(*ptr) ptr++; ptr++;
                while(*ptr <= ' ') ptr++;
                sendtoalias(&me, temp, ptr);
            }
            wclear(commandeline);
			wborder(commandeline, ' ', ' ', 0, 0, 0, 0, 0, 0);
			mvwprintw(chatbox,LINES-commandelinehight-1,1,"%s : [->%s] - %s\n",&me.alias,temp,ptr);
			
        }
        else if(!strncmp(buff, "/addfriend", 10)) {
            wclear(commandeline);
            wborder(commandeline, ' ', ' ', 0, 0, 0, 0, 0, 0);
            char *ptr = strtok(buff, " ");
            char temp[ALIASLEN];
            ptr = strtok(0, " ");
            memset(temp, 0, sizeof(char) * ALIASLEN);
            if(ptr != NULL) {
                aliaslen =  strlen(ptr);
                if(aliaslen > ALIASLEN) ptr[ALIASLEN] = 0;
                strcpy(temp, ptr);
                while(*ptr) ptr++; ptr++;
                while(*ptr <= ' ') ptr++;
                
                Addfriend(&me,temp);
            }
            
            wclear(commandeline);
			wborder(commandeline, ' ', ' ', 0, 0, 0, 0, 0, 0);
			
			
        }
        else if(!strncmp(buff, "/delfriend", 10)) {
            wclear(commandeline);
            wborder(commandeline, ' ', ' ', 0, 0, 0, 0, 0, 0);
            char *ptr = strtok(buff, " ");
            char temp[ALIASLEN];
            ptr = strtok(0, " ");
            memset(temp, 0, sizeof(char) * ALIASLEN);
            if(ptr != NULL) {
                aliaslen =  strlen(ptr);
                if(aliaslen > ALIASLEN) ptr[ALIASLEN] = 0;
                strcpy(temp, ptr);
                while(*ptr) ptr++; ptr++;
                while(*ptr <= ' ') ptr++;
                
                Delfriend(&me,temp);
            }
            
            wclear(commandeline);
			wborder(commandeline, ' ', ' ', 0, 0, 0, 0, 0, 0);
			
			
        }
        else if(!strncmp(buff, "/send", 5)) {
			
			char *ptr = strtok(buff, " ");
            char temp[ALIASLEN];
            ptr = strtok(0, " ");
            memset(temp, 0, sizeof(char) * ALIASLEN);
            if(ptr != NULL) {
                aliaslen =  strlen(ptr);
                if(aliaslen > ALIASLEN) ptr[ALIASLEN] = 0;
                strcpy(temp, ptr);
                while(*ptr) ptr++; ptr++;
                while(*ptr <= ' ') ptr++;
                
                sendtoall(&me,temp);
            }
            
            wclear(commandeline);
			wborder(commandeline, 0, 0, 0, 0, 0, 0, 0, 0);
			mvwprintw(chatbox,LINES-commandelinehight-1,1,"[->broadcast] %s : %s\n",&me.alias,temp);
		
        }
        else if(!strncmp(buff, "/logout", 7)) {
            logout(&me);
        }
        else
        {
			wclear(commandeline);
			wborder(commandeline, ' ', ' ', 0, 0, 0, 0, 0, 0);
			wattron(chatbox, A_BOLD | COLOR_PAIR(1));
			mvwprintw(chatbox,LINES-commandelinehight-1,1,"Commande inconnue !\n");	
			wattroff(chatbox, A_BOLD | COLOR_PAIR(1));	
		}
    
		
		
		
		

			wrefresh(clients_conn);
			wrefresh(commandeline);
			wrefresh(chatbox);
	}

	close(s);
	endwin();

	return 0;
}


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
            printf("\t/launch pour lancer l'interface graphique.\n"); 
            printf("\t/exit pour quitter le programe.\n"); 
           
} 
