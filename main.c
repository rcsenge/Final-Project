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
#define IDOHOSSZ 8

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

bool egyezo_esemenyek(const Esemeny *esemeny1, const Esemeny *esemeny2);

void rekord_hozzaadasa(Adatbazis *adatbazis);

void bemenet_tisztitasa();

void uj_karaktersor_tisztitasa(char *bemenet);

void substring(char eredeti[], char *cel, int kezdo, int veg);

bool szokoev(int ev);

int honap_napok(int ho, int ev);

bool datum_ellenorzo(char datum[]);

bool ido_ellenorzo(char ido[]);

void uj_nev(Esemeny *esemeny);

void uj_datum(Esemeny *esemeny);

void uj_ido(Esemeny *esemeny);

void uj_hely(Esemeny *esemeny);

void uj_megjegyzes(Esemeny *esemeny);

void rekord_kiirasa(Esemeny esemeny);

bool rekord_keresese_nev_alapjan(Adatbazis adatbazis, Esemeny **talalatok);

bool rekord_keresese_idoszak_alapjan(Adatbazis adatbazis, Datum kezdo_datum, Datum veg_datum);

void rekord_keresese_nap_alapjan(Adatbazis adatbazis);

void rekord_keresese_het_alapjan(Adatbazis adatbazis);

void rekord_keresese_ho_alapjan(Adatbazis adatbazis);

void rekord_torlese(Adatbazis *adatbazis);

void rekord_modositasa(const Adatbazis *adatbazis);

void adatbazis_kilistazasa(const Adatbazis *adatbazis);

void adatbazis_felszabaditas(Adatbazis *adatbazis);

Datum datum_beolvasas(char *datum);

int esemenyek_osszehasonlitasa(Esemeny e1, Esemeny e2);

int datumok_osszehasonlitasa(Datum d1, Datum d2);

Datum datum_hozzaado(Datum d, int nap);

void rendez(Esemeny *tomb, int eleje, int vege, Esemeny *seged);

void osszefesul(const Esemeny *be, int eleje, int kozepe, int vege, Esemeny *ki);

void masol(const Esemeny *be, int eleje, int vege, Esemeny *ki);

void menu(Adatbazis *adatbazis);

void adatbazis_fajlba_irasa(Adatbazis *adatbazis);

void adatbazis_beolvasasa_fajlbol(Adatbazis *adatbazis2);

int main() {
    Adatbazis adatbazis = {NULL, 0};
    rekord_hozzaadasa(&adatbazis);
    rekord_hozzaadasa(&adatbazis);
    rekord_keresese_het_alapjan(adatbazis);
    adatbazis_felszabaditas(&adatbazis);

    return 0;
}

