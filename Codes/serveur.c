/**
 * serveur.c
 */

// On importe des fichiers.h n�c�ssaires � l'application
#include <stdio.h>      // Fichier contenant les en-t�tes des fonctions standard d'entr�es/sorties 
#include <stdlib.h>     // Fichier contenant les en-t�tes de fonctions standard telles que malloc()
#include <string.h>     // Fichier contenant les en-t�tes de fonctions standard de gestion de cha�nes de caract�res 
#include <unistd.h>     // Fichier d'en-t�tes de fonctions de la norme POSIX (dont gestion des fichiers : write(), close(), ...)
#include <sys/types.h>      // Fichier d'en-t�tes contenant la d�finition de plusieurs types et de structures primitifs (syst�me)
#include <sys/socket.h>     // Fichier d'en-t�tes des fonctions de gestion de sockets
#include <netinet/in.h>     // Fichier contenant diff�rentes macros et constantes facilitant l'utilisation du protocole IP
#include <netdb.h>      // Fichier d'en-t�tes contenant la d�finition de fonctions et de structures permettant d'obtenir des informations sur le r�seau (gethostbyname(), struct hostent, ...)
#include <memory.h>     // Contient l'inclusion de string.h (s'il n'est pas d�j� inclus) et de features.h
#include <errno.h>      // Fichier d'en-t�tes pour la gestion des erreurs (notamment perror()) 

#define P 12332

/*
  creersock

  Il s'agit de la fonction qui va permettre la cr�ation d'une socket.
  Elle est utilis�e dans la fonction main().
  Elle prend en param�tre un entier court non sign�, qui est le num�ro de port,
  n�c�ssaire � l'op�ration bind().
  Cette fonction renvoie un num�ro qui permet d'identifier la socket nouvellement cr��e
  (ou la valeur -1 si l'op�ration a �chou�e).
*/

int creersockudp( u_short port) {

  // On cr�e deux variables enti�res
  int sock, retour;

  // On cr�e une variable adresse selon la structure sockaddr_in (la structure est d�crite dans sys/socket.h)
  struct sockaddr_in adresse;
  
  /*
    La ligne suivante d�crit la cr�ation de la socket en tant que telle.
    La fonction socket prend 3 param�tres : 
    - la famille du socket : la plupart du temps, les d�veloppeurs utilisent AF_INET pour l'Internet (TCP/IP, adresses IP sur 4 octets)
    Il existe aussi la famille AF_UNIX, dans ce mode, on ne passe pas des num�ros de port mais des noms de fichiers.
    - le protocole de niveau 4 (couche transport) utilis� : SOCK_STREAM pour TCP, SOCK_DGRAM pour UDP, ou enfin SOCK_RAW pour g�n�rer
    des trames directement g�r�es par les couches inf�rieures.
    - un num�ro d�signant le protocole qui fournit le service d�sir�. Dans le cas de socket TCP/IP, on place toujours ce param�tre a 0 si on utilise le protocole par d�faut.
  */

  sock = socket(AF_INET,SOCK_DGRAM,0);

  // Si le code retourn� n'est pas un identifiant valide (la cr�ation s'est mal pass�e), on affiche un message sur la sortie d'erreur, et on renvoie -1
  if (sock<0) {
    perror ("ERREUR OUVERTURE");
    return(-1);
  }
  
  // On compl�te les champs de la structure sockaddr_in : 
  // La famille du socket, AF_INET, comme cit� pr�c�dement
  adresse.sin_family = AF_INET;

  /* Le port auquel va se lier la socket afin d'attendre les connexions clientes. La fonction htonl()  
     convertit  un  entier  long  "htons" signifie "host to network long" conversion depuis  l'ordre des bits de l'h�te vers celui du r�seau.
  */
  adresse.sin_port = htons(port);

  /* Ce champ d�signe l'adresse locale auquel un client pourra se connecter. Dans le cas d'une socket utilis�e 
     par un serveur, ce champ est initialis� avec la valeur INADDR_ANY. La constante INADDR_ANY utilis�e comme 
     adresse pour le serveur a pour valeur 0.0.0.0 ce qui correspond � une �coute sur toutes les interfaces locales disponibles.
    
  */
  adresse.sin_addr.s_addr=INADDR_ANY;
  
  /*
    bind est utilis� pour lier la socket : on va attacher la socket cr�e au d�but avec les informations rentr�es dans
    la structure sockaddr_in (donc une adresse et un num�ro de port).
    Ce bind affecte une identit� � la socket, la socket repr�sent�e par le descripteur pass� en premier argument est associ�e 
    � l'adresse pass�e en seconde position. Le dernier argument repr�sente la longueur de l'adresse. 
    Ce qui a pour but de  rendre la socket accessible de l'ext�rieur (par getsockbyaddr)
  */
  retour = bind (sock,(struct sockaddr *)&adresse,sizeof(adresse));
  
  // En cas d'erreur lors du bind, on affiche un message d'erreur et on renvoie -1
  if (retour<0) {
    perror ("IMPOSSIBLE DE NOMMER LA SOCKET");
    return(-1);
  }

  // Au final, on renvoie sock, qui contient l'identifiant � la socket cr�e et attach�e.
  return (sock);
}

