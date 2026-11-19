/*
 * main.c
 * ------
 * Point d'entree principal de l'application EDR.
 * Ce fichier initialise l'interface utilisateur en appelant gerer_menu().
 * La logique des modules est volontairement separee dans les autres fichiers
 * afin que le demarrage, l'analyse et l'affichage restent facilement lisibles.
 */

/* Fournit la declaration de la fonction gerer_menu(). */
#include "menu.h"

int main(void)
{
    /* Ouvre la boucle interactive du menu principal. */
    gerer_menu();

    /* Code de retour conventionnel indiquant une fin normale. */
    return 0;
}
