# 🛡️ Mini EDR / NIDS en C

### Système de surveillance réseau et de détection d'activités suspectes sous Linux

Projet académique développé dans le cadre de la formation **ESGI – Systèmes & Réseaux**.

---

## 📖 Présentation

Ce projet consiste à développer un **EDR (Endpoint Detection and Response) / NIDS (Network Intrusion Detection System)** en langage C, fonctionnant sous Linux.

L'objectif est de concevoir un outil de surveillance capable d'analyser le trafic réseau, d'identifier des comportements suspects et de générer des alertes à partir de règles de détection.

Le projet repose sur une architecture modulaire afin de séparer la capture réseau, l'analyse des paquets, la gestion des connexions, le moteur de règles et l'interface utilisateur.

**Statut :** 🚧 En cours de développement

## 🎯 Objectifs du projet

- Capturer et analyser les paquets réseau.
- Identifier les adresses IPv4, les protocoles et les ports.
- Suivre les connexions et les communications réseau.
- Détecter des activités suspectes à partir de règles.
- Gérer des listes d'adresses IP autorisées ou bloquées.
- Générer et enregistrer des alertes.
- Proposer une interface en terminal pour accéder aux fonctionnalités.
- Permettre l'ajout de modules de détection.

## ⚙️ Technologies utilisées

| Technologie | Utilisation |
|---|---|
| C | Développement des modules |
| Linux | Environnement d'exécution |
| GCC | Compilation |
| Makefile | Automatisation de la compilation |
| Git / GitHub | Gestion des versions et collaboration |
| JSON Lines | Journalisation des alertes |
| Sockets Linux | Capture et analyse du trafic réseau |

Le développement et les tests peuvent également être réalisés sous Windows à l'aide de **WSL Ubuntu**.

## 📁 Architecture du projet

```text
EDR-Projet-en-C/
│
├── include/
│   ├── alert.h
│   ├── capture.h
│   ├── menu.h
│   ├── packet.h
│   ├── rules_engine.h
│   └── state_table.h
│
├── src/
│   ├── alert.c
│   ├── capture.c
│   ├── menu.c
│   ├── rules_engine.c
│   └── state_table.c
│
├── config/
│   ├── blacklist.conf
│   ├── whitelist.conf
│   └── rules.conf
│
├── plugins/
│   └── rules_dot10.c
│
├── tests/
│   ├── test_alert.c
│   └── test_menu.c
│
├── prototype.c
├── Makefile
└── README.md
```

## 🔍 Modules du projet

### 1. Capture réseau

**Fichiers :** `src/capture.c`, `include/capture.h`, `include/packet.h`

Ce module est destiné à récupérer les paquets circulant sur le réseau et à extraire les informations nécessaires à leur analyse.

Les informations étudiées comprennent notamment :

- Les adresses IPv4 source et destination.
- Les protocoles réseau.
- Les ports source et destination.
- Les informations contenues dans les paquets.

Un prototype indépendant permet de tester certaines fonctions de capture et d'analyse.

### 2. Gestion des connexions

**Fichiers :** `src/state_table.c`, `include/state_table.h`

Ce module a pour objectif de conserver des informations sur les communications réseau afin de faciliter leur suivi et leur analyse.

Il comprend notamment des fonctions de manipulation et de conversion des adresses IPv4.

La gestion complète de l'état des connexions fait partie des fonctionnalités à développer et à intégrer.

### 3. Moteur de règles

**Fichiers :** `src/rules_engine.c`, `include/rules_engine.h`

Le moteur de règles doit permettre d'analyser les informations issues de la capture et de déterminer si une activité nécessite une alerte.

Il est destiné à prendre en compte les règles de détection et les listes d'adresses IP configurées.

Les fonctionnalités de détection sont en cours de développement et d'intégration.

### 4. Gestion des alertes

**Fichiers :** `src/alert.c`, `include/alert.h`

Le module d'alertes permet de représenter, afficher et enregistrer les événements détectés.

Les alertes peuvent être affichées dans le terminal et enregistrées dans un journal `alerts.json`.

Le format utilisé est **JSON Lines** : chaque ligne correspond à un objet JSON indépendant.

Ce format facilite la consultation et le traitement ultérieur des événements.

### 5. Interface utilisateur en terminal

**Fichiers :** `src/menu.c`, `include/menu.h`

Une interface interactive permet de naviguer entre les différentes fonctionnalités de l'EDR.

```text
========================================
                  EDR
========================================

[1] Demarrer la surveillance
[2] Afficher les alertes
[3] Connexions reseau
[4] Whitelist / Blacklist
[5] Gestion des regles
[6] Etat du systeme

[0] Quitter

========================================
```

