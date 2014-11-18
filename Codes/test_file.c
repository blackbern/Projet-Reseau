#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main() {
  FILE *fic, *fic2;
  fic = fopen("/home/blackbern/M1/Reseau/projet/Codes/Essai.txt", "w");
  if(fic == NULL)
    perror("erreur open : ");
  fic2 = fopen("/home/blackbern/M1/Reseau/projet/Codes/Essai.txt", "w");
  if(fic2 == NULL)
    perror("erreur open2 :");
  fwrite("Open", sizeof(char), 4, fic);
  fwrite("Open2", sizeof(char), 5, fic2);
  fclose(fic);
  fclose(fic2);
}
