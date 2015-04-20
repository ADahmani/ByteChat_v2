#include "fonction_client.h"

//Fonction d'affichage
void Afficher_Client(Client *C)
{
	printf("\n-------------------------");
	printf("\nId du client : %d",C->idClient);
	printf("\nNom client : %s", C->nomClient);
	printf("\nMot de passe : %s", C->mdpClient);
	printf("\nListe d'amis du client : ");	
	Afficher_Ami(C->amis);
}

void Afficher_Ami(T_Amis *A)
{
	int i=0;
	if(A->nbAmis == -1)
		printf("\nAucun ami");
	else
	{
		for(i;i<=A->nbAmis;i++)
		{
			printf("\n id Ami nÂ° %d : %d",i+1,A->idAmis[i]);
		}
	}
	
}

void Afficher_All_Client()
{
	tabClient tab_C;
	int i=0;
	tab_C.nbC = -1;
	Select_All(&tab_C);
	if(tab_C.nbC == -1)
	{
		printf("\nIl n'y a pas de client\n");
	}
	else
	{
		printf("\nIl y a %d client(s)",tab_C.nbC+1);
		for(i;i<=tab_C.nbC;i++)
		{
			Afficher_Client(&(tab_C.tabC[i]));
		}
	}
}


void Select_All(tabClient *tab_C)
{
	char car;
	char *string = malloc(sizeof(char)*(3+NomLength+AmisLength+2)); 
	int ptr_string = 0;
	FILE *BDD_Client;
	BDD_Client = fopen("BaseClient","r");

	if(BDD_Client == NULL)
		printf("\nErreur Ouverture\n");
	else
	{			
		while((car = getc(BDD_Client)) != EOF)
		{
			if(car == ':')
			{
				if(strlen(string)>0)
				{
					string[ptr_string] = '\0';
					ptr_string = 0;
					Client Cl_aux = Str2Clt(string);
					tab_C->nbC++;
					tab_C->tabC[tab_C->nbC] = Cl_aux;
					
				}
					
			}
			else if(car == '\n')
			{
			}
			else
			{
				string[ptr_string] = car;
				ptr_string++;
			}
		}
		if(strlen(string)>0)
		{
			string[ptr_string] = '\0';
			ptr_string = 0;
			Client Cl_aux = Str2Clt(string);
			tab_C->nbC++;
			tab_C->tabC[tab_C->nbC] = Cl_aux;
			
		}
		fclose(BDD_Client);
	}
}


// fonction de conversion d'un string vers notre structure "Client" 
Client Str2Clt(char *m )
{
 	Client cl_temp;
	char *strIdAmis;

	char *index = strchr(m, ';'); // recupere l'index du premier caractÃ¨re de sÃ©paration ';'
	char *index_1 = strchr(index+1, ';');	
	char *index_2 = strchr(index_1+1,';');
	
	// allocation des memoires 
	cl_temp.nomClient = malloc(sizeof(char)*(strlen(index+1)-strlen(index_1+1)+1));
	cl_temp.mdpClient = malloc(sizeof(char)*(strlen(index_1+1)-strlen(index_2+1)+1));
	strIdAmis = malloc(sizeof(char)*(strlen(index_2+1)));
	cl_temp.amis = malloc(sizeof(T_Amis));
	
	//affectation
	sscanf(m,"%d;%[^;];%[^;];%s",&(cl_temp.idClient),cl_temp.nomClient,cl_temp.mdpClient,strIdAmis);
	
	cl_temp.amis->nbAmis = -1;
	//Transformation de la chaine strIdAmis en tableau d'int d'Id Amis
	if(strlen(strIdAmis)>0)
		strToTamis(cl_temp.amis,strIdAmis);



	return cl_temp;
}



void strToTamis(T_Amis *ta, char *strIdAmis)
{
	char *index;//recueille une sous chaine pour prendre au fur et Ã  mesure les amis
	char *index2 = strIdAmis;
	char *strId;
	while((index = strchr(index2,',')) != NULL)
	{
		ta->nbAmis++;
		strId = calloc(strlen(index2)-strlen(index),sizeof(char));
		strncpy(strId,index2,sizeof(char)*(strlen(index2)-strlen(index)));
		if(realloc(index2,(strlen(index)-1)*sizeof(char)) != NULL)
		{	
			
			strcpy(index2,index+1);
			//printf("\nid amis nÂ°%d : %s\n",nbAmis,strId);
			//idAmis[nbAmis-1] = 0;
			sscanf(strId,"%d",&(ta->idAmis[ta->nbAmis]));		
		}		
		
	}
	if(strlen(index2)>0)
	{
		ta->nbAmis++;
		sscanf(index2,"%d",&(ta->idAmis[ta->nbAmis]));	
	}
	
}


