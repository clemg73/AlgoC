
/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/* 
 * Le but principal de ce code est de s'assurer que les messages envoyés par le client sont
 * valides, c'est-à-dire que les messages respectent le format JSON et
 * respectent le protocole spécifié par le côté serveur.
 */

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "json.h"

/* 
    Cette fonction vérifie si le caractère passé en paramètre est un guillemet
        - Retourne 0 si c'est vrai, -1 sinon 
*/

int isAQuote(char character){

    if(character == '\"' || character == '\''){
        return 0;
    }else{
        return -1;
    }
}


/*
    Vérifie la valeur du code du JSON
*/

int isThisCode(char* string, char* code){

    char _string[100];
    strcpy(_string, string);

    char *del = ",";
    char *imgToken = strtok(_string, del);

    if(strstr(imgToken, code) != NULL){
        return 0;
    } 
    return 1;

}

/* 
    Cette fonction vérifie si le caractère passé en paramètre est un caractère spécial
        - Retourne 0 si c'est vrai, -1 sinon 
*/

int isASpecialCharacter(char character) {
    const char* specialCharacters = ",#[]:{}";
    char specialChar;

    for (int i = 0; i < strlen(specialCharacters); i++) {
        specialChar = specialCharacters[i];

        if (character == specialChar) {
            return 0;
        }
    }

    return -1;
}

/* 
    Cette fonction vérifie si le caractère passé en paramètre est un opérateur autorisé
        - Retourne 0 si c'est vrai, -1 sinon 
*/

int isAnOperator(char character) {
    const char* specialCharacters = "+-x/";
    char specialChar;

    for (int i = 0; i < strlen(specialCharacters); i++) {
        specialChar = specialCharacters[i];

        if (character == specialChar) {
            return 0;
        }
    }

    return -1;
}


/* 
    Cette fonction vérifie si le mot passé en paramètre est une balise
        - Retourne 0 si c'est vrai, -1 sinon 
*/

int isABalise(char *string) {

    char character = *string;

    if(character != '#' || strlen(string) <= 1){
        return -1;
    }

    return 0;
}

/* 
    Cette fonction renvoi la liste des valeurs passés dans le JSON 
*/

char* getParams(char *string) {

    char *params = malloc(1024);
    char character;
    int start = 1000000;

    for(int i = 0; i < strlen(string); i++){

        character = string[i];

        if(character == '['){
            start = i;
        }

        if(i >= start && character != '}'){
            params[i-start] = string[i];
        } 
    }

    return params;
}



/* 
    Cette fonction a pour but de vérifier la bonne conformité des guillemets:
        - Si le nombre de guillemets présents est pair
        - Si les chaînes de caractères sont encadrés de guillemets
    Renvoie 0 si les guillemets sont conformes
*/

int quotesChecking(char* jsonStringify)
{
    int numberofQuotes = 0;
    char character;

    for (int i = 0; i < strlen(jsonStringify); i++)
    {
            character = jsonStringify[i];

            if(isAQuote(character) == 0){
               numberofQuotes++;
            }

            //Si le nombre de guillements est pair, un guillement est fermé
            if(numberofQuotes%2 == 0 && isASpecialCharacter(character) != 0 && isdigit(character) == 0 && isAQuote(character) != 0){
                puts("There's a value without quotes");
                return(EXIT_FAILURE);
            }

    }
    
    printf("number of quotes : %i\n", numberofQuotes);

    if(numberofQuotes%2 == 0){
        return 0;
    }else
    {
        puts("There's a quote no expected");
        return(EXIT_FAILURE);
    }
    
}


/* 
    Cette fonction a pour but de vérifier la conformité des calculs:
        - Si les paramètres contiennent bien une opération et 2 nombres
    Renvoie 0 si le test est okay, -1 sinon
*/

