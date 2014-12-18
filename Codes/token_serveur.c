/**
 * serveur.c
 */

#include "tool_serv.h"

int socks[BUFSIZ];

/**
 * structure décrivant la liste des clients ainsi que les informations qui leur sont associées.
 */
typedef struct client {
  int socket; // socket associée
  int type; // -1 pour non initialisé / 0 pour udp / 1 pour tcp
  int sockbla; // socket utilisée après un accept en tcp (identique à socket en udp)
  struct sockaddr_in addr; // adresse utilisée pour la communication en udp (0 en tcp)
  struct client *suivant; // client suivant dans l'anneau
} Client;

/**
 * fonction permettant de supprimer le client courant de la liste
 */
Client* detruire_client(Client *c)
{
  Client *p = c;

  if(p->suivant == c)
    return NULL;
  while(p->suivant != c)
    p = p->suivant;
  free(p->suivant);
  p->suivant = c->suivant;
  return p->suivant;
}

/**
 * fonction de création d'un client dans la liste, à la fin de la liste
 */
Client* creer_client(Client *c, int sock, int t)
{
  Client *new, *p = c;

  if(c == NULL)
    {
      c = (Client*)malloc(sizeof(Client));
      c->socket = sock;
      c->type = t;
      c->sockbla = -1;
      c->suivant = c;
    }
  else
    {
      new = (Client*)malloc(sizeof(Client));
      new->socket = sock;
      new->type = t;
      new->sockbla = -1;
      new->suivant = c;
      while(p->suivant != c)
	p = p->suivant;
      p->suivant = new;
    }
  return c;
}

/**
 * fonction de déroute d'interruption
 */
void deroute()
{
  int i;
  for(i = 0; socks[i] != '\0'; i++)
    {
      close(socks[i]);
    }
  exit(EXIT_SUCCESS);
}

/**
 * fonction de traitement d'un client udp
 */
Client* fils_udp(Client *client)
{
  socklen_t len;
  struct sockaddr addr;
  int s/*, jeton = 1, fin = 0*/;
  char mrec[BUFSIZ], *menv;

  len = sizeof(addr);
  //initialisation de la connexion
  s = recvfrom(client->socket, mrec, 1024, 0, &addr, &len);

  if(s == -1)
    perror("Problemes");
  else
    {
      // Si le code d'erreur est bon, on affiche le message.
      mrec[s] = 0;
      menv = strdup("?");

      s = sendto(client->socket, menv, strlen(menv), 0,  &addr, len);
      if (s == -1)
	{
	  perror("Erreur sendto");
	}

      memset(mrec, 0, sizeof(mrec));
      s = recvfrom(client->socket, mrec, 1024, 0, &addr, &len);

      if(s == -1)
	perror("Problemes");
      else
	{
	  if(strcmp(mrec, "--quit"))
	    ecrire_ligne(mrec);
	}
    }
  return client;
}

/**
 * fonction de traitement d'un client tcp
 */
Client* fils_tcp(Client *client)
{
  socklen_t len;
  int optval = 1;
  char mrec[BUFSIZ], *menv;

  printf("tcp : %d\n", client->socket);
  len = sizeof(client->addr);
  //initialisation de la connexion
  if(client->sockbla == -1)
    {
      client->sockbla = accept(client->socket, (struct sockaddr *)&(client->addr), &len);
      if(client->sockbla < 1)
	perror("accept erreur");
      setsockopt(client->sockbla, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    }

  // Si le code d'erreur est bon, on affiche le message.
  menv = strdup("?");
  if(write(client->sockbla, menv, strlen(menv)) < 1)
    {
      perror("Erreur write ");
    }

  memset(mrec, 0, sizeof(mrec));
  if(read(client->sockbla, mrec, 1024) < 0)
    perror("Problemes");
  else
    {
      if(strcmp(mrec, "--quit") && strcmp(mrec, ""))
	ecrire_ligne(mrec);
      else
	client = detruire_client(client);
    }
  return client;
}

int main (int argc, char* argv[]) {

  if(argc < 2)
    {
      printf("port(s) manquant(s)\n");
      return 1;
    }

  // On définit les variables nécéssaires
  int i;
  Client *client, *p;
  fd_set fd_read;
  struct timeval timespan;
  
  // On crée les sockets
  int optval = 1;
  client = NULL;

  for(i = 0; i < argc-1; i++)
    {
      socks[i] = creersockudp(atoi(argv[i+1]));
      socks[i+argc] = creersocktcp(atoi(argv[i+1]));
      setsockopt(socks[i+argc], SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
      listen(socks[i+argc], 5);
    }
  signal(SIGINT, deroute);

  while(1)
    {
      // réinitialisations
      timespan.tv_sec = 1;
      FD_ZERO(&fd_read);
      for(i = 0; i < argc-1; i++)
	{
	  FD_SET(socks[i], &fd_read);
	  FD_SET(socks[i+argc], &fd_read);
	}

      // ne pas relire les sockets en cours d'utilisation
      if(client != NULL)
	{
	  p = client;
	  while(p->suivant != client)
	    {
	      FD_CLR(p->socket, &fd_read);
	      p = p->suivant;
	    }
	}

      select(socks[2*argc-2]+1, &fd_read, 0, 0, &timespan);
      for(i = 0; i < argc-1; i++)
	{
	  if(FD_ISSET(socks[i], &fd_read))
	    {
	      client = creer_client(client, socks[i], 0);
	    }
	  else if(FD_ISSET(socks[i+argc], &fd_read))
	    {
	      client = creer_client(client, socks[i+argc], 1);
	    }
	}
      if(client != NULL)
	{
	  client = client->suivant;
	  printf("socks : %d - %d - %d - %d\n", client->socket, client->suivant->socket, client->suivant->suivant->socket, client->suivant->suivant->suivant->socket);
	  if(client->type == 0)
	    {
	      client = fils_udp(client);
	    }
	  else if(client->type == 1)
	    {
	      client = fils_tcp(client);
	    }
	}
    }

  // On referme les sockets d'écoute.
  for(i = 0; i < argc-1; i++)
    {
      close(socks[i]);
      close(socks[i+argc-1]);
    }

  exit(EXIT_SUCCESS);
}
