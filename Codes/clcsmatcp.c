/**
 * clienttcp
 */

// On importe les fichiers.h nc�ssaires.
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
#include <time.h>


int main(int argc, char* argv[]) {

  if(argc != 3){
    printf("Utilisation : hote port1 [port2] ... [portn]\n");
    return 0;
  }

  // On d�finit les variables utilis�es dans le programme client.
  int s, sock, temps, fin =0;
  char msg[BUFSIZ], NomDistant[BUFSIZ];

  // On cr�e une variable de type sokaddr_in qui sera utilis�e pour la cr�ation de la socket cliente.
  struct sockaddr_in adresse;

  // Ce pointeur sur la structure hostent sera utilis� pour la recherche d'une adresse IP connaissant le nom du serveur (r�solution de noms)
  struct hostent *recup;
    
  // On cr�e la socket cliente, de type AF_INET, qui utilisera TCP comme protocole de transport.
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("Erreur ouverture");
    return(-1);
  }
    
  //on renseigne la machine distante
  sprintf(NomDistant, "%s", argv[1]);
  printf("NomDistant: %s\n", NomDistant);
    
  // On tente de r�soudre le nom donn� par l'utilisateur, afin de r�cup�rer l'adresse qui lui correspond
  // et remplir ainsi la structure sockaddr_in
  recup = gethostbyname(NomDistant);
  if (recup == NULL) {
    perror("Erreur obtention adresse");
    return(-1);
  }

  /*
    On copie la zone m�moire qui contient la valeur de reecup (l'adresse IP du serveur) directement dans la zone m�moire 
    qui doit contenir cette valeur dans la structure sockaddr_in. 
    En param�tres :  la zone m�moire destination, la zone m�moire source,
    la longueur des donn�es a copier.
  */
  memcpy((char *)&adresse.sin_addr, (char *)recup->h_addr, recup->h_length);
    
  // On utilise toujours une socket de type AF_INET
  adresse.sin_family = AF_INET;

  // On place le num�ro de port
  adresse.sin_port = htons((u_short) atoi(argv[2]));

  // On tente de se connecter au serveur : cette op�ration prend la place du accept que l'on a effectu� dans le programme serveur.
  if (connect(sock, (struct sockaddr *)&adresse, sizeof(adresse)) == -1) {
    perror("Erreur connexion");
    exit(EXIT_FAILURE);
  }

  while(!fin)
    {
      memset(msg, 0, strlen(msg));	
      if(read(sock, msg, 1024)== -1){
	perror("Erreur read");
	exit(EXIT_FAILURE);
      }


      /*si la ressource n'est pas libre 
	le client envoie le message "attendre" 
	puis le temps d'attente
      */
      if(strcmp(msg,"attendre") == 0){ 
	memset(msg, 0, strlen(msg));
	if(read(sock, msg, 1024) == -1){
	  perror("Erreur read");
	  exit(EXIT_FAILURE);
	}
  	
	temps = atoi(msg);
	printf("La ressource n'est pas libre, attendre : %d secondes\n", temps);
	sleep(temps);
	
	if (write(sock, "fini", 4) == -1) {
	  perror("Erreur write");
	  exit(EXIT_FAILURE);
	}	
      }
      else
	fin =1;
    }
  
  //on envoie au serveur ce que l'on souhaite �crire dans le fichier
  printf("%s\n", msg);
  memset(msg, 0, strlen(msg));
  scanf(" %[^\n]",msg);
  if (write(sock, msg, strlen(msg)) == -1){
    perror("Erreur write");
    exit(EXIT_FAILURE);
  }
    
  memset(msg, 0, strlen(msg));
  if(read(sock, msg, 1024) == -1){
    perror("Erreur read");
    exit(EXIT_FAILURE);
  }
  else
    printf("%s\n", msg);
    
  // On referme la socket cliente. Cette op�ration ferme la socket cliente pour le programme client, le serveur fait de m�me de son cot�,
  // en plus de refermer sa propre socket.
  close(sock);
    
  return 0;
}