Client Search_Client_By_Name(char *str_search)
{	
	char car; 
	char *string = malloc(sizeof(char)*(3+NomLength+AmisLength+2)); 
	string[0] = '\0';
	int ptr_string = 0;
	Client Cl_aux;

	FILE *BDD_Client;
	BDD_Client = fopen("BaseClient","r");

	if(BDD_Client == NULL)
		printf("\nErreur Ouverture\n");
	else
	{		
		while((car = getc(BDD_Client)) != EOF)
		{
			if(car == ':')
			{
				if(strlen(string)>0)
				{
					string[ptr_string] = '\0';
					ptr_string = 0;
					Cl_aux = Str2Clt(string);
					if(strcmp(str_search,Cl_aux.nomClient)==0)
					{
						fclose(BDD_Client);
						return Cl_aux;
					}				
				}		
			}
			else if(car == '\n' || car == ' ')
			{
				//On ne fait rien
			}
			else
			{	
				string[ptr_string] = car;
				ptr_string++;
			}
		}

		if(string != NULL)
		{
			if(strlen(string)>0)
			{
				string[ptr_string] = '\0';
				Cl_aux = Str2Clt(string);
				if(strcmp(str_search,Cl_aux.nomClient)==0)
				{
					fclose(BDD_Client);
					return Cl_aux;
				}	
			}
		}
		fclose(BDD_Client);
	}
	
	Cl_aux.idClient = 0;
	return Cl_aux;
}


Client Search_Client_By_Id(int id_search)
{	
	char car;
	char *string = malloc(sizeof(char)*(3+NomLength+AmisLength+2)); 
	int ptr_string = 0;
	Client Cl_aux;

	FILE *BDD_Client;
	BDD_Client = fopen("BaseClient","r");
	if(BDD_Client == NULL)
		printf("\nErreur Ouverture\n");
	else
	{		
		while((car = getc(BDD_Client)) != EOF)
		{
			//caractÃ¨re de dÃ©but de Row	
			if(car == ':')
			{
				if(strlen(string)>0)
				{
					string[ptr_string] = '\0';
					ptr_string = 0;
					Cl_aux = Str2Clt(string);
					if(Cl_aux.idClient == id_search)
					{
						fclose(BDD_Client);
						return Cl_aux;
					}				
				}		
			}
			else if(car == '\n')
			{

			}
			else
			{
				string[ptr_string] = car;
				ptr_string++;

			}
		}
		if(strlen(string)>0)
		{
			string[ptr_string] = '\0';
			Cl_aux = Str2Clt(string);
			if(Cl_aux.idClient == id_search)
			{
				fclose(BDD_Client);
				return Cl_aux;
			}				
		}
		fclose(BDD_Client);
	}
	Cl_aux.idClient = 0;
	strcpy(Cl_aux.nomClient,"");
	strcpy(Cl_aux.mdpClient,"");
	Cl_aux.amis->nbAmis = -1;	
	
	return Cl_aux;
}

int Ajouter_Client(char *nom_client,char *mdp_client)
{
	tabClient tab_C;
	int i=0;
	tab_C.nbC = -1;
	Client leCl;
	leCl = Search_Client_By_Name(nom_client);
	FILE *f;
	char chaine[1024];
	chaine[0] = '\0';
	
	if(leCl.idClient>0)
		return 0;
	else
	{
		Select_All(&tab_C);
		tab_C.nbC++;
		leCl.idClient = tab_C.nbC + 1;
		leCl.nomClient = malloc(sizeof(char)*NomLength);
		leCl.mdpClient = malloc(sizeof(char)*MdpLength);
		leCl.amis = malloc(sizeof(T_Amis));
		strcpy(leCl.nomClient,nom_client);
		strcpy(leCl.mdpClient,mdp_client);
		leCl.amis->nbAmis = -1;
		tab_C.tabC[tab_C.nbC] = leCl;

		for(i;i<=tab_C.nbC;i++)
		{
			CltToStr(chaine,&(tab_C.tabC[i]));	
		} 
		
		f = fopen("BaseClient_aux","w+");
		if(f != NULL)
		{
			fprintf(f,"%s",chaine);
			fclose(f);
			remove("BaseClient");
			rename("BaseClient_aux","BaseClient");
		}
		else 
			return -1;		
		return 1;

	}	
	return -1;
}

int Supprimer_Client(char *nom_client)
{
	tabClient tab_C;
	int i=0;
	int j=0;
	tab_C.nbC = -1;
	Client leCl;
	leCl = Search_Client_By_Name(nom_client);
	FILE *f;
	char chaine[1024];
	chaine[0] = '\0';
	
	if(leCl.idClient==0)
		return 0;
	else
	{
		Select_All(&tab_C);
		
		for(i=0;i<=tab_C.nbC;i++)
		{	
			if(strcmp(tab_C.tabC[i].nomClient,nom_client) == 0)
			{
				tab_C.tabC[i] = tab_C.tabC[tab_C.nbC];
				tab_C.nbC--;
				tab_C.tabC[i].idClient = i + 1;	
			}
			for(j=0;j<=tab_C.tabC[i].amis->nbAmis;j++)
			{
					if(tab_C.tabC[i].amis->idAmis[j] == leCl.idClient)
					{
						tab_C.tabC[i].amis->idAmis[j] = tab_C.tabC[i].amis->idAmis[tab_C.tabC[i].amis->nbAmis];
						tab_C.tabC[i].amis->nbAmis--;
					}
			}
		}
		
		for(i=0;i<=tab_C.nbC;i++)
		{
			CltToStr(chaine,&(tab_C.tabC[i]));
		}
		

		f = fopen("BaseClient_aux","w+");
		if(f != NULL)
		{
			fprintf(f,"%s",chaine);
			fclose(f);
			remove("BaseClient");
			rename("BaseClient_aux","BaseClient");
		}
		else 
			return -1;		
		return 1;

	}	
	return -1;
}