void adatbazis_felszabaditas(Adatbazis *adatbazis) {
    for (int i = 0; i < adatbazis->meret; ++i) {
        free(adatbazis->esemenyek[i].nev);
        free(adatbazis->esemenyek[i].hely);
        free(adatbazis->esemenyek[i].megjegyzes);
    }

    if (adatbazis->esemenyek != NULL) {
        free(adatbazis->esemenyek);
        adatbazis->esemenyek = NULL;
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

    Esemeny *uj_memoria = realloc(adatbazis->esemenyek, (adatbazis->meret + 1) * sizeof(Esemeny));
    if (uj_memoria == NULL) {
        printf("Hiba történt a memória foglalása során!\n");
        return;
    }

    adatbazis->esemenyek = uj_memoria;

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

    if (nap_int > honap_napok(ho_int, ev_int)) {
        return false;
    }

    return true;
}

int honap_napok(int ho, int ev) {
    int honap_napok[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (szokoev(ev)) {
        honap_napok[1] = 29;
    }
    return honap_napok[ho - 1];
}

/**
 * Ellenőrzi, hogy egy időpont megfelel-e a megadott formátumnak és az érték helyes-e.
 *
 * @param ido
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
    bemenet_tisztitasa();
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
    bemenet_tisztitasa();
    char datum[DATUMHOSSZ];
    printf("Adj meg egy dátumot az eseményednek: ");

    Datum d = datum_beolvasas(datum);
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
        return;
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
    bemenet_tisztitasa();
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

//itt **talalatok kellenek mert ha csak a sima * pointert adjuk at akkor a tomb elemeit fel tudja tolteni,
//de magát a pointernek az értékét nem, márpedig itt az átméretezés miatt át kell írjuk az értékét
bool rekord_keresese_nev_alapjan(Adatbazis adatbazis, Esemeny **talalatok) {
    char keresett[MAX_NEV_HOSSZ];

    if (fgets(keresett, MAX_NEV_HOSSZ + 1, stdin) == NULL) {
        printf("Sikertelen a beolvasás!\n");
        return false;
    }

    uj_karaktersor_tisztitasa(keresett);

    int max_talalat = 3;
    Esemeny *p = (Esemeny *) malloc(max_talalat * sizeof(Esemeny));

    if (p == NULL) {
        printf("Hiba történt a memória foglalása során!");
        free(p);
        return false;
    }

    int talalat_szam = 0;
    for (int i = 0; i < adatbazis.meret; ++i) {
        if (strcmp(adatbazis.esemenyek[i].nev, keresett) == 0) {
            if (talalat_szam >= max_talalat) {
                max_talalat *= 2;
                Esemeny *p2 = realloc(p, max_talalat * sizeof(Esemeny));

                if (p2 == NULL) {
                    printf("Hiba történt a memória foglalása során!");
                    free(p);
                    return false;
                }
                p = p2;
            }
            p[talalat_szam++] = adatbazis.esemenyek[i];
        }
    }

    if (talalat_szam > 0) {
        *talalatok = p;
        printf("============Találatok============\n");
        printf("---------------------------------\n");
        for (int i = 0; i < talalat_szam; ++i) {
            printf("%d. esemény: \n", i + 1);
            rekord_kiirasa(p[i]);
            printf("---------------------------------\n");
        }

    } else {
        printf("Nincs találat.\n");
        return false;
    }

    free(p);
    return true;
}

void rekord_torlese(Adatbazis *adatbazis) {
    Esemeny *talalatok = NULL;
    int torlendo_talalat_index;

    printf("Add meg az esemény nevét, amit törölni szeretnél: ");
    rekord_keresese_nev_alapjan(*adatbazis, &talalatok);

    printf("Hányadik sorszámú eseményt szeretnéd törölni? ");
    scanf("%d", &torlendo_talalat_index);

    if (torlendo_talalat_index < 1) {
        printf("Helytelen sorszámot adtál meg!");
        free(talalatok);
        return;
    }

    //nagyon nem jo!
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

void rekord_modositasa(const Adatbazis *adatbazis) {
    Esemeny *talalatok = NULL;
    int talalat_index;

    printf("Add meg az esemény nevét, amit módosítani szeretnél: ");
    rekord_keresese_nev_alapjan(*adatbazis, &talalatok);

    printf("Hányadik sorszámút szeretnéd módosítani? ");
    scanf("%d", &talalat_index);

    if (talalat_index < 1) {
        printf("Helytelen sorszámot adtál meg!");
        free(talalatok);
        return;
    }

    int ertek_index;
    // ide akár a régi értékeket be lehetne tenni
    printf("Melyik érteket szeretnéd modosítani?\n");
    printf("1. Esemény neve\n");
    printf("2. Esemény dátuma\n");
    printf("3. Esemény időpontja\n");
    printf("4. Esemény helyszíne\n");
    printf("5. Esemény megjegyzése\n");
    scanf("%d", &ertek_index);

    if (ertek_index < 1 || ertek_index > 5) {
        printf("Helytelen sorszámot adtál meg!");
        free(talalatok);
        return;
    }

    Esemeny *modositando_esemeny = &talalatok[talalat_index - 1];
    Esemeny *adatbazisban_levo_esemeny = NULL;

    for (int i = 0; i < adatbazis->meret; ++i) {
        if (egyezo_esemenyek(&adatbazis->esemenyek[i], modositando_esemeny)) {
            adatbazisban_levo_esemeny = &adatbazis->esemenyek[i];
            break;
        }
    }

    if (adatbazisban_levo_esemeny == NULL) {
        return;
    }

    switch (ertek_index) {
        case 1:
            free(adatbazisban_levo_esemeny->nev);
            uj_nev(adatbazisban_levo_esemeny);
            break;
        case 2:
            uj_datum(adatbazisban_levo_esemeny);
            break;
        case 3:
            uj_ido(adatbazisban_levo_esemeny);
            break;
        case 4:
            free(adatbazisban_levo_esemeny->hely);
            uj_hely(adatbazisban_levo_esemeny);
            break;
        case 5:
            free(adatbazisban_levo_esemeny->megjegyzes);
            uj_megjegyzes(adatbazisban_levo_esemeny);
            break;
        default:
            printf("Helytelen bemenetet adtál meg!");
            return;
    }
    printf("Sikeresen módosult az esemény!\n");
}

bool egyezo_esemenyek(const Esemeny *esemeny1, const Esemeny *esemeny2) {
    if ((strcmp(esemeny1->nev, esemeny2->nev) == 0)
        && (strcmp(esemeny1->hely, esemeny2->hely) == 0)
        && (strcmp(esemeny1->megjegyzes, esemeny2->megjegyzes) == 0)
        && (esemeny1->datum.ev == esemeny2->datum.ev)
        && (esemeny1->datum.ho == esemeny2->datum.ho)
        && (esemeny1->datum.nap == esemeny2->datum.nap)
        && (esemeny1->ido.ora == esemeny2->ido.ora)
        && (esemeny1->ido.perc == esemeny2->ido.perc)) {
        return true;
    }
    return false;
}

void adatbazis_kilistazasa(const Adatbazis *adatbazis) {
    int idoszak;
    printf("Melyik időszak eseményeit szeretnéd kilistázni?\n");
    printf("1. Egy adott nap\n");
    printf("2. Egy adott hét\n");
    printf("3. Egy adott hónap\n");

    scanf("%d", &idoszak);
    if (idoszak < 1 || idoszak > 3) {
        printf("Helytelen bemenetet adtál meg!\n");
        return;
    }

    switch (idoszak) {
        case 1:
            rekord_keresese_nap_alapjan(*adatbazis);
            break;
        case 2:
            rekord_keresese_het_alapjan(*adatbazis);
            break;
        case 3:
            rekord_keresese_ho_alapjan(*adatbazis);
            break;
        default:
            printf("Helytelen bemenetet adtál meg!\n");
    }
}


void rekord_keresese_nap_alapjan(Adatbazis adatbazis) {
    bemenet_tisztitasa();
    char datum[DATUMHOSSZ];
    printf("Add meg a nap dátumát: ");
    Datum kezdo_datum = datum_beolvasas(datum);
    Datum veg_datum = kezdo_datum;
    rekord_keresese_idoszak_alapjan(adatbazis, kezdo_datum, veg_datum);
}

void rekord_keresese_het_alapjan(Adatbazis adatbazis) {
    bemenet_tisztitasa();
    char datum[DATUMHOSSZ];
    printf("Add meg a nap dátumát: ");
    Datum kezdo_datum = datum_beolvasas(datum);
    Datum veg_datum = datum_hozzaado(kezdo_datum, 7);

    rekord_keresese_idoszak_alapjan(adatbazis, kezdo_datum, veg_datum);
}

void rekord_keresese_ho_alapjan(Adatbazis adatbazis) {
    bemenet_tisztitasa();
    char datum[DATUMHOSSZ];
    printf("Add meg a nap dátumát: ");
    Datum kezdo_datum = datum_beolvasas(datum);
    Datum veg_datum = datum_hozzaado(kezdo_datum, honap_napok(kezdo_datum.ho, kezdo_datum.ev));
    rekord_keresese_idoszak_alapjan(adatbazis, kezdo_datum, veg_datum);
}

bool rekord_keresese_idoszak_alapjan(Adatbazis adatbazis, Datum kezdo_datum, Datum veg_datum) {
    int max_talalat = 3;
    Esemeny *p = (Esemeny *) malloc(max_talalat * sizeof(Esemeny));

    if (p == NULL) {
        printf("Hiba történt a memória foglalása során!");
        free(p);
        return false;
    }

    int talalat_szam = 0;
    for (int i = 0; i < adatbazis.meret; ++i) {
        Esemeny *p2 = NULL;
        if (datumok_osszehasonlitasa(kezdo_datum, adatbazis.esemenyek[i].datum) <= 0
            && datumok_osszehasonlitasa(veg_datum, adatbazis.esemenyek[i].datum) >= 0) {
            if (talalat_szam >= max_talalat) {
                max_talalat *= 2;
                p2 = realloc(p, max_talalat * sizeof(Esemeny));

                if (p2 == NULL) {
                    printf("Hiba történt a memória foglalása során!");
                    free(p);
                    free(p2);
                    return false;
                }
                p = p2;
            }
            p[talalat_szam++] = adatbazis.esemenyek[i];
        }
        if (i == adatbazis.meret - 1) {
            free(p2);
        }
    }

    Esemeny *seged = malloc(talalat_szam * sizeof(Esemeny));
    rendez(p, 0, talalat_szam, seged);
    free(seged);

    if (talalat_szam > 0) {
        printf("============Találatok============\n");
        printf("---------------------------------\n");
        for (int i = 0; i < talalat_szam; ++i) {
            printf("%d. esemény: \n", i + 1);
            rekord_kiirasa(p[i]);
            printf("---------------------------------\n");
        }
    } else {
        printf("Nincs találat.\n");
        free(p);
        return false;
    }

    free(p);
    return true;
}

Datum datum_beolvasas(char *datum) {
    if (fgets(datum, DATUMHOSSZ + 1, stdin) == NULL) {
        printf("Sikertelen a beolvasás!\n");
    }

    uj_karaktersor_tisztitasa(datum);

    if (!datum_ellenorzo(datum)) {
        printf("Helytelen dátum formátum!\n");
        //TODO ezt ellenorizni a fgv hivasnal
        Datum d = {0, 0, 0};
        return d;
    }

    char ev[5], ho[3], nap[3];

    substring(datum, ev, 0, 3);
    substring(datum, ho, 6, 7);
    substring(datum, nap, 10, 11);

    int ev_int = atoi(ev);
    int ho_int = atoi(ho);
    int nap_int = atoi(nap);

    Datum d = {ev_int, ho_int, nap_int};
    return d;
}

// Pozitív, ha datum1 későbbi, negatív ha korábbi, 0 ha egyenlőek
int datumok_osszehasonlitasa(Datum d1, Datum d2) {
    if (d1.ev != d2.ev) {
        return d1.ev - d2.ev;
    }
    if (d1.ho != d2.ho) {
        return d1.ho - d2.ho;
    }
    return d1.nap - d2.nap;
}

// Pozitív, ha datum1 későbbi, negatív ha korábbi, 0 ha egyenlőek
//TODO ahol használtam ezt kijavítani hogy időt is használjon
int esemenyek_osszehasonlitasa(Esemeny e1, Esemeny e2) {
    int datum_eredmeny = datumok_osszehasonlitasa(e1.datum, e2.datum);
    if (datum_eredmeny != 0) {
        return datum_eredmeny;
    }

    if (e1.ido.ora != e2.ido.ora) {
        return e1.ido.ora - e2.ido.ora;
    }
    return e1.ido.perc - e2.ido.perc;
}

Datum datum_hozzaado(Datum d, int nap) {
    if (nap > 0) {
        int napok_a_honapban = honap_napok(d.ho, d.ev);

        if (d.nap + nap <= napok_a_honapban) {
            d.nap += nap;
            return d;
        }

        d.nap = d.nap + nap - napok_a_honapban;

        if (d.ho == 12) {
            d.ev++;
            d.ho = 1;
        } else {
            d.ho++;
        }
    } else {
        printf("Helytelen nap számot adtál meg!");
    }
    return d;
}

void rendez(Esemeny *tomb, int eleje, int vege, Esemeny *seged) {
    if (vege - eleje < 2)
        return;
    int kozepe = (eleje + vege) / 2;
    rendez(tomb, eleje, kozepe, seged);
    rendez(tomb, kozepe, vege, seged);
    osszefesul(tomb, eleje, kozepe, vege, seged);
    masol(seged, eleje, vege, tomb);
}

void osszefesul(const Esemeny *be, int eleje, int kozepe, int vege, Esemeny *ki) {
    int i = eleje, j = kozepe;
    for (int c = eleje; c < vege; ++c) {
        if (i < kozepe && (j >= vege || esemenyek_osszehasonlitasa(be[i], be[j]) <= 0)) {
            ki[c] = be[i];
            i++;
        } else {
            ki[c] = be[j];
            j++;
        }
    }
}

void masol(const Esemeny *be, int eleje, int vege, Esemeny *ki) {
    for (int c = eleje; c < vege; ++c)
        ki[c] = be[c];
}
