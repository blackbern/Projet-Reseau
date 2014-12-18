/** 
 * serveur_csma.c 
 */ 

#include "tool_serv.h"
 
typedef struct client {
  int socket; // socket associée
  int type; // -1 pour non initialisé / 0 pour udp / 1 pour tcp
  struct sockaddr_in addr; // adresse utilisée pour la communication en udp (0 en tcp)
} Client;

 
int main (int argc, char* argv[]) { 

  // On définit les variables nécéssaires 
  int *socks, i, s, sock = 0, udp, tcp, f, existefils;
  fd_set fd_read, fd_write; 

  int sockacc[argc];

  unsigned int len , l ;
  struct sockaddr_in addr, adresse; 

  char * msg;
  msg = (char *) malloc(sizeof(char) * BUFSIZ); 
  
  int temps; //variable correpondant au temps que le client doit attendre 
  
  Client c[BUFSIZ]; // tableau de type client

  srand((unsigned int) time(NULL));

  if(argc < 2){ 
    printf("port(s) manquant(s).\n");
    return 1; 
  }

  // On crée les sockets 
  socks = (int*)malloc(2*(argc-1)*sizeof(int)); 
  for(i = 0; i < argc-1; i++){ 	
    socks[i] = creersockudp(atoi(argv[i+1])); 
    socks[i+argc] = creersocktcp(atoi(argv[i+1])); 
    listen(socks[i+argc], 5); 
  }  

  len = sizeof(addr);
  l = sizeof(adresse);

  while(1) 
    { 
      udp = 0; 
      tcp = 0; 

      FD_ZERO(&fd_read); 
      FD_ZERO(&fd_write); 	
      
      for(i = 0; i < argc-1; i++){ 
	FD_SET(socks[i], &fd_read); 
	FD_SET(socks[i+argc], &fd_read); 
      }        
 
      /*on test si le fils s'est terminé, 
	et on remet l'entier sock à zéro qui correpond que aucune socket n'est traité actuellement
	et on remet l'entier existefils à zéro car le fils s'est terminé 
      */
      if((waitpid(f, 0, WNOHANG)) == -1){
	sock = 0; 
	existefils = 0;
      }
      
      //utilisation de la primitive select
      select(socks[2*argc-2]+1, &fd_read, 0, 0, 0); 

      //on regarde sur quelles sockets il y a quelque chose à lire 
      for(i = 0; i < argc-1; i++){
        //cas des socket udp  
	if(FD_ISSET(socks[i], &fd_read)) {

	  //on récupère l'adresse du client
	  if(c[i].socket == 0){
	    c[i].socket = socks[i];
	    memset(msg, 0, strlen(msg));
	    if((recvfrom(c[i].socket, msg, 1024, 0, (struct sockaddr *) &c[i].addr, &len)) == -1){
	      perror("Erreur recvfrom");
	      exit(EXIT_FAILURE);
	    }
	  }

	  //on récupère le numéro de socket que l'on va traiter
	  if(sock == 0) {	
	    sock = socks[i];
	    addr = c[i].addr;
	  } 
	    
	  /* si la socket socks[i] n'est pas la socket que l'on ait en train de traiter alors 
	     on indique au client que la ressource n'est pas libre 
	     qu'il doit attendre un certain temps
	  */
	  if(socks[i] != sock){
	    printf("attendre\n");
	    if(sendto(socks[i], "attendre", 8, 0,  (struct sockaddr *) &c[i].addr, l) == -1){
	      perror("Erreur sendto");
	      exit(EXIT_FAILURE);
      	    }
	    memset(msg, 0, strlen(msg));
	    //on calcul un temps aléatoire entre 0 et 30 secondes
	    temps = rand() % 30;
	    sprintf(msg, "%d", temps);
	    if(sendto(socks[i], msg, strlen(msg), 0,  (struct sockaddr *) &c[i].addr, l) == -1){
	      perror("Erreur sendto");
	      exit(EXIT_FAILURE);
      	    }
      	    printf("temps : %d\n", temps);
	  }
	  udp = 1;
	} 
	
	//cas des socket tcp 
	else if(FD_ISSET(socks[i+argc], &fd_read)) { 
	  if(sockacc[i] == 0)
	    sockacc[i] = accept(socks[i+argc], (struct sockaddr *) 0, (unsigned int*) 0); 
	  
	  //on récupère le numéro de socket que l'on va traiter
	  if(sock == 0) 		
	    sock = sockacc[i];
	    	  
	  /* si la socket socks[i] n'est pas la socket que l'on ait en train de traiter alors 
	     on indique au client que la ressource n'est pas libre 
	     qu'il doit attendre un certain temps
	  */ 
	  if(sockacc[i] != sock){
	    printf("attendre\n");
	    if(write(sockacc[i], "attendre", 8) == -1){
	      perror("Erreur write");
	      exit(EXIT_FAILURE);
	    }
	    memset(msg, 0, strlen(msg));
	    //on calcul un temps aléatoire entre 0 et 30 secondes
	    temps = rand() % 30;
	    sprintf(msg, "%d", temps);
	    if(write(sockacc[i], msg, strlen(msg)) == -1){
	      perror("Erreur write");
	      exit(EXIT_FAILURE);
	    }
	    printf("temps : %d \n", temps);
	  }
	  tcp = 1;
	} 
      }

      if(existefils == 0){
	f=fork();
	if(f==-1)
	  perror("fils");
	else
	  existefils = 1;
	if(f==0){
	
	  //le client qui s'est connecté est de type : udp 
	  if(udp){ 

	    //on demande au client ce qu'il veut écrire dans le fichier
	    memset(msg, 0, strlen(msg));
	    msg= strdup("La ressource est libre, que voulez vous écrire dans le fichier :");
	    if((sendto(sock, msg, strlen(msg), 0,  (struct sockaddr *) &addr, len)) == -1){
	      perror("Erreur sendto"); 
	      exit(EXIT_FAILURE);
	    }
	  
	    //on récupère le message qu'il souhaite écrire
	    memset(msg, 0, strlen(msg));
	    if((recvfrom(sock, msg, 1024, 0, (struct sockaddr *) &addr, &len)) == -1){
	      perror("Erreur recvfrom");
	      exit(EXIT_FAILURE);
	    }
	 
	    //écriture dans le fichier
	    printf("ecrire : %s \n", msg);
	    ecrire_ligne(msg);	

	    //on indique au client que l'écriture dans le fichier s'est bien passé	
	    memset(msg, 0, strlen(msg));
	    msg = strdup("Ecriture réussie\n");
	    if((sendto(sock, msg, strlen(msg), 0,  (struct sockaddr *) &addr, len)) == 0){
	      perror("Erreur sendto"); 
	      exit(EXIT_FAILURE);
	    }
	  } 

	  //le client qui s'est connecté est de type : tcp 
	  else if(tcp){ 
	    //on demande au client ce qu'il veut écrire dans le fichier
	    memset(msg, 0, strlen(msg));
	    msg= strdup("La ressource est libre, que voulez vous écrire dans le fichier :\n");
	    if(write(sock, msg, strlen(msg)) == -1){
	      perror("Erreur write");
	      exit(EXIT_FAILURE);
	    }

	    //on récupère le message qu'il souhaite écrire	
	    memset(msg, 0, strlen(msg));
	    if(read(sock, msg, 1024) == -1){
	      perror("Erreur write");
	      exit(EXIT_FAILURE);
	    }	
	
	    //écriture dans le fichier
	    ecrire_ligne(msg);

	    //on indique au client que l'écriture dans le fichier s'est bien passé	
	    memset(msg, 0, strlen(msg));
	    msg = strdup("Ecriture réussie");
	    if(write(sock, msg, strlen(msg)) == -1){
	      perror("Erreur write");
	      exit(EXIT_FAILURE);
	    }
	  }
	}
      }
    } 
 
  // On referme les sockets d'écoute. 
  for(i = 0; i < argc-1; i++) 
    { 
      close(socks[i]); 
      close(socks[i+argc]); 
      close(sockacc[i]);
    } 
 
  return 0; 
} 
	




