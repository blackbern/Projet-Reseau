/**
 * serveur.c
 */

// On importe des fichiers.h nécéssaires à l'application
#include <stdio.h>      // Fichier contenant les en-têtes des fonctions standard d'entrées/sorties 
#include <stdlib.h>     // Fichier contenant les en-têtes de fonctions standard telles que malloc()
#include <string.h>     // Fichier contenant les en-têtes de fonctions standard de gestion de chaînes de caractères 
#include <unistd.h>     // Fichier d'en-têtes de fonctions de la norme POSIX (dont gestion des fichiers : write(), close(), ...)
#include <sys/types.h>      // Fichier d'en-têtes contenant la définition de plusieurs types et de structures primitifs (système)
#include <sys/socket.h>     // Fichier d'en-têtes des fonctions de gestion de sockets
#include <netinet/in.h>     // Fichier contenant différentes macros et constantes facilitant l'utilisation du protocole IP
#include <netdb.h>      // Fichier d'en-têtes contenant la définition de fonctions et de structures permettant d'obtenir des informations sur le réseau (gethostbyname(), struct hostent, ...)
#include <memory.h>     // Contient l'inclusion de string.h (s'il n'est pas déjà inclus) et de features.h
#include <errno.h>      // Fichier d'en-têtes pour la gestion des erreurs (notamment perror()) 

#define P 12332

/*
  creersock

  Il s'agit de la fonction qui va permettre la création d'une socket.
  Elle est utilisée dans la fonction main().
  Elle prend en paramètre un entier court non signé, qui est le numéro de port,
  nécéssaire à l'opération bind().
  Cette fonction renvoie un numéro qui permet d'identifier la socket nouvellement créée
  (ou la valeur -1 si l'opération a échouée).
*/

int creersockudp( u_short port) {

  // On crée deux variables entières
  int sock, retour;

  // On crée une variable adresse selon la structure sockaddr_in (la structure est décrite dans sys/socket.h)
  struct sockaddr_in adresse;
  
  /*
    La ligne suivante décrit la création de la socket en tant que telle.
    La fonction socket prend 3 paramètres : 
    - la famille du socket : la plupart du temps, les développeurs utilisent AF_INET pour l'Internet (TCP/IP, adresses IP sur 4 octets)
    Il existe aussi la famille AF_UNIX, dans ce mode, on ne passe pas des numéros de port mais des noms de fichiers.
    - le protocole de niveau 4 (couche transport) utilisé : SOCK_STREAM pour TCP, SOCK_DGRAM pour UDP, ou enfin SOCK_RAW pour générer
    des trames directement gérées par les couches inférieures.
    - un numéro désignant le protocole qui fournit le service désiré. Dans le cas de socket TCP/IP, on place toujours ce paramètre a 0 si on utilise le protocole par défaut.
  */

  sock = socket(AF_INET,SOCK_DGRAM,0);

  // Si le code retourné n'est pas un identifiant valide (la création s'est mal passée), on affiche un message sur la sortie d'erreur, et on renvoie -1
  if (sock<0) {
    perror ("ERREUR OUVERTURE");
    return(-1);
  }
  
  // On complète les champs de la structure sockaddr_in : 
  // La famille du socket, AF_INET, comme cité précédement
  adresse.sin_family = AF_INET;

  /* Le port auquel va se lier la socket afin d'attendre les connexions clientes. La fonction htonl()  
     convertit  un  entier  long  "htons" signifie "host to network long" conversion depuis  l'ordre des bits de l'hôte vers celui du réseau.
  */
  adresse.sin_port = htons(port);

  /* Ce champ désigne l'adresse locale auquel un client pourra se connecter. Dans le cas d'une socket utilisée 
     par un serveur, ce champ est initialisé avec la valeur INADDR_ANY. La constante INADDR_ANY utilisée comme 
     adresse pour le serveur a pour valeur 0.0.0.0 ce qui correspond à une écoute sur toutes les interfaces locales disponibles.
    
  */
  adresse.sin_addr.s_addr=INADDR_ANY;
  
  /*
    bind est utilisé pour lier la socket : on va attacher la socket crée au début avec les informations rentrées dans
    la structure sockaddr_in (donc une adresse et un numéro de port).
    Ce bind affecte une identité à la socket, la socket représentée par le descripteur passé en premier argument est associée 
    à l'adresse passée en seconde position. Le dernier argument représente la longueur de l'adresse. 
    Ce qui a pour but de  rendre la socket accessible de l'extérieur (par getsockbyaddr)
  */
  retour = bind (sock,(struct sockaddr *)&adresse,sizeof(adresse));
  
  // En cas d'erreur lors du bind, on affiche un message d'erreur et on renvoie -1
  if (retour<0) {
    perror ("IMPOSSIBLE DE NOMMER LA SOCKET");
    return(-1);
  }

  // Au final, on renvoie sock, qui contient l'identifiant à la socket crée et attachée.
  return (sock);
}

