// Copyright 2019 Alexandru Necula 312CD

#include <stdio.h>
#include <math.h>

#define TRUE 1
#define FALSE 0
#define NMAX 500

/* citim fiecare byte al adresei IP */
void citire_IP(int *ip) {
  scanf("%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
}

/* citim fiecare byte al adresei IP + masca*/
void citire_IP_set(int *ip) {
  scanf("%d.%d.%d.%d/%d", &ip[0], &ip[1], &ip[2], &ip[3], &ip[4]);
}

/* va afisa in format IP un vector intreg cu fiecare byte al unui IP*/
void afisare_IP(int *ip) {
  printf("%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
}

/* face conversia in alta baza, byte cu byte, a unei adrese IP si o afiseaza */
void conversie(int *adresa_ip, int baza) {
  int v[10], j, c, aux;
  for (j = 0; j <= 3; j++) {
    c = 0;
    if (adresa_ip[j] == 0) printf("0");
    aux = adresa_ip[j];
    while (aux) {
      v[c] = aux%baza;
      aux = aux/baza;
      c++;
    }
    for (int i = c-1; i >= 0; i--) {
      if (v[i] <= 9) {
        printf("%d", v[i]);
      } else {
        switch (v[i]) {
          case 10:
            printf("A");
            break;
          case 11:
            printf("B");
            break;
          case 12:
            printf("C");
            break;
          case 13:
            printf("D");
            break;
          case 14:
            printf("E");
            break;
          case 15:
            printf("F");
            break;
        }
      }
    }
    if (j != 3) {
      printf(".");
    }
  }
}

/* calculeaza adresa de retea byte cu byte */
void adresa_retea(int *retea, int *IP, int *masca) {
  for (int i = 0; i <= 3; i++) {
    retea[i] = IP[i] & masca[i];
  }
}

/* calculeaza adresa de broadcast byte cu byte */
void adresa_broadcast(int *retea, int *IP, int *masca) {
  for (int i = 0; i <= 3; i++) {
    retea[i] = (IP[i] | (~masca[i])) + 256;
  }
}

/* creeaza masca in format complet dintr-un prefix */
void creare_masca(int masca_prefix, int *masca) {
  int i, j;
  for (i = 0; i <= 3; i++) {
    masca[i] = 0;
  }
  for (i = 1; i <=3 ; i++) {
    masca[i-1] = 255;
    if (masca_prefix < i * 8) {
      for (j = 7-masca_prefix % 8; j >= 0; j--) {
        masca[i-1] -= pow(2, j); // din valoarea maxima 255 scadem bitii de 0
      }
      break;
    }
  }
}

/* face conversia in format binar a unei adrese IP in format intreg */
void conversie_binar(int *adresa_ip, int *adresa_ip_binar) {
  int aux, c;
  for (int i = 0; i <= 3; i++) {
    adresa_ip_binar[i] = 0;
    c = 0;
    aux = adresa_ip[i];
    while (aux) {
      if (aux % 2 == 1) {
        adresa_ip_binar[i] = adresa_ip_binar[i] + pow(10, c);
      }
      c++;
      aux = aux / 2;
    }
  }
}

/* face conversia intr-un string a unei adrese IP intregi in format binar */
void conversie_char(int adresa_ip_binar, char byte[10]) {
  for (int i = 7; i >= 0; i--) {
    if (adresa_ip_binar % 10 == 1) {
      byte[i] = '1';
    } else {
      byte[i] = '0';
    }
    adresa_ip_binar = adresa_ip_binar / 10;
  }
}

/* afiseaza un byte in format char */
void afisare_char(char byte[10]) {
  for (int i = 0; i <= 7; i++) {
    printf("%c", byte[i]);
  }
}

/* verifica si corecteaza daca o masca in format char este valida */
int verificare_masca(char byte[20]) {
  int bit_semnificativ, i, ok = 0;
  for (i = 0; i <= 7 ; i++) {
    if (byte[i] == '0') ok = 1; // verificam daca byte-ul contine 0
  }
  if (ok == 0) return 1; // 11111111 este o masca valida
  if (ok == 1) {
    for (i = 0; i <= 7; i++) {
      if (byte[i] == '0') {
        bit_semnificativ = i; // aflam primul bit de 0
        break;
      }
    }
    for (i = bit_semnificativ; i <= 7; i++) {
      if (byte[i] != '0') {
      ok = 0;
      byte[i] = '0'; // cream o masca valida
      }
    }
  }
  if (ok == 0) {
    return 0;
  } else {
    return 1;
  }
}

/* facem conversia in baza 10 a unui byte in format char */
void conversie_10(char byte[10], int *adresa_ip, int counter) {
  adresa_ip[counter] = 0;
  for (int i = 0; i <= 7; i++) {
    if (byte[i] == '1') {
      adresa_ip[counter] = adresa_ip[counter] + pow(2, 7-i);
    }
  }
}

int main()
{ int counter_date, ok, seturi_date, N;
  int IP_1[4], IP_2[4], MSK_1[4], MSK_2, MSK_2_complet[4], temp1[4], temp2[4];
  int adresa[NMAX][5], i, j, adresa_retea_2[4];
  char temp_b0[10], temp_b1[10], temp_b2[10], temp_b3[10];

  scanf("%d", &seturi_date);
  for (counter_date = 1; counter_date <= seturi_date; counter_date++) {
    citire_IP(MSK_1);
    scanf("%d", &MSK_2);
    citire_IP(IP_1);
    citire_IP(IP_2);
    scanf("%d", &N);
    for (j=0; j < N; j++) {
      citire_IP_set(adresa[j]);
    }

    printf("%d \n", counter_date);

    printf("-0 ");
    afisare_IP(IP_1);
    printf("/%d \n", MSK_2);

    printf("-1 ");
    creare_masca(MSK_2, MSK_2_complet);
    afisare_IP(MSK_2_complet);
    printf("\n");

    printf("-2 ");
    conversie(MSK_1, 8);
    printf(" ");
    conversie(MSK_1, 16);
    printf("\n");

    printf("-3 ");
    adresa_retea(temp1, IP_1, MSK_2_complet);
    afisare_IP(temp1);
    printf("\n");

    printf("-4 ");
    adresa_broadcast(temp1, IP_1, MSK_2_complet);
    afisare_IP(temp1);
    printf("\n");

    printf("-5 ");
    ok = 1;
    adresa_retea(temp1, IP_1, MSK_2_complet);
    adresa_retea(temp2, IP_2, MSK_2_complet);
    for (i = 0; i <= 2; i++) {
      if (temp1[i] != temp2[i]) {
        printf("NU \n");
        ok = 0;
        break;
      }
    }
    if (ok == 1) printf("DA \n");

    printf("-6 ");
    ok = 0;
    conversie_binar(MSK_1, temp1);
    conversie_char(temp1[0], temp_b0);
    conversie_char(temp1[1], temp_b1);
    conversie_char(temp1[2], temp_b2);
    conversie_char(temp1[3], temp_b3);
    if (verificare_masca(temp_b3) == 1) {
      if (verificare_masca(temp_b2) == 1) {
        if (verificare_masca(temp_b1) == 1) {
          if (verificare_masca(temp_b0) == 1) {
            printf("DA");
            ok = 1;
          }
        }
      }
    }
    if (ok == 0) printf("NU");
    printf("\n");

    printf("-7 ");
    if (ok == 1) {
      afisare_IP(MSK_1);
    } else {
      conversie_10(temp_b0, temp1, 0);
      conversie_10(temp_b1, temp1, 1);
      conversie_10(temp_b2, temp1, 2);
      conversie_10(temp_b3, temp1, 3);
      afisare_IP(temp1);
    }
    printf("\n");

    printf("-8 ");
    conversie_binar(IP_1, temp1);
    conversie_char(temp1[0], temp_b0);
    conversie_char(temp1[1], temp_b1);
    conversie_char(temp1[2], temp_b2);
    conversie_char(temp1[3], temp_b3);
    afisare_char(temp_b0);
    printf(".");
    afisare_char(temp_b1);
    printf(".");
    afisare_char(temp_b2);
    printf(".");
    afisare_char(temp_b3);
    printf("\n");

    printf("-9 ");
    for (j = 0; j < N; j++) {
      ok = 1;
      creare_masca(adresa[j][4], temp1);
      adresa_retea(adresa_retea_2, IP_2, temp1);
      adresa_retea(temp2, adresa[j], temp1);
      for (i = 0; i <= 2; i++) {
        if (temp2[i] != adresa_retea_2[i]) {
          ok = 0;
          break;
        }
      }
      if (ok == 1) {
        printf("%d ", j);
      }
    }
    printf("\n");
  }
  return 0;
}
