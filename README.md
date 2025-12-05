# 🌳 ABRnois - Structure de Données Hybride pour l'Analyse de Fréquence des Mots

## Introduction

Ce projet implémente la structure de données ABRnois en langage C. L'ABRnois est un arbre binaire qui combine deux propriétés clés:

1. Arbre Binaire de Recherche (ABR): Les valeurs (les mots, 'mot') sont ordonnées selon l'ordre lexicographique.

2. Arbre Tournoi : Les priorités (le nombre d'occurrences, 'nb_occ') respectent la propriété de tas, assurant que la priorité d'un noeud est toujours supérieure ou égale à celle de ses enfants.

L'objectif de ce projet est d'utiliser cette structure pour analyser un ou plusieurs corpus de texte et construire une liste ordonnée des mots les plus fréquents.

## Implémentation et Algorithmique

## Structure de Données

Les noeuds de l'arbre stockent à la fois la valeur (le mot) et sa priorité (le compte).

```c
typedef struct _noeud {
    char *mot;      // Clé d'ABR 
    int nb_occ;     // Priorité 
    struct _noeud *fg, *fd;
} Noeud, ABRnois;
```

## Compilation

Le projet est compilé en utilisant gcc.

```bash
gcc -Wall abrnois.c -o abrnois
```

## Syntaxe: 
```bash
./abrnois [OPTIONS] <fichier_frequents> <corpus_1> [<corpus_2> ...].
```
-g permet de créer une représentation sous forme de fichiers .pdf des arbres intermédiaires crées au cours du
    processus de création de la liste des mots les plus fréquents.
-n suivi d’un entier p pour créer uniquement une liste des p premiers mots les plus fréquents.

## Auteur
VEY Ronan
Projet d'algorithmique 2024-2025
