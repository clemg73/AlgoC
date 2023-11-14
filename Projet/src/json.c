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
#include <ctype.h>

JsonObject parser(const char *data) {
    char* code = malloc(1024);
    char* value = malloc(1024);

    char* pos_start_code = strstr(data, "\"code\":\"");
    pos_start_code += strlen("\"code\":\"");
    char* pos_end_code = strchr(pos_start_code, '\"');
    snprintf(code, 1024, "%.*s", (int)(pos_end_code - pos_start_code), pos_start_code);

    char* pos_start_value = strstr(data, "\"valeurs\":[");
    pos_start_value += strlen("\"valeurs\":[");
    char* pos_end_value = strchr(pos_start_value, ']');
    snprintf(value, 1024, "%.*s", (int)(pos_end_value - pos_start_value), pos_start_value);

    char delim[] = ",";
    char **words = malloc(1024);
    char *ptr = strtok(value, delim);
    int wordCount = 0;

    JsonObject object;
    object.nb = 0;
    while (ptr != NULL) {
      //Si on détecte un " alors on supprime le premier et dernier caractère
      if(ptr[0]=='"')
      {
        for (int i = 0; i < strlen(ptr); i++) {
            ptr[i] = ptr[i+1];
        }
        ptr[strlen(ptr) - 1] = '\0';
      }
      words[wordCount] = ptr;
      wordCount++;
      object.nb++;
      ptr = strtok(NULL, delim);
    }
    object.code = code;
    object.valeurs = words;

    return object;
}

int isNumber(const char *str) {
    char *temp;
    strtod(str, &temp);
    return (temp != str);
}

char* serializator(char *code,char *data){
  char *message = malloc(1024);

  char *del1 = " ";
  char *del2 = ",";
  
  snprintf(message, 1024, "{\"code\":\"%s\",\"valeurs\":[",code);

  if(strcmp(code, "images") == 0){

    char *imgToken = strtok(data, del2);

    while (imgToken != 0)
    {
      int isNum = isNumber(imgToken);
      if(isNum == 0)
      {
        strcat(message, "\"");
        strcat(message, imgToken);
        strcat(message, "\",");     
      }else{
        strcat(message, imgToken);
        strcat(message, ","); 
      }
      imgToken = strtok(NULL, del2);
    }
  }
  else{
  
    char *token = strtok(data, del1);

    while (token != 0)
    {
        int isNum = isNumber(token);
        if(isNum == 0)
        {
            strcat(message, "\"");
            if(strcmp(code, "color") == 0|| strcmp(code, "balises")==0){
                strcat(message, "#");
            }
            strcat(message, token);
            strcat(message, "\",");
        }else{
            strcat(message, token);
            strcat(message, ",");
        }
        
        token = strtok(NULL, del1);
      
    }
  }
  int len = strlen(message);
    message[len - 1] = '\0';
  strcat(message, "]}");

  printf("Json sended: %s\n", message);

  return message;
}