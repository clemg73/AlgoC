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
#include "json.h"
#include "serveur.h"

#define MAXCLIENTS 3

int socketfd;
struct SocketObject procSocket = {"rien",0};
struct SocketObject clientSockets[MAXCLIENTS];

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

int plot(char **data)
{
  int numberofColors = atoi(data[0]);

  printf("%i\n", numberofColors);

  double angles[numberofColors + 1];
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

  for (int i = 1; i < numberofColors; i++)
  {
   
    angles[i] = 360.0 / numberofColors;

    double end_angle = start_angle + angles[i];

    double start_angle_rad = degreesToRadians(start_angle);
    double end_angle_rad = degreesToRadians(end_angle);

    double x1 = center_x + radius * cos(start_angle_rad);
    double y1 = center_y + radius * sin(start_angle_rad);
    double x2 = center_x + radius * cos(end_angle_rad);
    double y2 = center_y + radius * sin(end_angle_rad);

    fprintf(svg_file, "  <path d=\"M%.2f,%.2f A%.2f,%.2f 0 0,1 %.2f,%.2f L%.2f,%.2f Z\" fill=\"%s\" />\n",
            x1, y1, radius, radius, x2, y2, center_x, center_y, data[i]);

    start_angle = end_angle;
  }

  fprintf(svg_file, "</svg>\n");

  fclose(svg_file);

  visualize_plot();
  return 0;
}

/* renvoyer un message (*data) au client (client_socket_fd)
 */
