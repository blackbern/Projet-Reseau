all : token cltokudp cltoktcp fifo clfifotcp clfifoudp

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

fifo : fifo_serv.o tools.o
	gcc fifo_serv.o tools.o -o fifo

fifo_serv.o : fifo_serv.c
	gcc fifo_serv.c -c -Wall -Wextra

clfifoudp : clfifoudp.o tools.o
	gcc clfifoudp.o tools.o -o clfifoudp

clfifoudp.o : clfifoudp.c
	gcc clfifoudp.c -c -Wall -Wextra

clfifotcp : clfifotcp.o tools.o
	gcc clfifotcp.o tools.o -o clfifotcp

clfifotcp.o : clfifotcp.c
	gcc clfifotcp.c -c -Wall -Wextra
