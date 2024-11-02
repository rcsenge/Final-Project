#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debugmalloc.h"
#define MAX_NEV_HOSSZ 100
#define MAX_HELY_HOSSZ 100
#define MAX_MEGJEGYZES_HOSSZ 300
#define DATUMHOSSZ 15 //itt valami nagyon nem jo
#define IDOHOSSZ 6

typedef struct Datum {
    int ev, ho, nap;
} Datum;

typedef struct Ido {
    int ora, perc;
} Ido;

typedef struct Esemeny {
    char *nev;
    Datum datum;
    Ido ido;
    char *hely;
    char *megjegyzes;
} Esemeny;

typedef struct Adatbazis {
    Esemeny *esemenyek;
    int meret;
} Adatbazis;


void menu(Adatbazis *adatbazis);

void rekord_hozzaadasa(Adatbazis *adatbazis);

void bemenet_tisztitasa();

void uj_karaktersor_tisztitasa(char *bemenet);

void substring(char eredeti[], char *cel, int kezdo, int veg);

bool szokoev(int ev);

bool datum_ellenorzo(char datum[]);

bool ido_ellenorzo(char ido[]);

void uj_nev(Esemeny *esemeny);

void uj_datum(Esemeny *esemeny);

void uj_ido(Esemeny *esemeny);

void uj_hely(Esemeny *esemeny);

void uj_megjegyzes(Esemeny *esemeny);

void rekord_kiirasa(Esemeny esemeny);

bool rekord_keresese(Adatbazis adatbazis, Esemeny **talalatok);

void rekord_torlese(Adatbazis *adatbazis);

void rekord_modositasa(Adatbazis *adatbazis);

void adatbazis_kilistazasa(Adatbazis *adatbazis);

void adatbazis_fajlba_irasa(Adatbazis *adatbazis);

void adatbazis_beolvasasa_fajlbol(Adatbazis *adatbazis2);

void esemeny_felszabaditasa(Esemeny *esemeny);


int main() {
    Adatbazis adatbazis = {NULL, 0};

    for (int i = 0; i < 1; ++i) {
        rekord_hozzaadasa(&adatbazis);
    }
    rekord_torlese(&adatbazis);

    for (int i = 0; i < adatbazis.meret; ++i) {
        rekord_kiirasa(adatbazis.esemenyek[i]);
    }

    for (int i = 0; i < adatbazis.meret; ++i) {
        esemeny_felszabaditasa(&adatbazis.esemenyek[i]);
    }


    return 0;
}
void esemeny_felszabaditasa(Esemeny *esemeny) {
    if (esemeny->nev != NULL) {
        free(esemeny->nev);
        esemeny->nev = NULL;
    }
}


void bemenet_tisztitasa() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

void uj_karaktersor_tisztitasa(char *bemenet) {
    size_t hossz = strlen(bemenet);
    if (hossz > 0 && bemenet[hossz - 1] == '\n') {
        bemenet[hossz - 1] = '\0';
    }
}

void rekord_hozzaadasa(Adatbazis *adatbazis) {
    Esemeny esemeny;
    uj_nev(&esemeny);
    uj_datum(&esemeny);
    uj_ido(&esemeny);
    uj_hely(&esemeny);
    uj_megjegyzes(&esemeny);

    adatbazis->esemenyek = realloc(adatbazis->esemenyek, (adatbazis->meret + 1) * sizeof(Esemeny));
    if (adatbazis->esemenyek == NULL) {
        printf("Hiba történt a memória foglalása során!\n");
        return;
    }
    adatbazis->esemenyek[adatbazis->meret] = esemeny;
    adatbazis->meret++;
    printf("Új esemény hozzáadása sikeres!\n");
    printf("---------------------------------\n");
}

/**
 * Egy tömb résztömbjét másolja át egy céltömbbe
 * @param eredeti az eredeti tömb, amelyből egy résztömböt szeretnénk kimásolni
 * @param cel a céltömb pointere, ahova a résztömb kerül
 * @param kezdo a kezdőindex az eredeti tömbben, ahonnan a másolást kezdjük (az indexelés 0-tól kezdődik)
 * @param veg a végindex az eredeti tömbben, ameddig a másolás tart, ez még belekerül a résztömbbe
 */
void substring(char eredeti[], char *cel, int kezdo, int veg) {
    if (kezdo > veg || kezdo < 0 || veg >= strlen(eredeti)) {
        printf("Helytelen bemenetek!");
    }

    int cel_hossz = veg - kezdo + 1;
    for (int i = 0; i < cel_hossz; ++i) {
        cel[i] = eredeti[kezdo + i];
    }

    cel[cel_hossz] = '\0';
}

bool szokoev(int ev) {
    return (ev % 4 == 0 && ev % 100 != 0) || (ev % 400 == 0);
}

/**
 * Ellenőrzi, hogy egy dátum megfelel-e a megadott formátumnak és az érték helyes-e.
 *
 * @param datum A vizsgálandó dátum karaktertömb (várt formátum: "EEEE. HH. NN.").
 * @return `true`, ha a dátum érvényes és helyes; `false` egyébként.
 */