int creersocktcp( u_short port) {

  // On cr�e deux variables enti�res
  int sock, retour;

  // On cr�e une variable adresse selon la structure sockaddr_in (la structure est d�crite dans sys/socket.h)
  struct sockaddr_in adresse;
  
  /*
    La ligne suivante d�crit la cr�ation de la socket en tant que telle.
    La fonction socket prend 3 param�tres : 
    - la famille du socket : la plupart du temps, les d�veloppeurs utilisent AF_INET pour l'Internet (TCP/IP, adresses IP sur 4 octets)
    Il existe aussi la famille AF_UNIX, dans ce mode, on ne passe pas des num�ros de port mais des noms de fichiers.
    - le protocole de niveau 4 (couche transport) utilis� : SOCK_STREAM pour TCP, SOCK_DGRAM pour UDP, ou enfin SOCK_RAW pour g�n�rer
    des trames directement g�r�es par les couches inf�rieures.
    - un num�ro d�signant le protocole qui fournit le service d�sir�. Dans le cas de socket TCP/IP, on place toujours ce param�tre a 0 si on utilise le protocole par d�faut.
  */


  sock = socket(AF_INET,SOCK_STREAM,0);

  // Si le code retourn� n'est pas un identifiant valide (la cr�ation s'est mal pass�e), on affiche un message sur la sortie d'erreur, et on renvoie -1
  if (sock<0) {
    perror ("ERREUR OUVERTURE");
    return(-1);
  }
  
  // On compl�te les champs de la structure sockaddr_in : 
  // La famille du socket, AF_INET, comme cit� pr�c�dement
  adresse.sin_family = AF_INET;

  /* Le port auquel va se lier la socket afin d'attendre les connexions clientes. La fonction htonl()  
     convertit  un  entier  long  "htons" signifie "host to network long" conversion depuis  l'ordre des bits de l'h�te vers celui du r�seau.
  */
  adresse.sin_port = htons(port);

  /* Ce champ d�signe l'adresse locale auquel un client pourra se connecter. Dans le cas d'une socket utilis�e 
     par un serveur, ce champ est initialis� avec la valeur INADDR_ANY. La constante INADDR_ANY utilis�e comme 
     adresse pour le serveur a pour valeur 0.0.0.0 ce qui correspond � une �coute sur toutes les interfaces locales disponibles.
    
  */
  adresse.sin_addr.s_addr=INADDR_ANY;
  
  /*
    bind est utilis� pour lier la socket : on va attacher la socket cr�e au d�but avec les informations rentr�es dans
    la structure sockaddr_in (donc une adresse et un num�ro de port).
    Ce bind affecte une identit� � la socket, la socket repr�sent�e par le descripteur pass� en premier argument est associ�e 
    � l'adresse pass�e en seconde position. Le dernier argument repr�sente la longueur de l'adresse. 
    Ce qui a pour but de  rendre la socket accessible de l'ext�rieur (par getsockbyaddr)
  */
  retour = bind (sock,(struct sockaddr *)&adresse,sizeof(adresse));
  
  // En cas d'erreur lors du bind, on affiche un message d'erreur et on renvoie -1
  if (retour<0) {
    perror ("IMPOSSIBLE DE NOMMER LA SOCKET");
    return(-1);
  }

  // Au final, on renvoie sock, qui contient l'identifiant � la socket cr�e et attach�e.
  return (sock);
}

int main (int argc, char* argv[]) {

  if(argc < 2)
    {
      printf("port(s) manquant(s).\n");
      return 1;
    }
  // On d�finit les variables n�c�ssaires
  //  int s, sock1, sock2, sock;
  int *socks, i, s, sock, msgsock, udp, tcp;
  fd_set fd_read, fd_write;
  unsigned int len;
  struct sockaddr addr;
  //  u_short port1, port2;
  char msg [BUFSIZ];
  
  // On cr�e les sockets
  socks = (int*)malloc(2*(argc-1)*sizeof(int));
  for(i = 0; i < argc-1; i++)
    {
      socks[i] = creersockudp(atoi(argv[i+1]));
      socks[i+argc] = creersocktcp(atoi(argv[i+1]));
      listen(socks[i+argc], 5);
    }

  /* La fonction accept permet d'accepter une connexion � notre socket par un client. On passe en param�tres la socket serveur d'�coute � demi d�finie.
     msgsock contiendra l'identifiant de la socket cliente. msgsock est la valeur de retour de la primitive accept. 
     C'est une socket d'�change de messages : elle est enti�rement d�finie.
     On peut pr�ciser aussi la structure et la taille de sockaddr associ�e 
     mais ce n'est pas obligatoire et ici on a mis le pointeur NULL
  */
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
      if(fork() == 0)
	{
	  for(i = 0; i < argc-1; i++)
	    {
	      if(FD_ISSET(socks[i], &fd_read))
		{
		  sock = socks[i];
		  udp = 1;
		}
	      else if(FD_ISSET(socks[i+argc], &fd_read))
		{
		  sock = socks[i+argc];
		  tcp = 1;
		}
	    }
	  // On lit le message envoy� par la socket de communication. 
	  //  msg contiendra la chaine de caract�res envoy�e par le r�seau,
	  // s le code d'erreur de la fonction. -1 si pb et sinon c'est le nombre de caract�res lus
	  if(udp)
	    {
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
	    }
	  else if(tcp)
	    {
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
	      s = read(msgsock, msg, 1024);
  

	      if (s == -1)
		perror("Problemes");
	      else {

		// Si le code d'erreur est bon, on affiche le message.
		msg[s] = 0;
		printf("Msg: %s\n", msg);
		printf("Recept reussie, emission msg: ");

		// On demande � l'utilisateur de rentrer un message qui va �tre exp�di� sur le r�seau
		msg[0] = '\0';
		scanf(" %[^\n]", msg);
    
		// On va �crire sur la socket, en testant le code d'erreur de la fonction write.
		s = write(msgsock, msg, strlen(msg));
		if (s == -1) {
		  perror("Erreur write");
		  return(-1);
		}
		else
		  printf("Ecriture reussie, msg: %s\n", msg);
		// On referme la socket de communication
		close(msgsock);
	      }
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

  return 0;
}
