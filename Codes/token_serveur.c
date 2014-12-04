/**
 * serveur.c
 */

#include "tool_serv.h"

#define PJETON 9999

int fils_udp(int sockcom, int sockjeton)
{
  socklen_t len;
  struct sockaddr addr;
  int s, jeton = 1, fin = 0;
  char mrec[BUFSIZ], *menv;

  len = sizeof(addr);
  //initialisation de la connexion
  s = recvfrom(sockcom, mrec, 1024, 0, &addr, &len);

  while(!fin)
    {
      while(!jeton)
	{
	  recvfrom(sockjeton, mrec, 1024, 0, &addr, &len);
	  jeton = (!strcmp(mrec, "jeton"));
	}

      if(s == -1)
	perror("Problemes");
      else
	{
	  // Si le code d'erreur est bon, on affiche le message.
	  mrec[s] = 0;
	  menv = strdup("?");

	  s = sendto(sockcom, menv, strlen(menv), 0,  &addr, len);
	  if (s == -1) {
	    perror("Erreur sendto");
	    return(-1);
	  }

	  memset(mrec, 0, sizeof(mrec));
	  s = recvfrom(sockcom, mrec, 1024, 0, &addr, &len);

	  if(s == -1)
	    perror("Problemes");
	  else
	    {
	      menv = strdup("jeton");
	      /*	      if(sendto(sockjeton, menv, strlen(menv), 0,  &addr, len) < 0)
		perror("Erreur passage jeton ");
		jeton = 0;*/
	      if(strcmp(mrec, "--quit"))
		ecrire_ligne(mrec);
	      else
		fin = 1;
	    }
	}
    }
  return 0;
}

int fils_tcp(int sock, int sockjeton)
{
  socklen_t len;
  struct sockaddr addr;
  int jeton = 1, fin = 0, sockbla, optval = 1;
  char mrec[BUFSIZ], *menv;

  len = sizeof(addr);
  //initialisation de la connexion
  sockbla = accept(sock, &addr, &len);
  setsockopt(sockbla, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  while(!fin)
    {
      while(!jeton)
	{
	  recvfrom(sockjeton, mrec, 1024, 0, &addr, &len);
	  jeton = (!strcmp(mrec, "jeton"));
	}
 
      // Si le code d'erreur est bon, on affiche le message.
      menv = strdup("?");

      if(write(sockbla, menv, strlen(menv)) < 1)
	{
	  perror("Erreur write ");
	  return(-1);
	}

      memset(mrec, 0, sizeof(mrec));
      if(read(sockbla, mrec, 1024) < 0)
	perror("Problemes");
      else
	{
	  menv = strdup("jeton");
	  /*	      if(sendto(sockjeton, menv, strlen(menv), 0,  &addr, len) < 0)
		      perror("Erreur passage jeton ");
		      jeton = 0;*/
	  if(strcmp(mrec, "--quit"))
	    ecrire_ligne(mrec);
	  else
	    fin = 1;
	}
    }
  return 0;
}

int main (int argc, char* argv[]) {

  if(argc < 2)
    {
      printf("port(s) manquant(s)\n");
      return 1;
    }

  // On définit les variables nécéssaires
  int *socks, i, sock, udp, tcp;
  fd_set fd_read, fd_write;
  
  // On crée les sockets
  int optval = 1;
  socks = (int*)malloc(2*(argc-1)*sizeof(int));
  for(i = 0; i < argc-1; i++)
    {
      socks[i] = creersockudp(atoi(argv[i+1]));
      socks[i+argc] = creersocktcp(atoi(argv[i+1]));
      setsockopt(socks[i+argc], SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
      listen(socks[i+argc], 5);
    }

  while(1)
    {
      udp = 0;
      tcp = 0;

      FD_ZERO(&fd_read);
      FD_ZERO(&fd_write);
      for(i = 0; i < argc-1; i++)
	{
	  FD_SET(socks[i], &fd_read);
	  FD_SET(socks[i+argc], &fd_read);
	}

      select(socks[2*argc-2]+1, &fd_read, 0, 0, 0);
      for(i = 0; i < argc-1; i++)
	{
	  if(FD_ISSET(socks[i], &fd_read))
	    {
	      sock = socks[i];
	      udp = 1;
	      break;
	    }
	  else if(FD_ISSET(socks[i+argc], &fd_read))
	    {
	      sock = socks[i+argc];
	      tcp = 1;
	      break;
	    }
	}
      if(udp)
	{
	  fils_udp(sock, 0);
	}
      else if(tcp)
	{
	  fils_tcp(sock, 0);
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
