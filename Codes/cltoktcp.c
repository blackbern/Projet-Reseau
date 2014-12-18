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

    // On définit les variables utilisées dans le programme client.
    int sock;
    char env[BUFSIZ], rec[BUFSIZ], NomDistant[BUFSIZ];

    // On crée une variable de type sokaddr_in qui sera utilisée pour la création de la socket cliente.
    struct sockaddr_in adresse;

    // Ce pointeur sur la structure hostent sera utilisé pour la recherche d'une adresse IP connaissant le nom du serveur (résolution de noms)
    struct hostent *recup;
    
    // On crée la socket cliente, de type AF_INET, qui utilisera TCP comme protocole de transport.
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Erreur ouverture");
        return(-1);
    }
    
    // On demande a l'utilisateur de rentrer le nom de la machine serveur a laquelle le programme va se connecter.
    sprintf(NomDistant, "%s", argv[1]);
    printf("NomDistant: %s\n", NomDistant);
    
    // On tente de résoudre le nom donné par l'utilisateur, afin de récupérer l'adresse qui lui correspond
// et remplir ainsi la structure sockaddr_in
    recup = gethostbyname(NomDistant);
    if (recup == NULL) {
        perror("Erreur obtention adresse");
        return(-1);
    }

    /*
    On copie la zone mémoire qui contient la valeur de reecup (l'adresse IP du serveur) directement dans la zone mémoire 
    qui doit contenir cette valeur dans la structure sockaddr_in. 
    En paramètres :  la zone mémoire destination, la zone mémoire source,
    la longueur des données a copier.
    */
    memcpy((char *)&adresse.sin_addr, (char *)recup->h_addr, recup->h_length);
    
    // On utilise toujours une socket de type AF_INET
    adresse.sin_family = AF_INET;

    // On place le numéro de port
    // adresse.sin_port = htons((u_short) P);
    adresse.sin_port = htons((u_short) atoi(argv[2]));

    // On tente de se connecter au serveur : cette opération prend la place du accept que l'on a effectué dans le programme serveur.
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
	printf("Que voulez-vous écrire ? (--quit pour se deconnecter)\n");
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
	printf("Deconnecté par le serveur\n");
	exit(EXIT_FAILURE);
      }
    printf("Message envoyé\nEn attente...\n");

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
	    printf("Ecriture réussie\n");
	    printf("Que voulez-vous écrire ? (--quit pour se deconnecter)\n");
	    scanf(" %[^\n]",env);
	    if (write(sock, env, strlen(env)) == -1)
	      {
		perror("Erreur ecriture");
		return(-1);
	      }
	  }
	else if(!strcmp(rec,""))
	  {
	    printf("Deconnecté par le serveur : écriture échouée\n");
	    exit(EXIT_FAILURE);
	  }
	if(!strcmp(env, "--quit"))
	  break;
	printf("En attente...\n");
      }
    printf("Deconnecté\n");
    // On referme la socket cliente. Cette opération ferme la socket cliente pour le programme client, le serveur fait de même de son coté,
    // en plus de refermer sa propre socket.
    close(sock);
    
    return 0;
}