int operationChecking(char* jsonStringify){
    if(isThisCode(jsonStringify ,"calcul") == 0){

        char *params = getParams(jsonStringify);

        char character;
        char args[10];
        int nbquotes = 0;
        int arrayPos;
        int numArg = 0;

        for(int i = 0; i< strlen(params); i++){

            character = params[i];
            if(character == '\"' && nbquotes%2 == 0){
                nbquotes ++;
                arrayPos = 0;
            }
            else if(character == '\"' && nbquotes%2 != 0){
                nbquotes ++;

                if(numArg == 0 && strlen(args) > 1){
                    puts("First argument too long");
                    return(EXIT_FAILURE);
                }
                else{
                    if(numArg == 0 && strlen(args) == 1){
                        if(isAnOperator(args[0]) != 0){
                            puts("First argument must be an operator in +-x/");
                            return(EXIT_FAILURE);
                        }
                    }
                    else if(numArg > 0){
                        if(!atoi(args)){
                            puts("Not a number");
                            return(EXIT_FAILURE);
                        }
                    }
                }

                numArg++;
            }
            
            if(nbquotes%2 != 0 && character != '\"'){
                args[arrayPos] = character;
                arrayPos++;
            }

        }
        free(params);
        return 0;
    }
    return -1;
}

/* 
    Cette fonction a pour but de vérifier la conformité des couleurs:
        - Si les paramètres contiennent bien un nombre de couleurs et X couleurs
    Renvoie 0 si le test est okay, -1 sinon
*/
int colorChecking(char* jsonStringify){
    if(isThisCode(jsonStringify, "color") == 0){

        char *params = getParams(jsonStringify);
        char character;
        int nbquotes = 0;
        int arrayPos;
        char args[32];
        int numArg = 0;

        int nbColors; 


        for(int i = 0; i< strlen(params); i++){

            character = params[i];

            if(character == '\"' && nbquotes%2 == 0){
                nbquotes ++;
                arrayPos = 0;
            }
            else if(character == '\"' && nbquotes%2 != 0){
                nbquotes ++;

                if(numArg == 0 && (!atoi(args) || atoi(args) < 1 || atoi(args) > 30)){
                    puts("First argument must be a integer between 1 and 30");
                    return(EXIT_FAILURE);
                }
                else if(numArg == 0){
                    nbColors = atoi(args);
                }
                else{
                    if(numArg > 0 && isABalise(args) != 0){
                        puts("The second and more argument must begin by #");
                        return(EXIT_FAILURE);
                    }
                    else if(numArg > 0 && strlen(args) != 7 ){
                        puts("Not a hexadecimal color");
                        return(EXIT_FAILURE);
                    }
                }

                memset(args, 0, strlen(args));
                numArg++;
            }

            if(nbquotes%2 != 0 && character != '\"'){
                args[arrayPos] = character;
                arrayPos++;
            }
        }

        // -1 parce qu'on termine sur numArg++
        if(nbColors != numArg-1){
            puts("number of balises doesn't match");
            return(EXIT_FAILURE);
        }

        free(params);
        return 0;
    }
    return -1;
}



/* 
    Cette fonction a pour but de vérifier la conformité des balises:
        - Si les paramètres contiennent bien un nombre de balises et X balises
    Renvoie 0 si le test est okay, -1 sinon
*/
int baliseChecking(char* jsonStringify){
    if(isThisCode(jsonStringify, "balises") == 0){

        char *params = getParams(jsonStringify);
        char character;
        int nbquotes = 0;
        int arrayPos;
        char args[32];
        int numArg = 0;

        int nbBalises; 


        for(int i = 0; i< strlen(params); i++){

            character = params[i];

            if(character == '\"' && nbquotes%2 == 0){
                nbquotes ++;
                arrayPos = 0;
            }
            else if(character == '\"' && nbquotes%2 != 0){
                nbquotes ++;

                if(numArg == 0 && (!atoi(args) || atoi(args) < 1)){
                    puts("First argument must be a integer greater or equal to 1");
                    return(EXIT_FAILURE);
                }
                else if(numArg == 0){
                    nbBalises = atoi(args);
                }
                else{
                    if(numArg > 0 && isABalise(args) != 0){
                        puts("The second and more argument must begin by #");
                        return(EXIT_FAILURE);
                    }
                }

                memset(args, 0, strlen(args));
                numArg++;
            }

            if(nbquotes%2 != 0 && character != '\"'){
                args[arrayPos] = character;
                arrayPos++;
            }
        }

        // -1 parce qu'on termine sur numArg++
        if(nbBalises != numArg-1){
            puts("number of balises doesn't match");
            return(EXIT_FAILURE);
        }

        free(params);
        return 0;
    }
    return -1;
}