int creersocktcp( u_short port) {

  // On crée deux variables entières
  int sock, retour;

  // On crée une variable adresse selon la structure sockaddr_in (la structure est décrite dans sys/socket.h)
  struct sockaddr_in adresse;
  
  /*
    La ligne suivante décrit la création de la socket en tant que telle.
    La fonction socket prend 3 paramètres : 
    - la famille du socket : la plupart du temps, les développeurs utilisent AF_INET pour l'Internet (TCP/IP, adresses IP sur 4 octets)
    Il existe aussi la famille AF_UNIX, dans ce mode, on ne passe pas des numéros de port mais des noms de fichiers.
    - le protocole de niveau 4 (couche transport) utilisé : SOCK_STREAM pour TCP, SOCK_DGRAM pour UDP, ou enfin SOCK_RAW pour générer
    des trames directement gérées par les couches inférieures.
    - un numéro désignant le protocole qui fournit le service désiré. Dans le cas de socket TCP/IP, on place toujours ce paramètre a 0 si on utilise le protocole par défaut.
  */


  sock = socket(AF_INET,SOCK_STREAM,0);

  // Si le code retourné n'est pas un identifiant valide (la création s'est mal passée), on affiche un message sur la sortie d'erreur, et on renvoie -1
  if (sock<0) {
    perror ("ERREUR OUVERTURE");
    return(-1);
  }
  
  // On complète les champs de la structure sockaddr_in : 
  // La famille du socket, AF_INET, comme cité précédement
  adresse.sin_family = AF_INET;

  /* Le port auquel va se lier la socket afin d'attendre les connexions clientes. La fonction htonl()  
     convertit  un  entier  long  "htons" signifie "host to network long" conversion depuis  l'ordre des bits de l'hôte vers celui du réseau.
  */
  adresse.sin_port = htons(port);

  /* Ce champ désigne l'adresse locale auquel un client pourra se connecter. Dans le cas d'une socket utilisée 
     par un serveur, ce champ est initialisé avec la valeur INADDR_ANY. La constante INADDR_ANY utilisée comme 
     adresse pour le serveur a pour valeur 0.0.0.0 ce qui correspond à une écoute sur toutes les interfaces locales disponibles.
    
  */
  adresse.sin_addr.s_addr=INADDR_ANY;
  
  /*
    bind est utilisé pour lier la socket : on va attacher la socket crée au début avec les informations rentrées dans
    la structure sockaddr_in (donc une adresse et un numéro de port).
    Ce bind affecte une identité à la socket, la socket représentée par le descripteur passé en premier argument est associée 
    à l'adresse passée en seconde position. Le dernier argument représente la longueur de l'adresse. 
    Ce qui a pour but de  rendre la socket accessible de l'extérieur (par getsockbyaddr)
  */
  retour = bind (sock,(struct sockaddr *)&adresse,sizeof(adresse));
  
  // En cas d'erreur lors du bind, on affiche un message d'erreur et on renvoie -1
  if (retour<0) {
    perror ("IMPOSSIBLE DE NOMMER LA SOCKET");
    return(-1);
  }

  // Au final, on renvoie sock, qui contient l'identifiant à la socket crée et attachée.
  return (sock);
}

int main (int argc, char* argv[]) {

  if(argc < 2)
    {
      printf("port(s) manquant(s).\n");
      return 1;
    }
  // On définit les variables nécéssaires
  //  int s, sock1, sock2, sock;
  int *socks, i, s, sock, msgsock, udp, tcp;
  fd_set fd_read, fd_write;
  unsigned int len;
  struct sockaddr addr;
  //  u_short port1, port2;
  char msg [BUFSIZ];
  
  // On crée les sockets
  socks = (int*)malloc(2*(argc-1)*sizeof(int));
  for(i = 0; i < argc-1; i++)
    {
      socks[i] = creersockudp(atoi(argv[i+1]));
      socks[i+argc] = creersocktcp(atoi(argv[i+1]));
      listen(socks[i+argc], 5);
    }

  /* La fonction accept permet d'accepter une connexion à notre socket par un client. On passe en paramètres la socket serveur d'écoute à demi définie.
     msgsock contiendra l'identifiant de la socket cliente. msgsock est la valeur de retour de la primitive accept. 
     C'est une socket d'échange de messages : elle est entièrement définie.
     On peut préciser aussi la structure et la taille de sockaddr associée 
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
	  // On lit le message envoyé par la socket de communication. 
	  //  msg contiendra la chaine de caractères envoyée par le réseau,
	  // s le code d'erreur de la fonction. -1 si pb et sinon c'est le nombre de caractères lus
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

		  // On demande à l'utilisateur de rentrer un message qui va être expédié sur le réseau
		  msg[0] = '\0';
		  scanf(" %[^\n]", msg);
    
		  // On va écrire sur la socket, en testant le code d'erreur de la fonction write.
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
  
	      // On lit le message envoyé par la socket de communication. 
	      //  msg contiendra la chaine de caractères envoyée par le réseau,
	      // s le code d'erreur de la fonction. -1 si pb et sinon c'est le nombre de caractères lus
	      s = read(msgsock, msg, 1024);
  

	      if (s == -1)
		perror("Problemes");
	      else {

		// Si le code d'erreur est bon, on affiche le message.
		msg[s] = 0;
		printf("Msg: %s\n", msg);
		printf("Recept reussie, emission msg: ");

		// On demande à l'utilisateur de rentrer un message qui va être expédié sur le réseau
		msg[0] = '\0';
		scanf(" %[^\n]", msg);
    
		// On va écrire sur la socket, en testant le code d'erreur de la fonction write.
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

  // On referme les sockets d'écoute.
  for(i = 0; i < argc-1; i++)
    {
      close(socks[i]);
      close(socks[i+argc]);
    }

  return 0;
}
