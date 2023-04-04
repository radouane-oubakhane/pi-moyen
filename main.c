#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Les variables globale

#define NUM_STATES 2 // nombre d'états dans le problème
#define NUM_ACTIONS 2 // nombre d'actions possibles dans chaque état
#define ALPHA 0.5 // facteur d'actualisation

float U[NUM_STATES]; // Le vecteur gain optimal
float PHI[NUM_STATES];
int strategie[NUM_STATES]={0,1}; // les stratégies

// définir les probabilités de transition et les récompenses pour chaque couple état-action
float T[NUM_STATES][NUM_ACTIONS][NUM_STATES] = {
        {{0.3333, 0.6666}, {0.5, 0.5}},
        {{0.25,   0.75},   {0,   1}}
};
float R[NUM_STATES][NUM_ACTIONS] = {{1, 0},
                                    {3, 2}
};

void findPHIandU() {

    int i, j, k;
    float a[10][10] = {0.0}, x[10] = {0.0};
    float pivot = 0.0;
    float factor = 0.0;
    float sum = 0.0;

    // I
    float I[NUM_STATES][NUM_STATES];
    for (i = 0; i < NUM_STATES; ++i) {
        for (j = 0; j < NUM_STATES; ++j) {
            if (i == j) I[i][j] = 1;
            else I[i][j] = 0;
        }
    }

    // P(f)
    float Pf[NUM_STATES][NUM_STATES];
    for (i = 0; i < NUM_STATES; ++i) {
        for (j = 0; j < NUM_ACTIONS; ++j) {
            Pf[i][j] = T[i][strategie[i]][j];
        }
    }

    // R(f)
    float r[NUM_STATES];
    for (i = 0; i < NUM_STATES; ++i) {
        r[i] = R[i][strategie[i]];
    }



    float temp[NUM_STATES*3][NUM_STATES*3+1];

    for (i = 0; i < NUM_STATES*3; ++i) {
        for (j = 0; j <= NUM_ACTIONS*3; ++j) {
            temp[i][j] = 0;
        }
    }

    // J
    for (i = 0; i < NUM_STATES; ++i) {
        for (j = 0; j < NUM_ACTIONS; ++j) {
            temp[i][j] = I[i][j] - Pf[i][j];
        }
    }
    int l = 0;
    for (i = NUM_STATES; i < (2*NUM_STATES); ++i) {
        temp[i][l++] = 1;
    }

    // U
    for (i = NUM_STATES; i < (2*NUM_STATES); ++i) {
        for (j = NUM_STATES; j < (2*NUM_STATES); ++j) {
            temp[i][j] = I[i-NUM_STATES][j-NUM_STATES] - Pf[i-NUM_STATES][j-NUM_STATES];
        }
        temp[i][3*NUM_STATES] = r[i-NUM_STATES];
    }
    l = NUM_STATES;
    for (i = (2*NUM_STATES); i < (3*NUM_STATES); ++i) {
        temp[i][l++] = 1;
    }

    // Z
    for (i = (2*NUM_STATES); i < (3*NUM_STATES); ++i) {
        for (j = (2*NUM_STATES); j < (3*NUM_STATES); ++j) {
            temp[i][j] = I[i-(2*NUM_STATES)][j-(2*NUM_STATES)] - Pf[i-(2*NUM_STATES)][j-(2*NUM_STATES)];
        }
    }





//==============================================================================================00


    for (i = 0; i < NUM_STATES*3; ++i) {
        for (j = 0; j <= NUM_ACTIONS*3; ++j) {
            printf("%f\t\t",temp[i][j]);
        }
        printf("\n");
    }

//==============================================================================================00

    for (i = 0; i < NUM_STATES*3; ++i) {
        for (j = 0; j <= NUM_ACTIONS*3; ++j) {
            a[i + 1][j + 1] = temp[i][j];
        }
    }

    for (k = 1; k <= NUM_STATES*3 - 1; k++) {
        if (a[k][k] == 0.0) {
            printf("error");
        } else {
            pivot = a[k][k];
            for (j = k; j <= NUM_STATES*3 + 1; j++) a[k][j] = a[k][j] / pivot;
            for (i = k + 1; i <= NUM_STATES*3; i++) {
                factor = a[i][k];
                for (j = k; j <= NUM_STATES*3 + 1; j++) {
                    a[i][j] = a[i][j] - factor * a[k][j];
                }
            }
        }

        if (a[NUM_STATES*3][NUM_STATES*3] == 0) printf("error");
        else {
            x[NUM_STATES*3] = a[NUM_STATES*3][NUM_STATES*3 + 1] / a[NUM_STATES*3][NUM_STATES*3];
            for (i = NUM_STATES*3 - 1; i >= 1; i--) {
                sum = 0.0;
                for (j = i + 1; j <= NUM_STATES*3; j++)
                    sum = sum + a[i][j] * x[j];
                x[i] = (a[i][NUM_STATES*3 + 1] - sum) / a[i][i];
            }
        }
    }
    for (i = 1; i <= NUM_STATES; i++) PHI[i - 1] = x[i];
}

int findA() {
    int i, j, a;
    int ensembleVide;
    int optimale[NUM_STATES][NUM_ACTIONS];

    float val;

    for (i = 0; i < NUM_STATES; i++) {
        for (a = 0; a < NUM_ACTIONS; a++) {
            val = R[i][a];
            for (j = 0; j < NUM_STATES; j++) {
                val += ALPHA * T[i][a][j] * U[j];
            }
            if (val > PHI[i] + U[i]) optimale[i][a] = 0;
            else optimale[i][a] = 1;
        }
    }

    int notOptimale;

    notOptimale = 0;

    for (i = 0; i < NUM_STATES; i++) {
        ensembleVide = 0;
        for (j = 0; j < NUM_ACTIONS; j++) {
            if (optimale[i][j]) ensembleVide = 1;
            else {
                ensembleVide = 0;
                notOptimale = 1;
                break;
            }
        }

        if (!ensembleVide) {
            for (a = 0; a < NUM_ACTIONS; ++a) {
                if (strategie[a] != strategie[i]) strategie[i] = strategie[a];
            }
        }
    }

    return notOptimale;
}


int main() {
    int i, stop = 1;

    /* initialiser toutes les stratégies à la première action
    for (i = 0; i < NUM_STATES; i++) {
        strategie[i] = 0;
    } */

    // initialiser U et PHI
    findPHIandU();

    while (stop) {

        // stop = findA();
        // findPHIandU();

        if (1) break;
    }
/*
    printf("La stratégie optimale:\n");
    for (i = 0; i < NUM_STATES; i++) {
        printf("Etat : %d -- Action : %d\n", i, strategie[i]);
    }

    printf("\nLe vecteur gain optimal: \n");
    for (i = 0; i < NUM_STATES; i++) {
        printf("V[%d] = %.2f\n", i, PHI[i]);
    }

    printf("\nLe vecteur U: \n");
    for (i = 0; i < NUM_STATES; i++) {
        printf("U[%d] = %.2f\n", i, U[i]);
    }
*/
    return 0;
}