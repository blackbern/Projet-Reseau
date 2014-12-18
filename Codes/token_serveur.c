/**
 * serveur.c
 */

#include "tool_serv.h"

int socks[BUFSIZ], nb_clients;

typedef struct client {
  int socket; // socket associée
  int type; // -1 pour non initialisé / 0 pour udp / 1 pour tcp
  int sockbla; // socket utilisée après un accept en tcp (identique à socket en udp)
  struct sockaddr_in addr; // adresse utilisée pour la communication en udp (0 en tcp)
  struct client *suivant; // client suivant dans l'anneau
} Client;

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

void deroute()
{
  int i;
  for(i = 0; socks[i] != '\0'; i++)
    {
      close(socks[i]);
    }
  exit(EXIT_SUCCESS);
}

Client* fils_udp(Client *client)
{
  socklen_t len;
  struct sockaddr addr;
  int s/*, jeton = 1, fin = 0*/;
  char mrec[BUFSIZ], *menv;

  len = sizeof(addr);
  //initialisation de la connexion
  s = recvfrom(client->socket, mrec, 1024, 0, &addr, &len);

  /*  while(!fin)
      {
      while(!jeton)
      {
      recvfrom(client.precedent, mrec, 1024, 0, &(client.addr_p), &len);
      jeton = (!strcmp(mrec, "jeton"));
      }*/

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
	{/*
	   menv = strdup("jeton");
	   if(sendto(client.suivant, menv, strlen(menv), 0,  &(client.addr_s), len) < 0)
	   perror("Erreur passage jeton ");
	   jeton = 0;*/
	  if(strcmp(mrec, "--quit"))
	    ecrire_ligne(mrec);
	  /*      else
		  fin = 1;*/
	}
    }
  //}
  return client;
}

Client* fils_tcp(Client *client)
{
  socklen_t len;
  int /*jeton = 1, fin = 0,*/ optval = 1;
  char mrec[BUFSIZ], *menv;

  len = sizeof(client->addr);
  //initialisation de la connexion
  if(client->sockbla == -1)
    {
      client->sockbla = accept(client->socket, (struct sockaddr *)&(client->addr), &len);
      if(client->sockbla < 1)
	perror("accept erreur");
      setsockopt(client->sockbla, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    }

  /*  while(!fin)
      {
      while(!jeton)
      {
      recvfrom(client.precedent, mrec, 1024, 0, &addr, &len);
      jeton = (!strcmp(mrec, "jeton"));
      }*/
 
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
    {/*
       menv = strdup("jeton");
       if(sendto(client.suivant, menv, strlen(menv), 0,  &addr, len) < 0)
       perror("Erreur passage jeton ");
       jeton = 0;*/
      if(strcmp(mrec, "--quit"))
	ecrire_ligne(mrec);
      else
	client = detruire_client(client);
      /*	  else
		  fin = 1;
		  }*/
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
  int i/*, f, sock*/;
  Client *client, *old;
  fd_set fd_read;
  struct timeval timespan;
  //  char jeton[BUFSIZ];
  
  // On crée les sockets
  int optval = 1;
  client = NULL;
  /*
  Client *c1, *c2, *c3, *c4, *c;

  c = creer_client(c, 1, 0);
  creer_client(c, 2, 0);
  creer_client(c, 3, 0);
  creer_client(c, 4, 0);

  printf("%d\n",c->socket);
  c = detruire_client(c);
  printf("%d -> %d\n", c->socket, c->suivant->socket);
  c = detruire_client(c);
  printf("%d -> %d\n", c->socket, c->suivant->socket);
  c = detruire_client(c);
  printf("%d -> %d\n", c->socket, c->suivant->socket);
  c = detruire_client(c);
  c = creer_client(c, 1, 0);
  if(c != NULL)
    printf("%d -> %d\n", c->socket, c->suivant->socket);
  else
    printf("NULL\n");
  */
  for(i = 0; i < argc-1; i++)
    {
      socks[i] = creersockudp(atoi(argv[i+1]));
      socks[i+argc] = creersocktcp(atoi(argv[i+1]));
      setsockopt(socks[i+argc], SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
      listen(socks[i+argc], 5);
    }
  signal(SIGINT, deroute);

  old = (Client*) malloc(sizeof(Client));
  nb_clients = 0;
  //  f = fork();

  while(1)
    {
      timespan.tv_sec = 1;
      FD_ZERO(&fd_read);
      for(i = 0; i < argc-1; i++)
	{
	  FD_SET(socks[i], &fd_read);
	  FD_SET(socks[i+argc], &fd_read);
	}

      printf("select\n");
      select(socks[2*argc-2]+1, &fd_read, 0, 0, &timespan);
      for(i = 0; i < argc-1; i++)
	{
	  if(FD_ISSET(socks[i], &fd_read))
	    {
	      printf("udp + %d\n", i);
	      if(client != NULL)
		*old = *client;
	      client = creer_client(client, socks[i], 0);
	      nb_clients++;
	      old->suivant = client;
	    }
	  else if(FD_ISSET(socks[i+argc], &fd_read))
	    {
	      printf("tcp + %d\n", i);
	      if(client != NULL)
		*old = *client;
	      client = creer_client(client, socks[i+argc], 1);
	      nb_clients++;
	      old->suivant = client;
	    }
	}
      if(client != NULL)
	{
	  printf("socks : %d - %d - %d\n", client->socket, client->suivant->socket, client->suivant->suivant->socket);
	  if(client->type == 0)
	    {
	      *old = *client;
	      client = fils_udp(client);
	      if(client->socket != old->socket)
		FD_SET(old->socket, &fd_read);
	    }
	  else if(client->type == 1)
	    {
	      *old = *client;
	      client = fils_tcp(client);
	      if(client == NULL || client->socket != old->socket)
		FD_SET(old->socket, &fd_read);
	    }
	  if(client != NULL)
	    client = client->suivant;
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