bool datum_ellenorzo(char datum[]) {
    for (int i = 0; i < 13; ++i) {
        if (i == 4 || i == 8 || i == 12) {
            if (datum[i] != '.') {
                return false;
            }
        } else if (i == 5 || i == 9) {
            if (datum[i] != ' ') {
                return false;
            }
        } else {
            if (!isdigit(datum[i])) {
                return false;
            }
        }
    }

    char ev[5], ho[3], nap[3];

    substring(datum, ev, 0, 3);
    substring(datum, ho, 6, 7);
    substring(datum, nap, 10, 11);

    int ev_int = atoi(ev);
    int ho_int = atoi(ho);
    int nap_int = atoi(nap);


    if (ho_int < 1 || ho_int > 12 || nap_int < 1 || nap_int > 31 || ev_int < 1) {
        return false;
    }

    int honap_napok[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int honap_napok_szokoevben[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if ((szokoev(ev_int) && honap_napok_szokoevben[ho_int - 1] < nap_int) ||
        (!szokoev(ev_int) && honap_napok[ho_int - 1] < nap_int)) {
        return false;
    }

    return true;
}

/**
 * Ellenőrzi, hogy egy időpont megfelel-e a megadott formátumnak és az érték helyes-e.
 *
 * @param datum A vizsgálandó időpont karaktertömb (várt formátum 24 órás időformátumban: "ÓÓ:PP").
 * @return `true`, ha az időpont érvényes és helyes; `false` egyébként.
 */
bool ido_ellenorzo(char ido[]) {
    if (strlen(ido) != 5) {
        return false;
    }

    for (int i = 0; i < 5; ++i) {
        if (i == 2) {
            if (ido[i] != ':') {
                return false;
            }
        } else {
            if (!isdigit(ido[i])) {
                return false;
            }
        }
    }

    char ora[3], perc[3];

    substring(ido, ora, 0, 1);
    substring(ido, perc, 3, 4);

    int ora_int = atoi(ora);
    int perc_int = atoi(perc);


    if (ora_int > 23 || perc_int > 59) {
        return false;
    }

    return true;
}

void uj_nev(Esemeny *esemeny) {
    char nev[MAX_NEV_HOSSZ];
    printf("Adj meg egy nevet az eseményednek: ");

    if (fgets(nev, MAX_NEV_HOSSZ + 1, stdin) == NULL) {
        printf("Sikertelen a beolvasás!\n");
        exit(1);
    }

    uj_karaktersor_tisztitasa(nev);

    esemeny->nev = malloc((strlen(nev) + 1) * sizeof(char));
    if (esemeny->nev == NULL) {
        printf("Hiba történt a memória foglalása során!\n");
    }

    strcpy(esemeny->nev, nev);
}

void uj_datum(Esemeny *esemeny) {
    char datum[DATUMHOSSZ];
    printf("Adj meg egy dátumot az eseményednek: ");

    if (fgets(datum, DATUMHOSSZ + 1, stdin) == NULL) {
        printf("Sikertelen a beolvasás!\n");
    }

    uj_karaktersor_tisztitasa(datum);

    if (!datum_ellenorzo(datum)) {
        printf("Helytelen dátum formátum!\n");
        return;
    }

    char ev[5], ho[3], nap[3];

    substring(datum, ev, 0, 3);
    substring(datum, ho, 6, 7);
    substring(datum, nap, 10, 11);

    int ev_int = atoi(ev);
    int ho_int = atoi(ho);
    int nap_int = atoi(nap);


    Datum d = {ev_int, ho_int, nap_int};
    esemeny->datum = d;
}

void uj_ido(Esemeny *esemeny) {
    bemenet_tisztitasa();
    char ido[IDOHOSSZ];
    printf("Adj meg egy időpontot az eseményednek: ");

    if (fgets(ido, IDOHOSSZ + 1, stdin) == NULL) {
        printf("Hiba történt a beolvasás során!\n");
    }

    uj_karaktersor_tisztitasa(ido);

    if (!ido_ellenorzo(ido)) {
        printf("Helytelen az időpont bemenete");
    }

    char ora[3], perc[3];

    substring(ido, ora, 0, 1);
    substring(ido, perc, 3, 4);

    int ora_int = atoi(ora);
    int perc_int = atoi(perc);


    Ido i = {ora_int, perc_int};
    esemeny->ido = i;
}

void uj_hely(Esemeny *esemeny) {
    bemenet_tisztitasa();
    char hely[MAX_HELY_HOSSZ];
    printf("Adj meg egy helyet az eseményednek: ");

    if (fgets(hely,MAX_HELY_HOSSZ + 1, stdin) == NULL) {
        printf("Sikertelen a beolvasás!\n");
    }
    uj_karaktersor_tisztitasa(hely);

    size_t hossz = strlen(hely);

    esemeny->hely = malloc(hossz + 1 * sizeof(char));
    if (esemeny->hely == NULL) {
        printf("Hiba történt a memória foglalása során!\n");
    }
    strcpy(esemeny->hely, hely);
}

void uj_megjegyzes(Esemeny *esemeny) {
    char megjegyzes[MAX_MEGJEGYZES_HOSSZ];
    printf("Adj meg egy megjegyzest az esemenyednek: ");

    if (fgets(megjegyzes, MAX_MEGJEGYZES_HOSSZ + 1, stdin) == NULL) {
        printf("Sikertelen a beolvasás!\n");
    }
    uj_karaktersor_tisztitasa(megjegyzes);

    size_t hossz = strlen(megjegyzes);

    esemeny->megjegyzes = malloc(hossz + 1 * sizeof(char));
    if (esemeny->megjegyzes == NULL) {
        printf("Hiba történt a memória foglalása során!\n");
        exit(1);
    }

    strcpy(esemeny->megjegyzes, megjegyzes);
}

void rekord_kiirasa(Esemeny esemeny) {
    printf("Esemény neve: %s\n", esemeny.nev);
    printf("Esemény dátuma: %d.%d.%d.\n", esemeny.datum.ev, esemeny.datum.ho, esemeny.datum.nap);
    printf("Esemény időpontja: %d:%d\n", esemeny.ido.ora, esemeny.ido.perc);
    printf("Esemény helyszíne: %s\n", esemeny.hely);
    printf("Esémenyhez tartozó megjegyzés: %s\n", esemeny.megjegyzes);
}

//itt **talalatok kellenek mert ha csak a sima * pointert adjuk at akkor a tomb elemeit fel tudja tolteni, de magát a pointernek az értékét nem, márpedig itt az átméretezés miatt át kell írjuk az értékét
bool rekord_keresese(Adatbazis adatbazis, Esemeny **talalatok) {
    char keresett[MAX_NEV_HOSSZ];

    if (fgets(keresett, MAX_NEV_HOSSZ + 1, stdin) == NULL) {
        printf("Sikertelen a beolvasás!\n");
        exit(1);
    }

    uj_karaktersor_tisztitasa(keresett);

    //kell egy tomb amibe a talalatok kerulnek
    //kezdetben lefoglalunk egy helyet az esemenynek
    //ha tobb lesz -> realloc
    //!!!

    int max_talalat_szam = 3;
    Esemeny *p = (Esemeny *) malloc(max_talalat_szam * sizeof(Esemeny));

    if (p == NULL) {
        printf("Hiba történt a memória foglalása során!");
    }

    int talalatok_szama = 0;
    for (int i = 0; i < adatbazis.meret; ++i) {
        if (strcmp(adatbazis.esemenyek[i].nev, keresett) == 0) {
            if (talalatok_szama >= max_talalat_szam) {
                max_talalat_szam *= 2;
                Esemeny *p2 = (Esemeny *) realloc(p, max_talalat_szam * sizeof(Esemeny));

                if (p2 == NULL) {
                    printf("Hiba történt a memória foglalása során!");
                    free(p);
                }
                p = p2;
            }
            p[talalatok_szama++] = adatbazis.esemenyek[i];
        }
    }

    if (talalatok_szama > 0) {
        printf("============Találatok============\n");
        printf("---------------------------------\n");
        for (int i = 0; i < talalatok_szama; ++i) {
            printf("%d. esemény: \n", i + 1);
            rekord_kiirasa(p[i]);
            printf("---------------------------------\n");
        }
    } else {
        printf("Nincs találat.\n");
        return false;
    }

    if (p != NULL) {
        *talalatok = p;
    } else {
        printf("Hiba történt, a pointer értéke 0");
    }
    return true;
}

void rekord_torlese(Adatbazis *adatbazis) {
    Esemeny *talalatok = NULL;
    int torlendo_talalat_index;

    printf("Add meg az esemény nevét, amit törölni szeretnél: ");
    rekord_keresese(*adatbazis, &talalatok);

    printf("Hányadik sorszámú eseményt szeretnéd törölni? ");
    scanf("%d", &torlendo_talalat_index);

    if (torlendo_talalat_index < 1) {
        printf("Helytelen sorszámot adtál meg!");
        free(talalatok);
    }

    int torlendo_adatbazis_index = torlendo_talalat_index - 1;


    free(adatbazis->esemenyek[torlendo_adatbazis_index].nev);
    free(adatbazis->esemenyek[torlendo_adatbazis_index].hely);
    free(adatbazis->esemenyek[torlendo_adatbazis_index].megjegyzes);


    for (int i = torlendo_adatbazis_index; i < adatbazis->meret - 1; ++i) {
        adatbazis->esemenyek[i] = adatbazis->esemenyek[i + 1];
    }

    adatbazis->meret--;
    adatbazis->esemenyek = realloc(adatbazis->esemenyek, adatbazis->meret * sizeof(Esemeny));

    printf("Esemény törölve.\n");

    free(talalatok);
}
