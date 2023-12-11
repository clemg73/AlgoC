# Bibliothèques
<string.h>
<stdio.h>
<ctype.h>
<stdlib.h>
<unistd.h>
<sys/types.h>
<sys/socket.h>
<netinet/in.h>
<signal.h>
<math.h>

# Références
* https://stackoverflow.com/
* https://chat.openai.com/

# Difficulté
* Gérer plusieurs clients à la fois
* Encore un peu de difficulté entre les pointeurs et adresses

# Commentaires
* Le TP est complet et aborde les différents points importants 
* Sans avoir vu les spécificité du C avant de faire le projet, c'est difficile de s'y plonger

# Utilisation
* Dans le terminal 1, dans le dossier src du projet, lancer les commandes "make" et "./serveur"
* Dans le terminal 2, dans le dossier src du projet, lancer l'une des commandes si dessous:
*       - "./client -nom"
*           --> Renvoie le nom de la machine
*       - "./client -message" puis écrire le message
*           --> Renvoie le message
*       - ./client -calcul[avg, min, max, e-t, -, +, *, /] [liste de nombre (pour les opération, 2 nombres seulement)]
*           --> Renvoie la valeur calculé
*       - ./client -color 2 FFFFFF 000000
*           --> Enregistre les couleurs dans un fichier appelé colors_nomMachine.txt
*       - ./client -balises 2 balise1 balise2
*           --> Enregistre les balises dans un fichier appelé balises_nomMachine.txt
* Pour arrêter un client, faire ctrll+c ou control+c (mac)

# analyse des logs du serveur
* "Create new socket" --> Notifie qu'un nouveau client se connecte et que l'on enregistre son socket
* "Sockets list after ..." --> Affiche la liste des sockets avec le nom de machine associés, après traitement de la requete
* "Treatment of..." --> Affiche la requête que le serveur reçois et le numéro de socket du client émetteur
* "Json sended" --> Affiche le json renvoyer au client
* "-------------" --> sépare les traitements
* "Close socket..." --> Notifie que le socket x a été fermé

# Lancement des testes
* Lancer le serveur avec ./serveur puis lancer les testes avec ./testes
* A la fin des testes, vérifier si "Tous les tests ont été réalisés et on été réussis" est noté
