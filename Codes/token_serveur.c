/**
 * serveur.c
 */

#include "tool_serv.h"

int fils_udp(int sock)
{
  socklen_t len;
  struct sockaddr addr;
  int s;
  char msg[BUFSIZ];

  len = sizeof(addr);
  s = recvfrom(sock, msg, 1024, 0, &addr, &len);

  if(s == -1)
    perror("Problemes");
  else
    {
      // Si le code d'erreur est bon, on affiche le message.
      msg[s] = 0;
      printf("Msg: %s\n", msg);
      printf("Recept reussie, emission msg: ");

      // On demande � l'utilisateur de rentrer un message qui va �tre exp�di� sur le r�seau
      msg[0] = '\0';
      scanf(" %[^\n]", msg);
    
      // On va �crire sur la socket, en testant le code d'erreur de la fonction write.
      s = sendto(sock, msg, strlen(msg), 0,  &addr, len);
      if (s == -1) {
	perror("Erreur sendto");
	return(-1);
      }
      else
	printf("Ecriture reussie, msg: %s\n", msg);
    }
  return 0;
}

int fils_tcp(int sock, int presock, int postsock)
{
  int msgsock, s;
  char *jeton = strdup("notej");
  char msg[BUFSIZ];

  msgsock = accept (sock, (struct sockaddr *) 0, (unsigned int*) 0);
  // Si l'accept se passe mal, on quitte le programme en affichant un message d'erreur.
  if (msgsock == -1) {
    perror("Erreur accept");
    return(-1);
  }
  else
    printf("Accept reussi");
  
  // On lit le message envoy� par la socket de communication. 
  //  msg contiendra la chaine de caract�res envoy�e par le r�seau,
  // s le code d'erreur de la fonction. -1 si pb et sinon c'est le nombre de caract�res lus

  while(1)
    {
      if(presock != 0)
	{
	  write(msgsock, "En attente du jeton...\n", 1024);
	  while(strcmp(jeton, "jeton") != 0)
	    read(presock, jeton, 1024);
	}
      if(write(msgsock, "Ressource disponible : veuillez saisir votre texte\n", 1024 < 0))
	perror("Erreur write ");

      if((s = read(msgsock, msg, 1024)) < 0)
	perror("Erreur read ");
      else {
	// Si le code d'erreur est bon, on affiche le message.
	msg[s] = 0;
	ecrire(msg);

	if(write(msgsock, "Votre message a bien �t� �crit, passage du jeton", 1024) < 0)
	  perror("Erreur write ");
    
	if(postsock != 0 && write(postsock, "jeton", 1024) < 0)
	  perror("write ");
      }
    }
  close(msgsock);
  close(presock);
  close(postsock);
  return 0;
}

int main (int argc, char* argv[]) {

  if(argc < 2)
    {
      printf("port(s) manquant(s).\n");
      return 1;
    }

  // On d�finit les variables n�c�ssaires
  int *socks, i, sock, udp, tcp;
  fd_set fd_read, fd_write;
  
  // On cr�e les sockets
  socks = (int*)malloc(2*(argc-1)*sizeof(int));
  for(i = 0; i < argc-1; i++)
    {
      socks[i] = creersockudp(atoi(argv[i+1]));
      socks[i+argc] = creersocktcp(atoi(argv[i+1]));
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
      if(fork() == 0)
	{
	  if(udp)
	    {
	      fils_udp(sock);
	    }
	  else if(tcp)
	    {
	      fils_tcp(sock, 0, 0);
	    }
	  exit(EXIT_SUCCESS);
	}
    }

  // On referme les sockets d'�coute.
  for(i = 0; i < argc-1; i++)
    {
      close(socks[i]);
      close(socks[i+argc]);
    }

  exit(EXIT_SUCCESS);
}