L'interface comprend :

- Un système de navigation par choix numériques.
- Un sous-menu Whitelist / Blacklist.
- Une confirmation avant la fermeture du programme.
- L'effacement du terminal lors des changements d'écran.
- L'affichage du contenu des fichiers de configuration.

Certaines options sont actuellement des interfaces de démonstration et doivent encore être reliées aux modules correspondants.

### 6. Whitelist et Blacklist

**Fichiers :** `config/whitelist.conf`, `config/blacklist.conf`

Ces fichiers permettent de conserver des adresses IP associées à des identifiants ou commentaires.

**Whitelist :** liste des adresses IP autorisées ou considérées comme fiables selon les règles définies.

**Blacklist :** liste des adresses IP à surveiller ou à bloquer selon les mécanismes de détection.

Le sous-menu permet actuellement de consulter les deux fichiers directement depuis le terminal.

L'ajout et la suppression d'adresses IP depuis l'interface restent à implémenter.

## 🚀 Installation et compilation

### Prérequis

- Une distribution Linux (Ubuntu, Debian, etc.).
- GCC.
- Make.
- Git.

Sur Ubuntu ou Debian :

```bash
sudo apt update
sudo apt install build-essential git
```

### Récupérer le projet

```bash
git clone https://github.com/TheoMR7/EDR-Projet-en-C.git
cd EDR-Projet-en-C
```

### Compiler les modules

```bash
make
```

Le Makefile utilise notamment les options :

```bash
-Wall -Wextra -Werror
```

Ces options activent les avertissements du compilateur et traitent les avertissements comme des erreurs.

**Remarque :** la compilation des modules ne correspond pas encore nécessairement à la génération d'un exécutable EDR final entièrement intégré.

## 🧪 Tests

### Tests prévus par le Makefile

```bash
make test
```

La cible de test permet notamment de vérifier le fonctionnement du module d'alertes à l'aide d'une alerte simulée.

Les événements de test sont affichés dans le terminal et ajoutés au journal `alerts.json`.

### Tester l'interface du menu

Depuis la racine du projet :

```bash
gcc -Wall -Wextra -Werror -I./include src/menu.c tests/test_menu.c -o test_menu
```

Puis lancer :

```bash
./test_menu
```

Ce test permet de vérifier la navigation dans le menu principal, l'accès au sous-menu Whitelist / Blacklist et la lecture des fichiers de configuration.

### Prototype de capture réseau

Le fichier `prototype.c` constitue un programme de démonstration indépendant destiné à expérimenter les fonctionnalités de capture et d'analyse réseau.

Il ne correspond pas encore à l'application finale intégrée.

La capture de paquets bruts sous Linux peut nécessiter des privilèges particuliers, notamment `CAP_NET_RAW`.

## 📊 État d'avancement

| Fonctionnalité | État |
|---|---|
| Structure modulaire du projet | ✅ En place |
| Interface principale en terminal | ✅ Fonctionnelle |
| Navigation Whitelist / Blacklist | ✅ Fonctionnelle |
| Lecture des fichiers de listes | ✅ Fonctionnelle |
| Confirmation de fermeture | ✅ Fonctionnelle |
| Tests indépendants du menu | ✅ Fonctionnels |
| Affichage et journalisation des alertes | ✅ Testables |
| Prototype de capture réseau | 🟡 En développement |
| Suivi complet des connexions | 🟡 En développement |
| Moteur de règles | 🟡 En développement |
| Ajout et suppression des IP depuis le menu | ⏳ À implémenter |
| Intégration des modules | ⏳ À finaliser |
| Programme EDR final | ⏳ À finaliser |

## 🔮 Évolutions prévues

Les prochaines étapes du projet comprennent :

1. Finaliser les modules de capture et d'analyse réseau.
2. Compléter le moteur de règles et les mécanismes de détection.
3. Ajouter la modification des listes d'adresses IP depuis le terminal.
4. Relier les différentes fonctionnalités au menu principal.
5. Améliorer le suivi des connexions.
6. Intégrer les alertes et la journalisation au fonctionnement global.
7. Réaliser des tests d'intégration et de stabilité.

## 🔐 Cadre d'utilisation

Ce projet est développé à des fins **pédagogiques, de recherche et d'expérimentation en cybersécurité**.

La capture et l'analyse de trafic doivent être effectuées uniquement sur des réseaux et systèmes pour lesquels une autorisation a été obtenue.

---

**ESGI — Projet Mini EDR / NIDS en C**

*Projet académique en cours de développement.*