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

JsonObject parser(const char *data) {
    char* code = malloc(1024000);
    char* value = malloc(1024000);

    char* pos_start_code = strstr(data, "\"code\":\"");
    pos_start_code += strlen("\"code\":\"");
    char* pos_end_code = strchr(pos_start_code, '\"');
    snprintf(code, 102400, "%.*s", (int)(pos_end_code - pos_start_code), pos_start_code);

    char* pos_start_value = strstr(data, "\"valeurs\":[");
    pos_start_value += strlen("\"valeurs\":[");
    char* pos_end_value = strchr(pos_start_value, ']');
    snprintf(value, 102400, "%.*s", (int)(pos_end_value - pos_start_value), pos_start_value);

    char delim[] = ",";
    int compteur = 0;

    for (int i = 0; value[i] != '\0'; i++) {
        if (value[i] == ',') {
            compteur++;
        }
    }
    char **words = malloc(1024000);
    char *ptr = strtok(value, delim);
    int wordCount = 0;

    JsonObject object;
    object.nb = 0;

    while (ptr != NULL) {
        for (int i = 0; i < strlen(ptr); i++) {
            ptr[i] = ptr[i+1];
        }
        ptr[strlen(ptr) - 1] = '\0';
        words[wordCount] = ptr;
        wordCount++;
        object.nb++;
        ptr = strtok(NULL, delim);
    }

    object.code = code;
    object.valeurs = words;

    return object;
}