/*
 * test_menu.c
 * -----------
 * Programme autonome permettant de lancer manuellement l'interface principale.
 * Il ne teste pas les fonctions automatiquement: il demarre le menu afin qu'une
 * personne puisse verifier la navigation et les affichages dans le terminal.
 */

#include "menu.h"

int main(void)
{
    /* Demarre l'interface interactive complete. */
    gerer_menu();
    /* Le menu a ete ferme normalement. */
    return 0;
}