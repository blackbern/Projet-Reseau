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

  is_tcp = 1;
  is_udp = 0;

    // On d�finit les variables utilis�es dans le programme client.
    int sock;
    char env[BUFSIZ], rec[BUFSIZ], NomDistant[BUFSIZ];

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
    // adresse.sin_port = htons((u_short) P);
    adresse.sin_port = htons((u_short) atoi(argv[2]));

    // On tente de se connecter au serveur : cette op�ration prend la place du accept que l'on a effectu� dans le programme serveur.
    if (connect(sock, (struct sockaddr *)&adresse, sizeof(adresse)) == -1) {
        perror("Erreur connexion");
        return -1;
    }

    addr_err = adresse;
    sock_err = sock;
    // Gestion d'une deconnexion
    signal(SIGINT, deconnexion);

    printf("En attente...\n");
    if(read(sock, rec, 1024) < 0)
      {
	perror("Erreur read ");
	return -1;
      }
    if(!strcmp(rec, "?"))
      {
	printf("Que voulez-vous �crire ? (--quit pour se deconnecter)\n");
	scanf(" %[^\n]",env);
	if(!strcmp(env, "--quit"))
	  deconnexion();
	else
	  {
	    if(write(sock, env, strlen(env)) < 0)
	      {
		perror("Erreur ecriture");
		return(-1);
	      }
	  }
      }
    else if(!strcmp(rec,""))
      {
	printf("Deconnect� par le serveur\n");
	exit(EXIT_FAILURE);
      }
    printf("Message envoy�\nEn attente...\n");

    while(1)
      {
	memset(env, 0, sizeof(env));
	memset(rec, 0, sizeof(rec));
	if(read(sock, rec, 1024) < 0)
	  {
	    perror("Erreur lecture");
	    return(-1);
	  }
	if(!strcmp(rec,"?"))
	  {
	    printf("Ecriture r�ussie\n");
	    printf("Que voulez-vous �crire ? (--quit pour se deconnecter)\n");
	    scanf(" %[^\n]",env);
	    if (write(sock, env, strlen(env)) == -1)
	      {
		perror("Erreur ecriture");
		return(-1);
	      }
	  }
	else if(!strcmp(rec,""))
	  {
	    printf("Deconnect� par le serveur : �criture �chou�e\n");
	    exit(EXIT_FAILURE);
	  }
	if(!strcmp(env, "--quit"))
	  break;
	printf("En attente...\n");
      }
    printf("Deconnect�\n");
    // On referme la socket cliente. Cette op�ration ferme la socket cliente pour le programme client, le serveur fait de m�me de son cot�,
    // en plus de refermer sa propre socket.
    close(sock);
    
    return 0;
}
