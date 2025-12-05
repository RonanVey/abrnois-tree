
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


void vider_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


typedef struct _noeud{
    char *mot;
    int nb_occ;
    struct _noeud *fg, *fd;
} Noeud, *ABRnois;

typedef struct _cell{
    Noeud *n;
    struct _cell *suivant;
} Cell, *Liste;


Noeud * alloue_noeud(char * mot){
    Noeud *new = (Noeud *)malloc(sizeof(Noeud));
    if (!new)
        return new;
    new->mot = (char *)malloc(strlen(mot) + 1);
    if (!new->mot){
        free(new);
        return NULL;
    }
    strcpy(new->mot, mot);
    new->nb_occ = 1;
    new->fd = NULL;
    new->fg = NULL;
    return new;
}

/*
    Fonction permettant d'écrire le début dun fichier dot
*/
void ecrireDebut(FILE *f) {
    fprintf(f, "digraph arbre {\n");
    fprintf(f, "node [shape=record, height=.1, fontsize=12, fontname=\"Helvetica\"];\n");
    fprintf(f, "edge [tailclip=false, arrowtail=dot, dir=both, fontsize=10];\n");
}

/*
    Fonction d'écriture de l'arbre dans le fichier dot.
*/
void ecrireArbre(FILE *f, ABRnois a) {
    if (a) {
        fprintf(f, "n%p [label=\"<gauche> | <valeur> %s | <propriete> %d |<droit>\"];\n", a, a->mot, a->nb_occ);

        if (a->fg) {
            fprintf(f, "n%p:gauche -> n%p:valeur;\n", a, a->fg);
            ecrireArbre(f, a->fg);
        }

        if (a->fd) {
            fprintf(f, "n%p:droit -> n%p:valeur;\n", a, a->fd);
            ecrireArbre(f, a->fd);
        }
    }
}

/*
    Fin d'écriture du fichier dot
*/
void ecrireFin(FILE *f) {
    fprintf(f, "}\n");
}


/*
    Fonction d'export d'un arbre en fichier pdf
*/
int exporte_arbre(char *nom_pdf, ABRnois A) {
    char nom_dot[256];
    strcpy(nom_dot, "tmp_arbre.dot");

    // 1. Écriture du fichier DOT
    FILE *f = fopen(nom_dot, "w");
    if (!f) {
        fprintf(stderr, "Erreur ouverture fichier DOT");
        return -1;
    }

    ecrireDebut(f);
    ecrireArbre(f, A);
    ecrireFin(f);

    fclose(f);

    // 2. Conversion DOT -> PDF
    char commande[512];
    snprintf(commande, sizeof(commande), "dot -Tpdf %s -o %s", nom_dot, nom_pdf);

    int res = system(commande);
    if (res != 0) {
        fprintf(stderr, "Erreur lors de l'exécution de Graphviz (code %d)\n", res);
        return -2;
    }

    return 0;
}

/*
    Fonction effectuant une rotation gauche
*/
void rotation_gauche(ABRnois * A){
    assert(A);
    Noeud *tmp = (*A)->fd;
    (*A)->fd = tmp->fg;
    tmp->fg = *A;
    *A = tmp;
}


/*
    Fonction effectuant une rotation droite
*/
void rotation_droite(ABRnois * A){
    assert(A);
    Noeud *tmp = (*A)->fg;
    (*A)->fg = tmp->fd;
    tmp->fd = *A;
    *A = tmp;
}


/*
    Fonction qui permet l'insertion d'un mot dans l'arbre
*/
int insert(ABRnois *A, char *mot) {
    // Insertion du nouveau noeud
    if (*A == NULL) {
        Noeud *new = alloue_noeud(mot);
        if (!new)
            return 0;
        *A = new;
        return 1;
    }

    int compare = strcmp((*A)->mot, mot);
    // Si le mot est déjà présent, on incrémente sa priorité.
    if (compare == 0) {
        (*A)->nb_occ++;
        return 2;
    }
    // Si le mot est avant que celui présent dans la racine par rapport à l'ordre lexicographique.
    if (compare < 0)
        return insert(&((*A)->fd), mot);
    // Sinon
    else
        return insert(&((*A)->fg), mot);
}


/*
    Fonction qui permet de replacer un noeud au bon endroit en fonction de sa priorité
*/
void remplacement(ABRnois *A, char *mot){
    if (!A)
    return;
    int compare = strcmp((*A)->mot, mot);
    if (compare == 0)
        return;

    if (compare < 0) {
        remplacement(&((*A)->fd), mot);
        if ((*A)->fd && (*A)->nb_occ < (*A)->fd->nb_occ){
            rotation_gauche(A);
        }
    } else {
        remplacement(&((*A)->fg), mot);
        if ((*A)->fg){
            if ((*A)->fg && (*A)->nb_occ < (*A)->fg->nb_occ){
                rotation_droite(A);
            }
        }
    }
}

