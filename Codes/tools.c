#include "tool_serv.h"


/*
  creersock

  Il s'agit de la fonction qui va permettre la création d'une socket.
  Elle est utilisée dans la fonction main().
  Elle prend en paramètre un entier court non signé, qui est le numéro de port,
  nécéssaire à l'opération bind().
  Cette fonction renvoie un numéro qui permet d'identifier la socket nouvellement créée
  (ou la valeur -1 si l'opération a échouée).
*/

// version udp de creersock
int creersockudp( u_short port) {

  int sock, retour;

  // On crée une variable adresse selon la structure sockaddr_in
  struct sockaddr_in adresse;
  
  sock = socket(AF_INET,SOCK_DGRAM,0);

  // Si le code retourné n'est pas un identifiant valide (la création s'est mal passée), on affiche un message sur la sortie d'erreur, et on renvoie -1
  if (sock<0) {
    perror ("ERREUR OUVERTURE");
    return(-1);
  }
  
  // On complète les champs de la structure sockaddr_in
  adresse.sin_family = AF_INET;

  adresse.sin_port = htons(port);

  adresse.sin_addr.s_addr=INADDR_ANY;

  retour = bind (sock,(struct sockaddr *)&adresse,sizeof(adresse));
  
  // En cas d'erreur lors du bind, on affiche un message d'erreur et on renvoie -1
  if (retour<0) {
    perror ("IMPOSSIBLE DE NOMMER LA SOCKET");
    return(-1);
  }

  // Au final, on renvoie sock, qui contient l'identifiant à la socket crée et attachée.
  return (sock);
}

// version tcp de creersock
int creersocktcp( u_short port) {

  int sock, retour;

  // On crée une variable adresse selon la structure sockaddr_in (la structure est décrite dans sys/socket.h)
  struct sockaddr_in adresse;
  
  sock = socket(AF_INET,SOCK_STREAM,0);

  // Si le code retourné n'est pas un identifiant valide (la création s'est mal passée), on affiche un message sur la sortie d'erreur, et on renvoie -1
  if (sock<0) {
    perror ("ERREUR OUVERTURE");
    return(-1);
  }
  
  // On complète les champs de la structure sockaddr_in : 
  // La famille du socket, AF_INET, comme cité précédement
  adresse.sin_family = AF_INET;

  adresse.sin_port = htons(port);

  adresse.sin_addr.s_addr=INADDR_ANY;
  
  retour = bind (sock,(struct sockaddr *)&adresse,sizeof(adresse));
  
  // En cas d'erreur lors du bind, on affiche un message d'erreur et on renvoie -1
  if (retour<0) {
    perror ("IMPOSSIBLE DE NOMMER LA SOCKET");
    return(-1);
  }

  // Au final, on renvoie sock, qui contient l'identifiant à la socket crée et attachée.
  return (sock);
}

int ecrire(char *m)
{
  FILE *fic = fopen(FIC, "a");
  if(fwrite(m, sizeof(char), strlen(m), fic) == 0)
    perror("Erreur ecriture fichier ");
  fclose(fic);
  return 0;
}

int est_libre()
{
  return 1;
}