int renvoie_message(int client_socket_fd, char **data)
{
  char *data_send = serializator("message",data[0]);

  int data_size = write(client_socket_fd, (void *)data_send, strnlen(data_send, 1024));
  free(data);
  if (data_size < 0)
  {
    perror("erreur ecriture");
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

int renvoie_nom(int client_socket_fd, char **data)
{
  char *data_send = serializator("nom",data[0]);

  int data_size = write(client_socket_fd, (void *)data_send, strnlen(data_send, 1024));
  free(data);
  if (data_size < 0)
  {
    perror("erreur ecriture");
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

int renvoie_couleur(int client_socket_fd, char **colors){

  FILE *fp;
  char file[1024] = "colors_";
  char str[10];

  for (int i = 0; i < MAXCLIENTS; i++) {

    if (clientSockets[i].socket == client_socket_fd) {

        strcat(file, clientSockets[i].name);

        strcat(file, "_");
        sprintf(str, "%d", clientSockets[i].socket);
        strcat(file, str);
        strcat(file, ".txt");

        break;
    }
  }

  fp = fopen(file, "w");

  for(int i = 1; i <= atoi(colors[0]); i++){
    printf("écriture de couleur %i...: %s\n", i, colors[i]);
    fprintf(fp, "%s\n",colors[i]);
  }
  fclose(fp);

  free(colors[0]);
  free(colors);

  char *data = serializator("color","enregistré");
  int data_size = write(client_socket_fd, (void *)data, strnlen(data, 1024));

  if (data_size < 0)
  {
    perror("erreur ecriture");
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

int renvoie_balise(int client_socket_fd, char **balises){

  FILE *fp;
  char file[1024] = "balises_";
  char str[10];

  for (int i = 0; i < MAXCLIENTS; i++) {

    if (clientSockets[i].socket == client_socket_fd) {

        strcat(file, clientSockets[i].name);

        strcat(file, "_");
        sprintf(str, "%d", clientSockets[i].socket);
        strcat(file, str);
        strcat(file, ".txt");

        break;
    }
  }

  fp = fopen(file, "w");

  for(int i = 1; i <= atoi(balises[0]); i++){
    printf("écriture de balises %i...: %s\n", i, balises[i]);
    fprintf(fp, "%s\n",balises[i]);
  }
  fclose(fp);


  free(balises[0]);
  free(balises);


  char *data = serializator("balises","enregistré");
  int data_size = write(client_socket_fd, (void *)data, strnlen(data, 1024));


  if (data_size < 0)
  {
    perror("erreur ecriture");
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}


int recois_numeros_calcule(int client_socket_fd, char **words, int nb)
{
 
  double result;
  
  
  if(strcmp(words[0],"+")==0 || strcmp(words[0],"-")==0 || strcmp(words[0],"x")==0 || strcmp(words[0],"/")==0)
  {
    double number1;
    double number2;
    number1 = strtod(words[1], NULL);
    number2 = strtod(words[2], NULL);
    if(strcmp(words[0],"+")==0)
    {
      result = number1+number2;
    }
    else if(strcmp(words[0],"-")==0)
    {
      result = number1-number2;
    }
    else  if(strcmp(words[0],"x")==0)
    {
      result = number1*number2;
    }
    else  if(strcmp(words[0],"/")==0)
    {
      result = number1/number2;
    }else{
      result = 0;
    }
  }else if(strcmp(words[0],"avg")==0 || strcmp(words[0],"min")==0 || strcmp(words[0],"max")==0 || strcmp(words[0],"e-t")==0)
  {
    if(strcmp(words[0],"avg")==0)
    {
      result = 0;
      
      for (int compteur = 1 ; compteur <  nb; compteur++)
      {
        result += strtod(words[compteur], NULL);
      };
      result = result/(nb-1);
    }
    else if(strcmp(words[0],"min")==0)
    {
      result = strtod(words[1], NULL);
      for (int compteur = 2 ; compteur <  nb; compteur++)
      {
        if(strtod(words[compteur], NULL) < result)
        {
          result = strtod(words[compteur], NULL);
        }
      };
    }
    else  if(strcmp(words[0],"max")==0)
    {
      result = strtod(words[1], NULL);
      
      for (int compteur = 2 ; compteur <  nb; compteur++)
      {
        if(strtod(words[compteur], NULL) > result)
        {
          result = strtod(words[compteur], NULL);
        }
      };
    }
    else  if(strcmp(words[0],"e-t")==0)
    {
      double avg = 0;
      for (int j = 1; j <  nb; j++)
      {
        avg += strtod(words[j], NULL);
      };
      avg = avg/(nb-1);
      double sommeCarresEcarts = 0.0;
      for (int i = 1; i < nb; i++) {
        double ecart = strtod(words[i], NULL) - avg;
        sommeCarresEcarts += ecart * ecart;
      }
      double variance = sommeCarresEcarts / (nb-1);
            

      result = sqrt(variance);
    }else{
      result = 0;
    }
  }else{
    result = 0;
  }

  char res[15] = {0};
  sprintf(res, "%.2lf", result);
  char *data = serializator("calcul",res);
  int data_size = write(client_socket_fd, (void *)data, strnlen(data, 1024));

  free(words);
  if (data_size < 0)
  {
    perror("erreur ecriture");
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

int decode(char* data, char* code, char* value){

  char* pos_start_code = strstr(data, "\"code\":\"");
  pos_start_code += strlen("\"code\":\"");
  char* pos_end_code = strchr(pos_start_code, '\"');
  snprintf(code, 1024, "%.*s", (int)(pos_end_code - pos_start_code), pos_start_code);

  char* pos_start_value = strstr(data, "\"valeurs\":[");
  pos_start_value += strlen("\"valeurs\":[");
  char* pos_end_value = strchr(pos_start_value, ']');
  snprintf(value, 1024, "%.*s", (int)(pos_end_value - pos_start_value-2), pos_start_value+1);

  return 0;
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

  JsonObject object = parser(data);

  // Si le message commence par le mot: 'message:'
  if (strcmp(object.code,"message")==0)
  {
    free(object.code);
    renvoie_message(client_socket_fd, object.valeurs);
  }
  else if (strcmp(object.code,"nom")==0)
  {
    free(object.code);
    renvoie_nom(client_socket_fd, object.valeurs);
  }
  else if (strcmp(object.code,"images")==0)
  {
    free(object.code);
    plot(object.valeurs);
  }
  else if (strcmp(object.code,"color")==0)
  {   
    free(object.code);
    renvoie_couleur(client_socket_fd, object.valeurs);
  }
  else if (strcmp(object.code,"balises")==0)
  {
    free(object.code);
    renvoie_balise(client_socket_fd, object.valeurs);
  }
  else if (strcmp(object.code,"calcul")==0)
  {
    free(object.code);
    recois_numeros_calcule(client_socket_fd, object.valeurs,object.nb);
  }else if (strcmp(object.code,"machine")==0)
  {
    free(object.code);
    for (int i = 0; i < MAXCLIENTS; i++) {
        if (clientSockets[i].socket == client_socket_fd) {
            clientSockets[i].name = object.valeurs[0];
            break; 
        }
    };
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

void split_requests( char *data, const char *delimiter, char ***requests, int *num_requests) {
    char *copy = strdup(data);  // Faites une copie de la chaîne originale
    char *token = strtok(copy, delimiter);
    *num_requests = 0;

    // Comptez le nombre de requêtes
    while (token != NULL) {
        (*num_requests)++;
        token = strtok(NULL, delimiter);
    }

    // Allouez de la mémoire pour le tableau de requêtes
    *requests = (char **)malloc(sizeof(char *) * (*num_requests));

    // Remplissez le tableau de requêtes
    token = strtok(data, delimiter);
    for (int i = 0; i < *num_requests; i++) {
        (*requests)[i] = strdup(token);
        token = strtok(NULL, delimiter);
    }

    free(copy);  // Libérez la mémoire de la copie temporaire
}

int main()
{
  for (int i = 0; i < MAXCLIENTS; i++) {
    clientSockets[i].name = "null";
    clientSockets[i].socket = 0;
  }

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

  struct SocketObject so;
  so.name = "process socket";
  so.socket = socketfd;
  clientSockets[0] = so;

  fd_set readfds;

  // Écouter les messages envoyés par le client en boucle infinie
  while (1)
  {
    listen(socketfd, 10);

    FD_ZERO(&readfds);
    FD_SET(socketfd, &readfds);
    int maxSocket = socketfd;
    for (int i = 0; i < MAXCLIENTS; ++i) {
        int clientSocket = clientSockets[i].socket;
        if (clientSocket > 0) {
            FD_SET(clientSocket, &readfds);
        }
        if (clientSocket > maxSocket) {
            maxSocket = clientSocket;
        }
    }
    select(maxSocket + 1, &readfds, NULL, NULL, NULL);
    
    struct sockaddr_in client_addr;
    unsigned int client_addr_len = sizeof(client_addr);
    int client_socket_fd;

    if(FD_ISSET(socketfd,&readfds))
    {
      int newSocket = accept(socketfd, (struct sockaddr *)&client_addr, &client_addr_len);

      int position = -1; // Initialisation à -1 pour le cas où le zéro n'est pas trouvé
      for (int i = 0; i < MAXCLIENTS; i++) {
          if (clientSockets[i].socket == 0) {
              position = i;
              break; // Sortir de la boucle une fois que le premier zéro est trouvé
          }
      }
     
      if (position == -1) {
          printf("Number of maximum clients exceed\n");
      }else{

        clientSockets[position].socket = newSocket;
        clientSockets[position].name = "waitings";


        client_socket_fd = newSocket;
        printf("Create new socket\n");
      }
    }else{
      for (int i = 1; i < MAXCLIENTS; i++) {
        int clientSocket = clientSockets[i].socket;
        if(FD_ISSET(clientSocket,&readfds))
        {
          client_socket_fd = clientSocket;

          char data[1024];
          // la réinitialisation de l'ensemble des données
          memset(data, 0, sizeof(data));

          // lecture de données envoyées par un client
          int data_size = read(client_socket_fd, (void *)data, sizeof(data));
          if (data_size < 0)
          {
            perror("erreur lecture");
            return (EXIT_FAILURE);
          }else if(data_size == 0)
          {
            printf("Close socket %i\n",client_socket_fd);
            close(client_socket_fd);
            clientSockets[i].socket=0;
            clientSockets[i].name="null";
          }else{
            char **requests;
            int num_requests;
            split_requests(data, "}{", &requests, &num_requests);

            for (int i = 0; i < num_requests; i++) {
              printf("Treatment of {%s} by %i\n",requests[i],client_socket_fd);
              recois_envoie_message(client_socket_fd, requests[i]);
              free(requests[i]);
            }

            free(requests);

          }
        }
      }
    }   
    printf("Sockets list after treatment: (name --> socket)");
    for (int i = 0; i < MAXCLIENTS; i++) {
      printf("\n  %s --> %d", clientSockets[i].name, clientSockets[i].socket);
    }
    printf("\n--------------\n");
  }

  return 0;
}