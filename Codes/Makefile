all : token cltokudp cltoktcp

token : token_serveur.o tools.o
	gcc token_serveur.o tools.o -o token

token_serveur.o : token_serveur.c
	gcc token_serveur.c -c -Wall -Wextra

tools.o : tools.c
	gcc tools.c -c -Wall -Wextra

cltokudp : cltokudp.o tools.o
	gcc cltokudp.o tools.o -o cltokudp

cltokudp.o : cltokudp.c
	gcc cltokudp.c -c -Wall -Wextra

cltoktcp : cltoktcp.o tools.o
	gcc cltoktcp.o tools.o -o cltoktcp

cltoktcp.o : cltoktcp.c
	gcc cltoktcp.c -c -Wall -Wextra
