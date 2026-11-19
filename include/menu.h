#ifndef MENU_H
#define MENU_H

/*
 * menu.h
 * -------
 * Contrat public du module d'interface en terminal.
 * Il declare les fonctions permettant d'afficher les menus, gerer les saisies
 * de l'utilisateur et lire le contenu d'un fichier de configuration.
 */

/* Affiche le menu principal sans lire de saisie. */
void afficher_menu(void);

/* Affiche le menu, lit les choix et execute les actions principales. */
void gerer_menu(void);

/* Affiche les options du sous-menu whitelist/blacklist. */
void afficher_menu_listes(void);

/* Gere les choix relatifs aux listes d'adresses autorisees ou bloquees. */
void gerer_menu_listes(void);

/* Ouvre un fichier texte en lecture et affiche toutes ses lignes. */
void afficher_fichier(const char *chemin);

#endif