/*
    Fonction d'insertion dans un arbre ABRnois
*/
int insert_ABRnois(ABRnois *A, char *mot){
    int retour = insert(A, mot);
    if (retour != 0)
        remplacement(A, mot);
    return retour;
}

/*
    Fonction d'ajout d'un noeud dans une liste triée par ordre lexicographique
*/
void ajouter_liste(Liste *lst, Noeud *n) {

    Liste tmp = malloc(sizeof(Cell));
    if (!tmp) 
        return;

    tmp->n = n;
    tmp->suivant = NULL;

    if (*lst == NULL || strcmp(n->mot, (*lst)->n->mot) < 0) {//si le mot est deja au bon endroit (le prochain est plus grand)
        tmp->suivant = *lst;
        *lst = tmp;
    }

    else { //parcours de la liste pour trouver la bonne position d'insertion
        Liste current = *lst;
        while (current->suivant != NULL && strcmp(n->mot, current->suivant->n->mot) > 0) {
            current = current->suivant;
        }
        tmp->suivant = current->suivant;
        current->suivant = tmp;
    }
}


/*
    Fonction qui permet de faire passer le noeud de la racine de l'arbre jusqu'à une feuille
    pour pouvoir le supprimer.
*/
void descend(ABRnois *A, Liste * l) {
    if (*A == NULL)
        return;
    if ((*A)->fg == NULL && (*A)->fd == NULL) {
        ajouter_liste(l, *A);
        *A = NULL;
        return;
    }
    if ((*A)->fg == NULL && (*A)->fd != NULL) {  
        rotation_gauche(A);
        descend(&(*A)->fg,l); 
    }
    else if ((*A)->fg != NULL && (*A)->fd == NULL) {  
        rotation_droite(A); 
        descend(&(*A)->fd,l);
    }
    else if ((*A)->fg != NULL && (*A)->fd != NULL) {  
        if ((*A)->fg->nb_occ >= (*A)->fd->nb_occ) {
            rotation_droite(A);
            descend(&(*A)->fd,l);
        } else {
            rotation_gauche(A);
            descend(&(*A)->fg,l);
        }
    }
}


int extrait_priorite_max(ABRnois * A, Liste * lst){
    if (!A)
        return 0;
    int compteur = 0;
    do {
        descend(A, lst);
        compteur++;
    }while(*A && (*A)->nb_occ == (*lst)->n->nb_occ);
    return compteur;
}



int main(int argc, char *argv[]) {
    int choix = 0; // Vaut 0 si il n'y a ni -g ni -n, 1 si il y a -g, 2 si uniquement -n, 3 si il y a -g et -n
    int p;
    int compteur_insert = 1;
    char nom_file__pdf[100];
    for (int i = 0; i < argc; i++){
        if (strcmp("-n", argv[i]) == 0)
            choix += 2;
        if (strcmp("-g", argv[i]) == 0)
            choix++;
    }
    if ((argc - choix) >= 3){
        ABRnois A = NULL;
        int compteur = 0;
        for (int i = 1; i < argc; i++){
            if (strcmp("-n", argv[i]) == 0){
                i++;
                p = atoi(argv[i]);
            } else if (strcmp("-g", argv[i]) != 0 && (strcmp("frequents.txt", argv[i]) != 0)){
                FILE *f = fopen(argv[i], "r");
                if (!f)
                    return 1;
                char mot[100];
                while((fscanf(f, "%99s", mot)) == 1){
                    compteur++;
                    insert_ABRnois(&A, mot);
                    // Création de tous les fichiers pdf qui représente l'insertion à chaque étape si -g est présent dans la ligne de commande.
                    if (choix == 1 || choix == 3){
                        sprintf(nom_file__pdf, "insertion_%d.pdf", compteur_insert);
                        exporte_arbre(nom_file__pdf, A);
                        compteur_insert++;
                    }
                }
                fclose(f);
            }
        }
        FILE *frequents = fopen(argv[1], "w");
        int compteur_suppr = 1;
        int suppr = 0;
        if (!frequents)
            return 1;
        while(A){
            Liste lst = NULL;
            extrait_priorite_max(&A, &lst);
            if (choix == 1 || choix == 3){
                sprintf(nom_file__pdf, "suppression_%d.pdf", compteur_suppr);
                exporte_arbre(nom_file__pdf, A);
                compteur_suppr++;
            }
            for (;lst != NULL; lst = lst->suivant){
                if (choix == 2 || choix == 3){
                    if (suppr >= p)
                        break;
                }
                fprintf(frequents, "%s %.2f%%\n", lst->n->mot, (100 * lst->n->nb_occ) / (float)compteur);
                suppr++;
            }
        }
        fclose(frequents);
    }
    return 0;
}