void CltToStr(char *chaine,Client *C)
{
	char id[4];
	int i;
	
	sprintf(id,"%d",C->idClient);
	strcat(chaine,":");
	strcat(chaine,id);
	strcat(chaine,";");
	strcat(chaine,C->nomClient);
	strcat(chaine,";");
	strcat(chaine,C->mdpClient);
	strcat(chaine,";");
	
	for(i = 0;i<=C->amis->nbAmis;i++)
	{
		sprintf(id,"%d",C->amis->idAmis[i]);
		strcat(chaine,id);
		if(i!=C->amis->nbAmis)
			strcat(chaine,",");
	}
}

int Ajouter_Ami(char *nom_client,char *nom_ami)
{
	tabClient tab_C;
	int i=0;
	int j=0;
	tab_C.nbC = -1;
	Client leCl, lami;
	leCl = Search_Client_By_Name(nom_client);
	lami = Search_Client_By_Name(nom_ami);
	FILE *f;
	char chaine[1024];
	chaine[0] = '\0';
	
	if(leCl.idClient==0 || lami.idClient == 0)
		return 0;
	else
	{
		Select_All(&tab_C);		
		for(i=0;i<=tab_C.nbC;i++)
		{
			if(strcmp(tab_C.tabC[i].nomClient,nom_client) == 0)
			{
				for(j=0;j<=tab_C.tabC[i].amis->nbAmis;j++)
				{
					if(tab_C.tabC[i].amis->idAmis[j] == lami.idClient)
						return -2;
				}
				tab_C.tabC[i].amis->nbAmis++;
				tab_C.tabC[i].amis->idAmis[tab_C.tabC[i].amis->nbAmis] = lami.idClient;
			}
		}
		
		for(i=0;i<=tab_C.nbC;i++)
		{
			CltToStr(chaine,&(tab_C.tabC[i]));
		}
		

		f = fopen("BaseClient_aux","w+");
		if(f != NULL)
		{
			fprintf(f,"%s",chaine);
			fclose(f);
			remove("BaseClient");
			rename("BaseClient_aux","BaseClient");
		}
		else 
			return -1;		
		return 1;

	}	
	return -1;
}

int Supprimer_Ami(char *nom_client,char *nom_ami)
{
	tabClient tab_C;
	int i=0;
	int j=0;
	int flag=0;
	tab_C.nbC = -1;
	Client leCl, lami;
	leCl = Search_Client_By_Name(nom_client);
	lami = Search_Client_By_Name(nom_ami);
	FILE *f;
	char chaine[1024];
	chaine[0] = '\0';
	
	if(leCl.idClient==0 || lami.idClient == 0)
		return 0;
	else
	{
		Select_All(&tab_C);		
		for(i=0;i<=tab_C.nbC;i++)
		{
			if(strcmp(tab_C.tabC[i].nomClient,nom_client) == 0)
			{
				for(j=0;j<=tab_C.tabC[i].amis->nbAmis;j++)
				{
					if(tab_C.tabC[i].amis->idAmis[j] == lami.idClient)
					{
						tab_C.tabC[i].amis->idAmis[j] = tab_C.tabC[i].amis->idAmis[tab_C.tabC[i].amis->nbAmis];
						tab_C.tabC[i].amis->nbAmis--;
						flag =1;
					}
				}
				if(flag == 0)
					return -2;
			}
		}
		
		for(i=0;i<=tab_C.nbC;i++)
		{
			CltToStr(chaine,&(tab_C.tabC[i]));
		}
		

		f = fopen("BaseClient_aux","w+");
		if(f != NULL)
		{
			fprintf(f,"%s",chaine);
			fclose(f);
			remove("BaseClient");
			rename("BaseClient_aux","BaseClient");
		}
		else 
			return -1;		
		return 1;

	}	
	return -1;
}

void Afficher_All_Ami(char *nom_client){
	Client C = Search_Client_By_Name(nom_client);
	Client C_aux;
	int i=0;
	if(C.idClient > 0)
	{
		if(C.amis->nbAmis == -1)
			printf("\nCette personne n'a pas d'amis");
		else
		{
			for(i;i<=C.amis->nbAmis;i++)
			{
				C_aux = Search_Client_By_Id(C.amis->idAmis[i]);
				if(C.idClient > 0)
				{
					printf("\nAmi n°%d : %s",i+1, C_aux.nomClient);
				}
			}
		}
	}
	else
		printf("\nCe nom n'existe pas");
}