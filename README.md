# Mini EDR / NIDS en C

Projet ESGI en cours de developpement sous Linux.

## Compiler et essayer

Depuis la racine du projet :

```sh
make
make test
```

`make` compile tous les modules avec `-Wall -Wextra -Werror`.
Il ne cree pas encore le programme EDR final : il manque un `main` dans `src/`.

`make test` lance une demonstration : une fausse alerte est affichee dans le
terminal et ajoutee dans `alerts.json`. Chaque lancement ajoute une ligne.
Ce journal utilise JSON Lines : chaque ligne est un objet JSON independant.

## Module d'alertes de Hugo

- `include/alert.h` : les informations regroupees dans une alerte.
- `src/alert.c` : afficher une alerte et l'enregistrer dans le journal.
- `tests/test_alert.c` : creer une fausse alerte pour essayer ces fonctions.

Les fonctions recoivent une copie de l'alerte : `alert_print(alert)`.
On accede donc aux champs avec un point, par exemple `alert.message`.
Les textes doivent tenir dans les tableaux et se terminer par `\0`.

La date est conservee avec `time_t`. `localtime` et `strftime` permettent de
l'afficher sous une forme lisible. Le journal garde la date sous forme numerique.

La fonction interne `json_write_string` gere les caracteres speciaux du JSON.
Elle parcourt le texte avec une boucle et des indices comme `text[i]`.
`ferror` et `fclose` servent a verifier que l'enregistrement a reussi.

## Etat des autres modules

- `packet.h` definit les informations d'un paquet ; la capture reste a ecrire.
- `state_table.c` implemente les conversions d'adresses IPv4.
- Les fonctions d'arbre d'IP et de table des connexions sont declarees dans
  `state_table.h`, mais restent a implementer.
- `blacklist.conf` et `whitelist.conf` contiennent des IP et leurs commentaires.
- `rules.conf` est vide : le moteur de regles reste a ecrire.

Syslog, la detection et le tableau de bord restent egalement a developper.
