
/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/* 
 * L'objectif principal de ce code est d'effectuer des tests unitaires et
 *  fonctionnels sur les différents composants créés dans ce projet.
 */


#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <assert.h>
#include "validateur.h"


// Test pour savoir si les JSON sont correctements envoyées au serveur 
void test_verification_JSON(){

    // VERIFICATION DES GUILLEMETS

    char* jsonStringify_1 = "{\"code\":\"message\",\"valeurs\":[\"Bonjour\"]}";
    char* jsonStringify_2 = "{\"code\": message\", \"valeurs\": [\"Bonjour\"]}";

    assert(quotesChecking(jsonStringify_1) == 0);
    assert(quotesChecking(jsonStringify_2) == -1);


    // VERIFICATION DES OPERATIONS

    char* jsonStringify_3 = "{\"code\":\"calcul\",\"valeurs\":[\"+\",3,10]}";
    char* jsonStringify_4 = "{\"code\":\"calcul\",\"valeurs\":[\"3\",3,10]}";
    char* jsonStringify_5 = "{\"code\":\"calcul\",\"valeurs\":[\"-\",\"aa\",10]}";

    assert(operationChecking(jsonStringify_3) == 0);
    assert(operationChecking(jsonStringify_4) == -1);
    assert(operationChecking(jsonStringify_5) == -1);

    
    // VERIFICATION DES COULEURS

    char* jsonStringify_6 = "{\"code\":\"color\",\"valeurs\":[2,\"#aaaaaa\",\"#bbbbbb\"]}";
    char* jsonStringify_7 = "{\"code\":\"color\",\"valeurs\":[2,\"#aaaa\",\"#bbbb\"]}";
    char* jsonStringify_8 = "{\"code\":\"color\",\"valeurs\":[1,\"#aaaa\",\"#bbbb\"]}";


    assert(colorChecking(jsonStringify_6) == 0);
    assert(colorChecking(jsonStringify_7) == -1);
    assert(colorChecking(jsonStringify_8) == -1);


    // VERIFICATION DES COULEURS

    char* jsonStringify_9 = "{\"code\":\"balises\",\"valeurs\":[2,\"#aaaaaa\",\"#bbbbbb\"]}";
    char* jsonStringify_10 = "{\"code\":\"balises\",\"valeurs\":[2,\"#aaaa\",\"#bbbb\"]}";
    char* jsonStringify_11 = "{\"code\":\"balises\",\"valeurs\":[1,\"#aaaa\",\"#bbbb\"]}";
    char* jsonStringify_12 = "{\"code\":\"test\",\"valeurs\":[1,\"#aaaa\",\"#bbbb\"]}";

    assert(baliseChecking(jsonStringify_9) == 0);
    assert(baliseChecking(jsonStringify_10) == 0);
    assert(baliseChecking(jsonStringify_11) == -1);
    assert(baliseChecking(jsonStringify_12) == -1);

}

int main(){

    test_verification_JSON();
    puts("Tous les tests ont été réalisés et on été réussis");

    return 0;
}