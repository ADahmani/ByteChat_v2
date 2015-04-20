/**
* \file test_serveur.c
* \brief Programme de tests.
* \date 15 janvier 2015
*
* Programme de test pour les fonctions de traitement en base de données.
*/
#include "client_row.h"
#include "fonction_client.h"

/**
 *\fn int main()
 *\brief Programme principal
 *Contient tous les tests 
 * 
 */
int main()
{
	int choix = -1;
	char nom_client[NomLength];
	char mdp_client[MdpLength];
	char nom_ami[NomLength];
	//char nom_ami[NomLength];
	int id_client;
	int retour_fonction;
	Client C;
	
	do
	{
		printf("\n\n\n");
		printf("\n\t\t\tMenu Principal");
		printf("\n\t\t--- Fonctions base de données ---");
		printf("\n\tAfficher tous les clients : 1");
		printf("\n\tChercher un client par son nom : 2");
		printf("\n\tChercher un client par son id : 3");
		printf("\n\tAjouter un client : 4");
		printf("\n\tSupprimer un client : 5");
		printf("\n\tAjouter un ami : 6");
		printf("\n\tSupprimer un ami : 7");
		printf("\n\tAfficher les amis d'un client : 8");
		
		printf("\n Votre choix : ");
		scanf("%d",&choix);
		printf("\n\n");
		
		switch(choix)
		{
			case 1 ://afficher tous les clients
				Afficher_All_Client();
				break;
			case 2 : //chercher un client par son nom
				printf("\nEntrez le nom du client : ");
				scanf("%s",nom_client);
				C = Search_Client_By_Name(nom_client);
				if(C.idClient == 0)
					printf("\nAucun Client de ce nom");
				else
					Afficher_Client(&C);
				break;
			case 3 : //chercher un client par son id
				printf("\nEntrez l'id du client à  rechercher: ");
				scanf("%d",&id_client);
				C = Search_Client_By_Id(id_client);
				if(C.idClient == 0)
					printf("\nAucun Client de ce nom");
				else
					Afficher_Client(&C);
				break;
			case 4 ://Ajouter un client
				printf("\nEntrez le nom du client à ajouter : ");
				scanf("%s",nom_client);
				printf("\nEntrez le mdp du client à ajouter : ");
				scanf("%s",mdp_client);
				retour_fonction = Ajouter_Client(nom_client,mdp_client);
				if(retour_fonction == 1)
					printf("\nLe client a bien été ajouté");
				else if(retour_fonction == 0)
					printf("\nLe client existe déjà ");
				else
					printf("\nUne erreur est survenue");
					
				break;
			case 5 ://supprimer un client
				printf("\nEntrez le nom du client à  supprimer : ");
				scanf("%s",nom_client);
				retour_fonction = Supprimer_Client(nom_client);
				if(retour_fonction == 1)
					printf("\nLe client a bien été supprimé");
				else if(retour_fonction == 0)
					printf("\nLe client n'existe pas");
				else
					printf("\nUne erreur est survenue");
				break;
			case 6 ://Ajouter un ami
				printf("\nEntrez le nom du client : ");
				scanf("%s",nom_client);
				printf("\nEntrez le nom de l'ami : ");
				scanf("%s",nom_ami);
				retour_fonction = Ajouter_Ami(nom_client,nom_ami);
				if(retour_fonction == 1)
					printf("\nL'ami a bien été ajouté");
				else if(retour_fonction == 0)
					printf("\nLe client ou l'ami n'existe pas");
				else if(retour_fonction == -2)
					printf("\nLa personne est déjà dans ses amis");
				else
					printf("\nUne erreur est survenue");
				break;
			case 7 ://Supprimer un ami
				printf("\nEntrez le nom du client : ");
				scanf("%s",nom_client);
				printf("\nEntrez le nom de l'ami : ");
				scanf("%s",nom_ami);
				retour_fonction = Supprimer_Ami(nom_client,nom_ami);
				if(retour_fonction == 1)
					printf("\nL'ami a bien été supprimé");
				else if(retour_fonction == 0)
					printf("\nLe client ou l'ami n'existe pas");
				else if(retour_fonction == -2)
					printf("\nLa personne n'est pas dans ses amis");
				else
					printf("\nUne erreur est survenue");
				break;	
			case 8 ://Afficher tous les amis
				printf("\nEntrez le nom du client : ");
				scanf("%s",nom_client);
				Afficher_All_Ami(nom_client);
				break;
				
			default:
				printf("\nAu revoir");
			
			
		}
		
		
	}
	while(choix > 0);
	
	
	
	return 0;
	
	
	
}
