#ifndef SERVEUR
#define SERVEUR

#include <stdlib.h>// On importe des fichiers.h nécéssaires à l'application
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
#include <signal.h>

#define FIC "./fichier.txt"

int sock_err, len_err, is_tcp, is_udp;
struct sockaddr_in addr_err;

void deconnexion();

int creersockudp(u_short port);
int creersocktcp(u_short port);
int ecrire(char*);
int ecrire_ligne(char*);

#endif
