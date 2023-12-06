/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Code permettant de stocker différents types de couleurs : format 24 bits (RGB) ou format 32 bits (32). 
 */

 #ifndef __JSON_H__
 #define __JSON_H__

typedef struct  {
    char *code;
    char **valeurs;
    int nb;
} JsonObject;

JsonObject parser(const char*);
char* serializator(char*,char*);

struct SocketObject{
    char *name;
    int socket;
};

#endif