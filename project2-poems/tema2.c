// Copyright 2019 Alexandru Necula 312CD
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "util/task_helper.h"

#define MAX 10000
#define VERS 200
#define WORD 50

void uppercase(char** poem, int n) {
    for (int i = 0; i < n; i++) {
        poem[i][0] = toupper(poem[i][0]);
        for (int j = 1; poem[i][j] != '\0'; j++) {
              poem[i][j] = tolower(poem[i][j]);
        }
    }
    return;
}

// functie care verifica daca este un caracter special din cele cerute
int special_char(char c) {
    switch (c) {
    case 46:
        return 1;
    case 44:
        return 1;
    case 59:
        return 1;
    case 33:
        return 1;
    case 63:
        return 1;
    case 58:
        return 1;
    }
    return 0;
}

void trimming(char** poem, int n) {
    int k;
    for (int i = 0; i < n; i++) {
        k = 0; // numara cate caractere sunt valide
        for (int j = 0; poem[i][j] != '\0'; j++) {
            if ((poem[i][j] != ' ') || ((j > 0) && (poem[i][j - 1] != ' '))) {
                if (special_char(poem[i][j]) == 0) {
                    poem[i][k] = poem[i][j];
                    k++;
                }
            }
        }
        poem[i][k] = '\0';
    }
    return;
}

// bubble sort pentru un array de stringuri
void sort(char** list, int n) {
    char* temp = (char*)malloc(VERS * sizeof(char));
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (strcmp(list[j], list[j + 1]) > 0) {
                strcpy(temp, list[j]);
                strcpy(list[j], list[j + 1]);
                strcpy(list[j + 1], temp);
            }
        }
    }
    free(temp);
}

/* functia verifica daca exista un sinonim pentru cuvant1
si daca acesta rimeaza cu cuvant2 */
int check_synonym(char* cuvant1, char* cuvant2, int l2, char** synonym) {
    int nr, l;
    char** word_list;
    get_synonym(cuvant1, &nr, &word_list);
    if (&nr != NULL) {
        sort(word_list, nr);
        for (int i = 0; i < nr; i++) {
            l = strlen(word_list[i]) - 1;
            if (cuvant2[l2] == word_list[i][l]) {
                strcpy(*synonym, word_list[i]);
                return 1;
            }
        }
    }
    return 0;
}

// functia salveaza caracterele speciale dintr-un string
void save_special(char* cuvant, char* temp) {
  for (int i = 0; cuvant[i] != '\0'; i++) {
      if (special_char(cuvant[i]) == 1) {
          strncat(temp, cuvant + i, 1);
          cuvant[i] = '\0';
      }
  }
}

// functia inlocuieste cuvintele cu sinonimele care rimeaza
void verse_pair(char** poem, int n, int strofa, int v, int d) {
    char* cuvant1 = (char*)malloc(WORD * sizeof(char));
    if (cuvant1 == NULL) {
      return;
    }
    char* cuvant2 = (char*)malloc(WORD * sizeof(char));
    if (cuvant2 == NULL) {
      free(cuvant1);
      return;
    }
    char* synonym = (char*)malloc(WORD * sizeof(char));
    if (synonym == NULL) {
      free(cuvant1);
      free(cuvant2);
      return;
    }
    char* temp1 = (char*)malloc(WORD * sizeof(char));
    if (temp1 == NULL) {
      free(cuvant1);
      free(cuvant2);
      free(synonym);
      return;
    }
    char* temp2 = (char*)malloc(WORD * sizeof(char));
    if (temp2 == NULL) {
      free(cuvant1);
      free(cuvant2);
      free(synonym);
      free(temp1);
      return;
    }
    char* c1 = strrchr(poem[strofa * 4 + v], ' ') + 1;
    char* c2 = strrchr(poem[strofa * 4 + v + d], ' ') + 1;
    strcpy(cuvant1, c1);
    strcpy(cuvant2, c2);
    int l1, l2;
    temp1[0] = '\0';
    temp2[0] = '\0';
    save_special(cuvant1, temp1);
    save_special(cuvant2, temp2);
    l1 = strlen(cuvant1) - 1;
    l2 = strlen(cuvant2) - 1;
    cuvant1[l1] = tolower(cuvant1[l1]);
    cuvant2[l2] = tolower(cuvant2[l2]);
    int ok = 0;
    if (cuvant1[l1] != cuvant2[l2]) {
        ok = check_synonym(cuvant1, cuvant2, l2, &synonym);
        if (ok == 1) {
            poem[strofa * 4 + v][c1 - poem[strofa * 4 + v]] = '\0';
            strcat(poem[strofa * 4 + v], synonym);
            strcat(poem[strofa * 4 + v], temp1);
        }    else {
            ok = check_synonym(cuvant2, cuvant1, l1, &synonym);
            if (ok == 1) {
                poem[strofa * 4 + v + d][c2 - poem[strofa * 4 + v + d]] = '\0';
                strcat(poem[strofa * 4 + v + d], synonym);
                strcat(poem[strofa * 4 + v + d], temp2);
            }
        }
    }
    free(cuvant1);
    free(cuvant2);
    free(temp1);
    free(temp2);
    free(synonym);
}

