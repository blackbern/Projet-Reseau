all : token cltokudp cltoktcp

token : token_serveur.o tools.o
	gcc token_serveur.o tools.o -o token

token_serveur.o : token_serveur.c
	gcc token_serveur.c -c -Wall -Wextra

tools.o : tools.c
	gcc tools.c -c -Wall -Wextra


cltokudp : clientudp.o
	gcc clientudp.o -o cltokudp

clientudp.o : clientudp.c
	gcc clientudp.c -c -Wall -Wextra


cltoktcp : clienttcp.o
	gcc clienttcp.o -o cltoktcp

clienttcp.o : clienttcp.c
	gcc clienttcp.c -c -Wall -Wextra
