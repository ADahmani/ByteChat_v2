server : server.c fonction_client.c fonction_server.c fonction_ncurses.c
	gcc server.c fonction_client.c fonction_server.c fonction_ncurses.c -lncurses -lpthread -o server.run

test : test_serveur.c fonction_client.c
	gcc test_serveur.c fonction_client.c fonction_server.c -o test_serveur.run

rm : 
	rm *.run
