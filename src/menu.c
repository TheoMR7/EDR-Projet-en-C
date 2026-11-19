
#include <stdio.h>
#include <stdlib.h>
#include "menu.h"

void effacer_terminal(void)
{
    system("clear");
}

void attendre_entree(void)
{
    printf("\nAppuyez sur Entree pour continuer...");
    getchar();
    getchar();
}

void afficher_menu(void)
{
    printf("\n========================================\n");
    printf("                  EDR\n");
    printf("========================================\n\n");

    printf("[1] Demarrer la surveillance\n");
    printf("[2] Afficher les alertes\n");
    printf("[3] Connexions reseau\n");
    printf("[4] Whitelist / Blacklist\n");
    printf("[5] Gestion des regles\n");
    printf("[6] Etat du systeme\n\n");

    printf("[0] Quitter\n\n");
    printf("========================================\n");
}

void gerer_menu(void)
{
    int quitter = 0;

    do {
        int choix;

        effacer_terminal();
        afficher_menu();

        printf("Quelle fonctionnalite souhaitez-vous utiliser : ");

        if (scanf("%d", &choix) != 1) {
            printf("Saisie invalide.\n");
            return;
        }

        switch (choix)
        {
            case 0: {
                char reponse;

                printf("Etes-vous sur ? [Y/N] : ");
                scanf(" %c", &reponse);

                if (reponse == 'y' || reponse == 'Y') {
                    effacer_terminal();
                    printf("Fermeture de l'EDR...\n");
                    return;
                }
                else if (reponse == 'n' || reponse == 'N') {
                    printf("Fermeture annulee.\n");
                }
                else {
                    printf("Reponse invalide.\n");
                }

                break;
            }

            case 1:
                effacer_terminal();
                printf("Demarrage de l'EDR...\n");
                attendre_entree();
                break;

            case 2:
                effacer_terminal();
                printf("Affichage des alertes en cours...\n");
                attendre_entree();
                break;

            case 3:
                effacer_terminal();
                printf("Affichage des connexions reseau...\n");
                attendre_entree();
                break;

            case 4:
                effacer_terminal();
                gerer_menu_listes();
                break;

            case 5:
                effacer_terminal();
                printf("Ouverture du menu de gestion des regles...\n");
                attendre_entree();
                break;

            case 6:
                effacer_terminal();
                printf("Affichage de l'etat du systeme...\n");
                attendre_entree();
                break;

            default:
                effacer_terminal();
                printf("Choix invalide.\n");
                break;
        }

    } while (quitter == 0);
}

void afficher_menu_listes(void)
{
    printf("\n========================================\n");
    printf("          WHITELIST / BLACKLIST\n");
    printf("========================================\n\n");

    printf("[1] Afficher la whitelist\n");
    printf("[2] Afficher la blacklist\n");
    printf("[3] Ajouter une adresse IP\n");
    printf("[4] Supprimer une adresse IP\n\n");

    printf("[0] Retour au menu principal\n\n");
    printf("========================================\n");
}

void gerer_menu_listes(void)
{
    int choix = -1;

    do {
        effacer_terminal();
        afficher_menu_listes();

        printf("Votre choix : ");

        if (scanf("%d", &choix) != 1) {
            printf("Saisie invalide.\n");
            return;
        }

        switch (choix)
        {
            case 1:
                effacer_terminal();
                printf("\n=== WHITELIST ===\n");
                afficher_fichier("config/whitelist.conf");
                attendre_entree();
                break;

            case 2:
                effacer_terminal();
                printf("\n=== BLACKLIST ===\n");
                afficher_fichier("config/blacklist.conf");
                attendre_entree();
                break;

            case 3:
                effacer_terminal();
                printf("Ajout d'une adresse IP...\n");
                attendre_entree();
                break;

            case 4:
                effacer_terminal();
                printf("Suppression d'une adresse IP...\n");
                attendre_entree();
                break;

            case 0:
                printf("Retour au menu principal...\n");
                break;

            default:
                printf("Choix invalide.\n");
                break;
        }

    } while (choix != 0);
}


void afficher_fichier(const char *chemin)
{
    FILE *fichier = fopen(chemin, "r");
    char ligne[256];

    if (fichier == NULL) {
        printf("Impossible d'ouvrir le fichier.\n");
        return;
    }

    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        printf("%s", ligne);
    }

    fclose(fichier);
}
