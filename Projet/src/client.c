/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "client.h"
#include "bmp.h"

char* parser(char *code,char *data){
  char *message = malloc(1024);
  snprintf(message, 1024, "{\"code\":\"%s\",\"valeurs\":[\"%s\"]}",code,data);
  return message;
}

int envoie_nom_de_client(int socketfd){

  char name[1024];
  int state = gethostname(name, 1024);


  if(state == -1){
    perror("hostname");
    exit(EXIT_FAILURE);
  }
  char *data = parser("nom",name);
  int write_status = write(socketfd, data, strlen(data));

  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, strlen(data));

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if (read_status < 0)
  {
    perror("erreur lecture");
    return -1;
  }

  printf("Nom reçu: %s\n", data);
  free(data);
  return 0;
}


int envoie_operateur_numeros(int socketfd, char *operateur, double nb1, double nb2)
{
    printf("Calcul opération: %s\n", operateur);
    printf("Nombre 1: %lf\n", nb1);
    printf("Nombre 2: %lf\n", nb2);

    char operation[1024] = ". ";

    char nombre1[15] = {0};
    sprintf(nombre1, "%.2lf", nb1);

    char nombre2[15] = {0};
    sprintf(nombre2, "%.2lf", nb2);

    strcat(operation, operateur);
    strcat(operation, " ");
    strcat(operation, nombre1);
    strcat(operation, " ");
    strcat(operation, nombre2);

    char *data = parser("calcul",operation);
    int write_status = write(socketfd, data, strlen(data));
    if (write_status < 0)
    {
      perror("erreur ecriture");
      exit(EXIT_FAILURE);
    }
  
    // la réinitialisation de l'ensemble des données
    memset(data, 0, strlen(data));

    // lire les données de la socket
    int read_status = read(socketfd, data, sizeof(data));
    if (read_status < 0)
    {
      perror("erreur lecture");
      return -1;
    }

    printf("Resulat du calcul reçu: %s\n", data);

    return 0;

}

int envoie_couleurs(int socketfd, char *myList[], int len){
  int compteur;
  char colors[1024] = ". ";
  for (compteur = 0 ; compteur <  len; compteur++)
  { 
      strcat(colors, myList[compteur]);
      strcat(colors, " ");
  }

  char *data = parser("color",colors);
  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, strlen(data));

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if (read_status < 0)
  {
    perror("erreur lecture");
    return -1;
  }

  printf("Couleurs: %s\n", data);
  return 0;
}


int envoie_balise(int socketfd, char *myList[], int len){
  
  int compteur;
  char colors[1024] = ". ";
  for (compteur = 0 ; compteur <  len; compteur++)
  { 
      strcat(colors, myList[compteur]);
      strcat(colors, " ");
  }

  char *data = parser("balises",colors);
  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, strlen(data));

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if (read_status < 0)
  {
    perror("erreur lecture");
    return -1;
  }
  
  printf("Balises: %s\n", data);
  return 0;
}

/*
 * Fonction d'envoi et de réception de messages
 * Il faut un argument : l'identifiant de la socket
 */

int envoie_recois_message(int socketfd)
{

  // Demandez à l'utilisateur d'entrer un message
  char message[1024];
  printf("Votre message (max 1000 caracteres): ");

  fgets(message, sizeof(message), stdin);

  char *data = parser("message",message);
  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, strlen(data));

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if (read_status < 0)
  {
    perror("erreur lecture");
    return -1;
  }

  printf("Message recu: %s\n", data);

  return 0;
}


void analyse(char *pathname, char *data, int numberofColors)
{
  // compte de couleurs
  couleur_compteur *cc = analyse_bmp_image(pathname);

  int count;
  strcpy(data, "couleurs: ");
  
  //Formatter la chaîne
  char temp_string[numberofColors];
  sprintf(temp_string, "%d, ", numberofColors);

  if (cc->size < numberofColors && numberofColors < 30)
  {
    sprintf(temp_string, "%d,", cc->size);
  }
  strcat(data, temp_string);

  // choisir n couleurs
  for (count = 1; count < numberofColors+1 && cc->size - count > 0; count++)
  {
    if (cc->compte_bit == BITS32)
    {
      sprintf(temp_string, "#%02x%02x%02x,", cc->cc.cc24[cc->size - count].c.rouge, cc->cc.cc32[cc->size - count].c.vert, cc->cc.cc32[cc->size - count].c.bleu);
    }
    if (cc->compte_bit == BITS24)
    {
      sprintf(temp_string, "#%02x%02x%02x,", cc->cc.cc32[cc->size - count].c.rouge, cc->cc.cc32[cc->size - count].c.vert, cc->cc.cc32[cc->size - count].c.bleu);
    }
    strcat(data, temp_string);
  }

  // enlever le dernier virgule
  data[strlen(data) - 1] = '\0';
}

int envoie_images(int socketfd, char *pathname, int numberofColors)
{
  char data[1024];
    memset(data, 0, sizeof(data));

  analyse(pathname, data, numberofColors);


  char *temp = parser("images",data);

  int write_status = write(socketfd, temp, strlen(temp));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  return 0;
}

int main(int argc, char **argv)
{
  int socketfd;

  struct sockaddr_in server_addr;

  if (argc < 2)
  {
    printf("usage: ./client chemin_bmp_image\n");
    return (EXIT_FAILURE);
  }

  /*
   * Creation d'une socket
   */
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0)
  {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // détails du serveur (adresse et port)
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // demande de connection au serveur
  int connect_status = connect(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (connect_status < 0)
  {
    perror("connection serveur");
    exit(EXIT_FAILURE);
  }


  if (strcmp(argv[1],"-nom") == 0)
  {
    envoie_nom_de_client(socketfd);
  }else if (strcmp(argv[1],"-image") == 0)
  {
    envoie_images(socketfd, argv[2], atoi(argv[3]));
  }else if (strcmp(argv[1],"-message") == 0)
  {
    envoie_recois_message(socketfd);
  }else if (strcmp(argv[1],"-calcul") == 0)
  {
    envoie_operateur_numeros(socketfd,argv[2],atof(argv[3]),atof(argv[4]));
  }else if (strcmp(argv[1],"-color") == 0)
  {
    char *myList[atoi(argv[2])];
    int compteur;
    for (compteur = 0 ; compteur <=  atoi(argv[2]); compteur++)
    {
        myList[compteur] = argv[compteur+3];
    }
    envoie_couleurs(socketfd,myList,atoi(argv[2]));
  }else if (strcmp(argv[1],"-balises") == 0)
  {
    char *myList[atoi(argv[2])];
    int compteur;
    for (compteur = 0 ; compteur <=  atoi(argv[2]); compteur++)
    {
      myList[compteur] = argv[compteur+3];
    }
    envoie_balise(socketfd,myList,atoi(argv[2]));
  }

  close(socketfd);
}
