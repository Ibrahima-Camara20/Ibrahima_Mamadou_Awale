# Projet Awale - IA Game Programming

Implémentation d'une IA compétitive pour le jeu d'Awale utilisant l'algorithme Alpha-Beta avec évaluation heuristique avancée et recherche itérative en profondeur.

## Description

Ce projet implémente un bot joueur d'Awale (variante à 16 trous avec graines colorées) capable de rivaliser en tournoi. L'IA utilise:

- **Minimax avec élagage Alpha-Beta** pour l'exploration de l'arbre de jeu
- **Table de transposition** avec hachage Zobrist pour optimiser la recherche
- **Évaluation heuristique dynamique** adaptée aux différentes phases de jeu (ouverture, milieu, finale)
- **Iterative Deepening** pour optimiser l'utilisation du temps

## Technologies Utilisées

### Langages

- **C++17** - Implémentation du bot et de la logique de jeu
- **Java** - Arbitre de tournoi et gestion des matchs

### Outils de Build

- **GNU Make** - Compilation cross-platform (Linux/Windows)
- **g++** - Compilateur C++ avec optimisations (-O3)

### Architecture

- **Alpha-Beta Pruning** - Algorithme de recherche d'adversaire
- **Zobrist Hashing** - Système de hachage pour la table de transposition
- **Évaluation Multi-phase** - Poids dynamiques selon l'état de la partie

## Compilation et Exécution

### Linux/WSL

```bash
cd Arene/

# Compilation
make clean
make

# Configuration de l'arbitre
# Éditer Arbitre.java pour définir les deux joueurs
# Ligne 9-10: Process A = ... et Process B = ...

# Lancement du tournoi
javac Arbitre.java
java Arbitre
```
## Format des Coups

- **Format utilisateur**: `1-16` (numéros de trous)
- **Types de coups**:
  - `R` - Rouge seul
  - `B` - Bleu seul
  - `TR` - Transparent + Rouge
  - `TB` - Transparent + Bleu
- **Exemples**: `1R`, `5TR`, `12B`, `16TB`

## Protocole de Communication

- **START**: Début de partie
- **RESULT \<coup\> \<scoreJ1\> \<scoreJ2\>**: Fin de partie avec dernier coup et scores

## Auteurs

**Mamadou Ougailou Diallo** & **Ibrahima Camara**  
Supervisé par **Jean-Charles Régin**  
M1 Informatique - Université Côte d'Azur
