
/*
 * menu.c
 * -------
 * Ce fichier contient toute la partie interactive du programme EDR.
 * Il affiche les menus dans le terminal, lit les choix saisis par l'utilisateur
 * avec scanf(), puis appelle les fonctions correspondant a chaque option.
 *
 * Remarque : ce fichier gere actuellement surtout les parcours du menu.
 * Certaines actions affichent encore un message temporaire, tandis que les
 * lectures de fichiers sont deja implementees par afficher_fichier().
 */

/* stdio.h fournit printf(), scanf(), getchar(), fopen(), fgets() et fclose(). */
#include <stdio.h>

/*
 * stdlib.h fournit system(), utilise pour envoyer la commande "clear"
 * au shell afin d'effacer l'affichage du terminal.
 */
#include <stdlib.h>

/* Declarations des fonctions publiques utilisees par les autres fichiers. */
#include "menu.h"

/*
 * effacer_terminal()
 *
 * Efface le contenu visible du terminal avant d'afficher un nouveau menu.
 * La commande "clear" fonctionne dans les environnements Unix et Linux.
 * Si system() ne peut pas executer la commande, system() renvoie -1 et
 * un message d'erreur est affiche a l'utilisateur.
 */
void effacer_terminal(void)
{
    if (system("clear") == -1) {
        printf("Erreur lors de l'effacement du terminal.\n");
    }
}

/*
 * attendre_entree()
 *
 * Suspend temporairement le programme pour laisser l'utilisateur lire
 * les informations affichees a l'ecran.
 * Le premier getchar() lit l'appui sur Entree qui suit generalement scanf().
 * Le second getchar() attend explicitement une nouvelle ligne.
 * Cette methode simple peut toutefois consommer une entree inattendue si
 * plusieurs caracteres ont ete saisis a la suite.
 */
void attendre_entree(void)
{
    printf("\nAppuyez sur Entree pour continuer...");
    getchar();
    getchar();
}

/*
 * afficher_menu()
 *
 * Affiche uniquement le menu principal et ses principales options.
 * Cette fonction ne lit pas la saisie : elle se contente d'ecrire le texte
 * sur la sortie standard. La lecture du choix est realisee par gerer_menu().
 *
 * Les numeros affiches entre crochets correspondent directement aux valeurs
 * attendues par la structure switch() de gerer_menu().
 */
void afficher_menu(void)
{
    /* Ligne de separation du haut du menu. */
    printf("\n========================================\n");
    printf("                  EDR\n");
    printf("========================================\n\n");

    /* Options disponibles pour l'utilisateur. */
    printf("[1] Demarrer la surveillance\n");
    printf("[2] Afficher les alertes\n");
    printf("[3] Connexions reseau\n");
    printf("[4] Whitelist / Blacklist\n");
    printf("[5] Gestion des regles\n");
    printf("[6] Etat du systeme\n\n");

    /* Option permettant de quitter le menu. */
    printf("[0] Quitter\n\n");
    printf("========================================\n");
}

/*
 * gerer_menu()
 *
 * Point d'entree de l'interface textuelle principale.
 * La boucle do...while reste active tant que quitter vaut 0.
 * Dans chaque tour de boucle, le programme :
 *   1. efface le terminal ;
 *   2. affiche le menu principal ;
 *   3. demande un nombre entier a l'utilisateur ;
 *   4. execute l'action correspondant a ce nombre ;
 *   5. revient au debut du menu.
 *
 * Le mode 0 demande une confirmation avant de quitter.
 * Une saisie qui n'est pas un entier provoque un retour immediat de la boucle.
 */
void gerer_menu(void)
{
    /* Le drapeau reste a 0, car aucune sortie normale n'est implementee ici. */
    int quitter = 0;

    do {
        /* Variable locale recreee a chaque passage de la boucle. */
        int choix;

        /* Preparation de l'ecran avant l'affichage du menu. */
        effacer_terminal();
        afficher_menu();

        printf("Quelle fonctionnalite souhaitez-vous utiliser : ");

        /*
         * scanf() renvoie le nombre de valeurs lues correctement.
         * Tant qu'un entier n'est pas saisi, on annonce une saisie invalide
         * puis on arrête gerer_menu().
         */
        if (scanf("%d", &choix) != 1) {
            printf("Saisie invalide.\n");
            return;
        }

        /*
         * switch() choisit une operation selon la valeur d'entier saisie.
         * Chaque case correspond a une option affichee par afficher_menu().
         */
        switch (choix)
        {
            case 0: {
                char reponse;

                printf("Etes-vous sur ? [Y/N] : ");

                /* %c lit un caractere; l'espace avant %c ignore les blancs restants. */
                if (scanf(" %c", &reponse) != 1) {
                    printf("Saisie invalide.\n");
                    return;
                }

                /* Une reponse positive ferme l'application. */
                if (reponse == 'y' || reponse == 'Y') {
                    effacer_terminal();
                    printf("Fermeture de l'EDR...\n");
                    return;
                }
                /* Une reponse negative annule la fermeture. */
                else if (reponse == 'n' || reponse == 'N') {
                    printf("Fermeture annulee.\n");
                }
                /* Tout autre caractere est refuse. */
                else {
                    printf("Reponse invalide.\n");
                }

                break;
            }

            case 1:
                /* Option reservee au demarrage reel de la surveillance. */
                effacer_terminal();
                printf("Demarrage de l'EDR...\n");
                attendre_entree();
                break;

            case 2:
                /* Option reservee a l'affichage des alertes detectees. */
                effacer_terminal();
                printf("Affichage des alertes en cours...\n");
                attendre_entree();
                break;

            case 3:
                /* Option reservee a la consultation des connexions reseau. */
                effacer_terminal();
                printf("Affichage des connexions reseau...\n");
                attendre_entree();
                break;

            case 4:
                /* Cette option ouvre un sous-menu gerant les listes d'adresses IP. */
                effacer_terminal();
                gerer_menu_listes();
                break;

            case 5:
                /* Option reservee au futur menu de configuration des regles. */
                effacer_terminal();
                printf("Ouverture du menu de gestion des regles...\n");
                attendre_entree();
                break;

            case 6:
                /* Option reservee aux informations generales sur l'etat du systeme. */
                effacer_terminal();
                printf("Affichage de l'etat du systeme...\n");
                attendre_entree();
                break;

            default:
                /* Cette branche traite tous les numeros sans option associee. */
                effacer_terminal();
                printf("Choix invalide.\n");
                break;
        }

        /*
         * quitter vaut toujours 0 dans l'etat actuel, donc chaque option
         * valide ou invalide ramene normalement a l'affichage du menu principal.
         */
    } while (quitter == 0);
}

