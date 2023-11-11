/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <math.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "serveur.h"
int socketfd;

int visualize_plot()
{
  const char *browser = "firefox";

  char command[256];
  snprintf(command, sizeof(command), "%s %s", browser, svg_file_path);

  int result = system(command);

  if (result == 0)
  {
    printf("SVG file opened in %s.\n", browser);
  }
  else
  {
    printf("Failed to open the SVG file.\n");
  }

  return 0;
}

double degreesToRadians(double degrees)
{
  return degrees * M_PI / 180.0;
}

int plot(char *data)
{
  int i;
  char *saveptr = NULL;
  
  char *str = data;
  
  char *token = strtok_r(str, ",", &saveptr);
  char *subtoken = strtok(token, ": ");

  int numberofColors;

  while(subtoken != 0){
    printf("%s : %li\n", subtoken, strlen(subtoken));

    //Comme N < 30, on sait qu'il y a pas plus de deux chiffres
    if(strlen(subtoken)<=2){
      numberofColors = atoi(subtoken);
      break;
    }
    else
    {
      subtoken = strtok(0, ": ");    
    }
    
  }

  printf("%i\n", numberofColors);

  double angles[numberofColors];
  memset(angles, 0, sizeof(angles));

  FILE *svg_file = fopen(svg_file_path, "w");
  if (svg_file == NULL)
  {
    perror("Error opening file");
    return 1;
  }

  fprintf(svg_file, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
  fprintf(svg_file, "<svg width=\"400\" height=\"400\" xmlns=\"http://www.w3.org/2000/svg\">\n");
  fprintf(svg_file, "  <rect width=\"100%%\" height=\"100%%\" fill=\"#ffffff\" />\n");

  double center_x = 200.0;
  double center_y = 200.0;
  double radius = 150.0;

  double start_angle = -90.0;

  str = NULL;
  i = 0;
  while (1)
  {
    token = strtok_r(str, ",", &saveptr);
    if (token == NULL)
    {
      break;
    }
    str = NULL;
    angles[i] = 360.0 / numberofColors;

    double end_angle = start_angle + angles[i];

    double start_angle_rad = degreesToRadians(start_angle);
    double end_angle_rad = degreesToRadians(end_angle);

    double x1 = center_x + radius * cos(start_angle_rad);
    double y1 = center_y + radius * sin(start_angle_rad);
    double x2 = center_x + radius * cos(end_angle_rad);
    double y2 = center_y + radius * sin(end_angle_rad);

    fprintf(svg_file, "  <path d=\"M%.2f,%.2f A%.2f,%.2f 0 0,1 %.2f,%.2f L%.2f,%.2f Z\" fill=\"%s\" />\n",
            x1, y1, radius, radius, x2, y2, center_x, center_y, token);

    start_angle = end_angle;
    i++;
  }

  fprintf(svg_file, "</svg>\n");

  fclose(svg_file);

  visualize_plot();
  return 0;
}

/* renvoyer un message (*data) au client (client_socket_fd)
 */
int renvoie_message(int client_socket_fd, char *data)
{
  int data_size = write(client_socket_fd, (void *)data, strnlen(data, 1024));

  if (data_size < 0)
  {
    perror("erreur ecriture");
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

int renvoie_nom(int client_socket_fd, char *data)
{
  int data_size = write(client_socket_fd, (void *)data, strnlen(data, 1024));

  if (data_size < 0)
  {
    perror("erreur ecriture");
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

int renvoie_couleur(int client_socket_fd, char data[1024]){
  char delim[] = " ";
  char colors[100][100];
  int colorsCount = 0;

  char *ptr = strtok(data, delim);

  while (ptr != NULL) {
    strcpy(colors[colorsCount], ptr);
    colorsCount++;
    ptr = strtok(NULL, delim);
  }

  FILE *fp;
  fp = fopen("colors.txt", "w");

  for(int i = 1; i < colorsCount; i++){
    printf("écriture de couleur %i...: #%s\n", i, colors[i]);
    fprintf(fp, "#%s\n",colors[i]);
  }
  fclose(fp);

  char newData[] = "enregistrees";
  int data_size = write(client_socket_fd, (void *)newData, strnlen(newData, 1024));

  if (data_size < 0)
  {
    perror("erreur ecriture");
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

int renvoie_balise(int client_socket_fd, char data[1024]){
  char delim[] = " ";
  char balises[100][100];
  int balisesCount = 0;

  char *ptr = strtok(data, delim);

  while (ptr != NULL) {
    strcpy(balises[balisesCount], ptr);
    balisesCount++;
    ptr = strtok(NULL, delim);
  }

  FILE *fp;
  fp = fopen("balises.txt", "w");

  for(int i = 1; i < balisesCount; i++){
    printf("écriture de balise %i... %s\n", i, balises[i]);
    fprintf(fp, "#%s\n", balises[i]);
  }
  fclose(fp);

  char newData[] = "enregistrees";
  int data_size = write(client_socket_fd, (void *)newData, strnlen(newData, 1024));

  if (data_size < 0)
  {
    perror("erreur ecriture");
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}


int recois_numeros_calcule(int client_socket_fd, char data[1024])
{
  char delim[] = " ";
  
  char words[100][100];

  char *ptr = strtok(data, delim);
  int wordCount = 0;

  while (ptr != NULL) {
      strcpy(words[wordCount], ptr);
      wordCount++;
      ptr = strtok(NULL, delim);
  }

  double number1;
  double number2;
  number1 = strtod(words[2], NULL);
  number2 = strtod(words[3], NULL);
  double result;
  
  if(strcmp(words[1],"+")==0)
  {
    result = number1+number2;
  }
  else if(strcmp(words[1],"-")==0)
  {
    result = number1-number2;
  }
  else  if(strcmp(words[1],"x")==0)
  {
    result = number1*number2;
  }
  else  if(strcmp(words[1],"/")==0)
  {
    result = number1/number2;
  }

  char res[15] = {0};
  sprintf(res, "%.2lf", result);
  int data_size = write(client_socket_fd, (void *)res, strnlen(data, 1024));

  if (data_size < 0)
  {
    perror("erreur ecriture");
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

/* accepter la nouvelle connection d'un client et lire les données
 * envoyées par le client. En suite, le serveur envoie un message
 * en retour
 */
int recois_envoie_message(int client_socket_fd, char data[1024])
{
  /*
   * extraire le code des données envoyées par le client.
   * Les données envoyées par le client peuvent commencer par le mot "message :" ou un autre mot.
   */
  char code[10];
  sscanf(data, "%s", code);
  // Si le message commence par le mot: 'message:'
  if (strcmp(code, "message:") == 0)
  {
    renvoie_message(client_socket_fd, data);
  }
  else if (strcmp(code, "nom:") == 0)
  {
    renvoie_nom(client_socket_fd, data);
  }
  else if (strcmp(code, "images:") == 0)
  {
    plot(data);
  }
  else if (strcmp(code, "color:") == 0)
  {
    renvoie_couleur(client_socket_fd, data);
  }
  else if (strcmp(code, "balises:") == 0)
  {
    renvoie_balise(client_socket_fd, data);
  }
  else if (strcmp(code, "calcul:") == 0)
  {
    recois_numeros_calcule(client_socket_fd, data);
  }
  return (EXIT_SUCCESS);
}


// Fonction de gestion du signal Ctrl+C
void gestionnaire_ctrl_c(int signal)
{
  printf("\nSignal Ctrl+C capturé. Sortie du programme.\n");
  // fermer le socket
  close(socketfd);
  exit(0); // Quitter proprement le programme.
}

int main()
{
  int bind_status;

  struct sockaddr_in server_addr;

  /*
   * Creation d'une socket
   */
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0)
  {
    perror("Unable to open a socket");
    return -1;
  }

  int option = 1;
  setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

  // détails du serveur (adresse et port)
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // Relier l'adresse à la socket
  bind_status = bind(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (bind_status < 0)
  {
    perror("bind");
    return (EXIT_FAILURE);
  }

  // Enregistrez la fonction de gestion du signal Ctrl+C
  signal(SIGINT, gestionnaire_ctrl_c);

  // Écouter les messages envoyés par le client en boucle infinie
  while (1)
  {
    // Écouter les messages envoyés par le client
    listen(socketfd, 10);

    // Lire et répondre au client
    struct sockaddr_in client_addr;
    char data[1024];

    unsigned int client_addr_len = sizeof(client_addr);

    // nouvelle connection de client
    int client_socket_fd = accept(socketfd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_socket_fd < 0)
    {
      perror("accept");
      return (EXIT_FAILURE);
    }

    // la réinitialisation de l'ensemble des données
    memset(data, 0, sizeof(data));

    // lecture de données envoyées par un client
    int data_size = read(client_socket_fd, (void *)data, sizeof(data));

    if (data_size < 0)
    {
      perror("erreur lecture");
      return (EXIT_FAILURE);
    }

    recois_envoie_message(client_socket_fd, data);
  }

  return 0;
}