/* pentru fiecare tip de rima, stabileste indicii versurilor
 * v1 - indicele versului din prima pereche
 * d1 - diferenta pana la indicele celui de-al doilea vers din prima pereche
 */
void rhimy(char** poem, int n, int type) {
    int v1, v2, d1, d2;
    switch (type) {
    case 1: // imperecheata
        v1 = 0;
        v2 = 2;
        d1 = 1; d2 = 1;
        break;
    case 2: // incrucisata
        v1 = 0;
        v2 = 1;
        d1 = 2; d2 = 2;
        break;
    case 3: // imbratisata
        v1 = 0;
        v2 = 1;
        d1 = 3; d2 = 1;
        break;
    }
    for (int i = 0; i < n / 4; i++) {
        verse_pair(poem, n, i, v1, d1);
        verse_pair(poem, n, i, v2, d2);
    }
}

// functia separa fiecare vers in cuvinte si verifica daca exista diminutiv
void friendly(char** poem, int n) {
    int l, j;
    char* p;
    char* dym = (char*)malloc(WORD * sizeof(char));
    if (dym == NULL) {
      return;
    }
    char* temp = (char*)malloc(VERS * sizeof(char));
    if (temp == NULL) {
      free(dym);
      return;
    }
    char* copy = (char*)malloc(VERS * sizeof(char));
    if (copy == NULL) {
      free(dym);
      free(temp);
      return;
    }
    for (int i = 0; i < n; i++) {
        j = 0; // counterul caraceterelor din sirul initial
        strcpy(temp, "");
        strcpy(copy, poem[i]);
        p = strtok(poem[i], "\n,. ");
        while (p) {
            j += strlen(p);
            get_friendly_word(p, &dym);
            if (dym != NULL) {
                strcat(temp, dym);
            }    else {
                strcat(temp, p);
            }
            l = strlen(temp);
            while (isspace(copy[j]) || special_char(copy[j])) {
                strncat(temp, copy + j, 1);
                l++;
                j++;
            }
            p = strtok(NULL, "\n., ");
        }
        strcpy(poem[i], temp);
    }
    free(dym);
    free(temp);
    free(copy);
}

void silly(char** poem, int n, float p) {
    float sample;
    for (int i = 0; i < n; i++) {
        for (int j = 0; poem[i][j] != '\0'; j++) {
            if (poem[i][j] >= 'a' && poem[i][j] <= 'z') {
                sample = rand() % 100 / 99.0;
                if (p > sample) {
                    poem[i][j] = toupper(poem[i][j]);
                }
            }    else if (poem[i][j] >= 'A' && poem[i][j] <= 'Z') {
                sample = rand() % 100 / 99.0;
                if (p > sample) {
                    poem[i][j] = tolower(poem[i][j]);
                }
            }
        }
    }
}

// printeaza fiecare vers al poeziei, adauga spatiu dupa fiecare catren
void print(char** poem, int n) {
    for (int i = 0; i < n; i++) {
        printf("%s\n", poem[i]);
        if ((i + 1) % 4 == 0) {
            printf("\n");
        }
    }
}

// functia imparte poezia in versuri
int split_vers(char* buffer, char** poem) {
    char* p = strtok(buffer, "\n");
    unsigned int i = 0;
    while (p) {
        poem[i] = (char*)malloc(VERS * sizeof(char));
        if (poem[i] == NULL) {
          for (int j = 0; j < i; j++) {
            free(poem[j]);
          }
          return -1;
        }
        strcpy(poem[i], p);
        p = strtok(NULL, "\n");
        i++;
    }
    return i;
}


int main(void)
{
    srand(42);
    int n; // nr versuri
    float p;
    char cmd[WORD], path[WORD], type[WORD];
    char* buffer = (char*)malloc(MAX * sizeof(char));
    if (buffer == NULL) {
      return -1;
    }
    char** poem = (char**)malloc(VERS * sizeof(char*));
    if (poem == NULL) {
      free(buffer);
      return -1;
    }
    while (1) {
        scanf("%s", cmd);
        if (strcmp(cmd, "load") == 0) {
            scanf("%s", path);
            load_file(path, buffer);
            n = split_vers(buffer, poem);
            if (n < 0) {
              return -1;
            }
            free(buffer);
        }    else if (strcmp(cmd, "uppercase") == 0) {
            uppercase(poem, n);
        }    else if (strcmp(cmd, "trimming") == 0) {
            trimming(poem, n);
        }    else if (strcmp(cmd, "make_it_silly") == 0) {
            scanf("%f", &p);
            silly(poem, n, p);
        }    else if (strcmp(cmd, "make_it_friendlier") == 0) {
            friendly(poem, n);
        }    else if (strcmp(cmd, "make_it_rhyme") == 0) {
            scanf("%s", type);
            if (strcmp(type, "imperecheata") == 0) {
                rhimy(poem, n, 1);
            }    else if (strcmp(type, "incrucisata") == 0) {
                rhimy(poem, n, 2);
            }    else if (strcmp(type, "imbratisata") == 0) {
                rhimy(poem, n, 3);
            }
        }    else if (strcmp(cmd, "print") == 0) {
            print(poem, n);
        }    else if (strcmp(cmd, "quit") == 0) {
            break;
        }
    }
    for (int i = 0; i < n; i++) {
      free(poem[i]);
    }
    free(poem);
    return 0;
}