/*
 * afficher_menu_listes()
 *
 * Affiche les operations disponibles pour la whitelist et la blacklist.
 * Les numeros utilises ici sont traites exclusivement par gerer_menu_listes().
 */
void afficher_menu_listes(void)
{
    /* En-tete du sous-menu. */
    printf("\n========================================\n");
    printf("          WHITELIST / BLACKLIST\n");
    printf("========================================\n\n");

    /* Options de consultation et de modification des listes. */
    printf("[1] Afficher la whitelist\n");
    printf("[2] Afficher la blacklist\n");
    printf("[3] Ajouter une adresse IP\n");
    printf("[4] Supprimer une adresse IP\n\n");

    /* 0 termine ce sous-menu et ramene au menu principal. */
    printf("[0] Retour au menu principal\n\n");
    printf("========================================\n");
}

/*
 * gerer_menu_listes()
 *
 * Gere les choix relatifs aux listes d'adresses autorisees ou bloquees.
 * La boucle continue jusqu'a ce que l'utilisateur saisisse 0.
 * Les options 1 et 2 utilisent afficher_fichier() pour lire les fichiers
 * de configuration. Les options 3 et 4 sont pour l'instant placeholders.
 */
void gerer_menu_listes(void)
{
    /* -1 permet d'entrer dans la boucle meme si aucune saisie n'a encore ete faite. */
    int choix = -1;

    do {
        /* Reinitialise l'affichage avant chaque proposition d'action. */
        effacer_terminal();
        afficher_menu_listes();

        printf("Votre choix : ");

        /* Une saisie non entiere ou incorrecte ferme ce sous-menu proprement. */
        if (scanf("%d", &choix) != 1) {
            printf("Saisie invalide.\n");
            return;
        }

        /* Distribution du choix vers les actions disponibles. */
        switch (choix)
        {
            case 1:
                /* Affiche le contenu du fichier contenant les adresses autorisees. */
                effacer_terminal();
                printf("\n=== WHITELIST ===\n");
                afficher_fichier("config/whitelist.conf");
                attendre_entree();
                break;

            case 2:
                /* Affiche le contenu du fichier contenant les adresses bloquees. */
                effacer_terminal();
                printf("\n=== BLACKLIST ===\n");
                afficher_fichier("config/blacklist.conf");
                attendre_entree();
                break;

            case 3:
                /* Message temporaire : l'ajout reel n'est pas encore implemente ici. */
                effacer_terminal();
                printf("Ajout d'une adresse IP...\n");
                attendre_entree();
                break;

            case 4:
                /* Message temporaire : la suppression reelle reste a implementer. */
                effacer_terminal();
                printf("Suppression d'une adresse IP...\n");
                attendre_entree();
                break;

            case 0:
                /* Le prochain test de boucle detectera 0 et mettra fin au sous-menu. */
                printf("Retour au menu principal...\n");
                break;

            default:
                /* Le nombre ne correspond a aucune option du sous-menu. */
                printf("Choix invalide.\n");
                break;
        }

        /*
         * La condition est reevaluee apres chaque switch.
         * Elle termine la boucle uniquement lorsque choix vaut 0.
         */
    } while (choix != 0);
}

/*
 * afficher_fichier()
 *
 * Ouvre un fichier texte en lecture, affiche son contenu sur la sortie
 * standard ligne par ligne, puis ferme le fichier.
 *
 * chemin est le nom ou le chemin du fichier a ouvrir.
 * Si l'ouverture echoue, une erreur est affichee a l'utilisateur.
 * Une ligne peut contenir au maximum 255 caracteres avec le tampon ligne[256],
 * car la taille reservee doit egalement tenir compte du caractere nul.
 */
void afficher_fichier(const char *chemin)
{
    /* "r" signifie que le fichier est ouvert en lecture seule. */
    FILE *fichier = fopen(chemin, "r");

    /* Tampon temporaire recevant une ligne du fichier a chaque iteration. */
    char ligne[256];

    /* fopen() renvoie NULL si le fichier est introuvable ou inaccessible. */
    if (fichier == NULL) {
        printf("Impossible d'ouvrir le fichier.\n");
        return;
    }

    /*
     * fgets() lit une ligne depuis le fichier et place '\0' en fin de chaine.
     * fgets() retourne NULL lorsque la fin du fichier est atteinte ou en cas d'erreur.
     */
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        /* Affiche exactement la chaine lue, sans reformater son contenu. */
        printf("%s", ligne);
    }

    /* Libere les ressources associees au fichier une fois sa lecture terminee. */
    fclose(fichier);
}
