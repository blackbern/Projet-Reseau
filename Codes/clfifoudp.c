/**
 * client.c
 */

#include "tool_serv.h"

#define P 12332
#define LGMES 200

int main(int argc, char* argv[]) {

  if(argc != 3)
    {
      printf("Utilisation : hote port1 [port2] ... [portn]\n");
      return 0;
    }

  is_udp = 1;
  is_tcp = 0;

  // On d�finit les variables utilis�es dans le programme client.
  int s, sock;
  char env[BUFSIZ], rec[BUFSIZ], NomDistant[BUFSIZ];
  /*
  // On cr�e une variable de type sokaddr_in qui sera utilis�e pour la cr�ation de la socket cliente.
  struct sockaddr_in adresse;*/

  // Ce pointeur sur la structure hostent sera utilis� pour la recherche d'une adresse IP connaissant le nom du serveur (r�solution de noms)
  struct hostent *recup;
    
  struct sockaddr_in adresse;
  // On cr�e la socket cliente, de type AF_INET, qui utilisera TCP comme protocole de transport.
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("Erreur ouverture");
    return(-1);
  }
    
  // On demande a l'utilisateur de rentrer le nom de la machine serveur a laquelle le programme va se connecter.
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
    
  /*
    On demande � l'utilisateur d'�crire un message qui sera envoy� via le r�seau au serveur.
    On remarque que, contrairement au serveur qui lit en premier et �crit ensuite, le client commence par �crire le message
    que le serveur attend, et lit ensuite la r�ponse.
  */
  unsigned int len = sizeof(adresse);

  // Comme pour le serveur, on utilise write pour exp�dier le message sur le r�seau, avec message d'erreur si l'�criture se passe mal.
  if (sendto(sock, "Hello", 5, 0, (struct sockaddr *) &adresse, len) == -1) {
    perror("Erreur ecriture");
    return(-1);
  }

  len_err = len;
  addr_err = adresse;
  sock_err = sock;
  // Gestion d'une deconnexion
  signal(SIGINT, deconnexion);

  // On lit la r�ponse du serveur.
  printf("Attente de r�ponse...\n");
  s = recvfrom(sock, rec, 1024, 0, (struct sockaddr *) &adresse, &len);

  // Message d'erreur si la lecture se passe mal.
  if (s == -1) {
    perror("Erreur read");
    return(-1);
  }
  else
    {
      if(!strcmp(rec,"?"))
	{
	  printf("Que voulez-vous �crire ? (--quit pour se deconnecter)\n");
	  scanf(" %[^\n]",env);
	  if(!strcmp(env, "--quit"))
	    deconnexion();
	  if (sendto(sock, env, strlen(env), 0, (struct sockaddr *) &adresse, len) == -1)
	    {
	      perror("Erreur ecriture");
	      return(-1);
	    }
	  printf("Ecriture termin�e\n");
	}
      else if(!strcmp(rec,"-"))
	{
	  printf("Deconnect� par le serveur\n");
	  exit(EXIT_FAILURE);
	}
    }

  while(1)
    {
      memset(env, 0, sizeof(env));
      memset(rec, 0, sizeof(env));
      printf("Que voulez-vous �crire ? (--quit pour se deconnecter)\n");
      scanf(" %[^\n]",env);
      if(recvfrom(sock, rec, 1024, 0, (struct sockaddr *) &adresse, &len) < 0)
	{
	  perror("Erreur ecriture");
	  return(-1);
	}
      if(!strcmp(rec,"?"))
	{
	  if (sendto(sock, env, strlen(env), 0, (struct sockaddr *) &adresse, len) == -1)
	    {
	      perror("Erreur ecriture");
	      return(-1);
	    }
	  printf("Ecriture termin�e\n");
	}
      else if(!strcmp(rec,"-"))
	{
	  printf("Deconnect� par le serveur\n");
	  exit(EXIT_FAILURE);
	}
      if(!strcmp(env, "--quit"))
	break;
      printf("En attente...\n");
    }
  printf("Deconnect�");

  // On referme la socket cliente. Cette op�ration ferme la socket cliente pour le programme client, le serveur fait de m�me de son cot�,
  // en plus de refermer sa propre socket.
  close(sock);
    
  return 0;
}
