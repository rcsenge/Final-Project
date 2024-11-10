#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debugmalloc.h"
#define DATUMHOSSZ 13
#define IDOHOSSZ 5

//TODO olvashatobb valtozo nevek
//TODO modulokra bontas
//TODO malloc, realloc ellenorzesek
//TODO reszletesebb dokumentacio
//TODO teszteles

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

void substring(char eredeti[], char *cel, int kezdo, int veg);

bool szokoev(int ev);

int napok_szama_honapban(int ho, int ev);

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

void adatbazis_felszabaditasa(Adatbazis *adatbazis);

Datum datum_beolvasas(char *datum);

int esemenyek_osszehasonlitasa(Esemeny e1, Esemeny e2);

int datumok_osszehasonlitasa(Datum d1, Datum d2);

Datum datum_hozzaado(Datum d, int nap);

void rendez(Esemeny *tomb, int eleje, int vege, Esemeny *seged);

void osszefesul(const Esemeny *be, int eleje, int kozepe, int vege, Esemeny *ki);

void masol(const Esemeny *be, int eleje, int vege, Esemeny *ki);

bool menu(Adatbazis *adatbazis);

bool adatbazis_fajlba_irasa(Adatbazis adatbazis);

bool adatbazis_beolvasasa_fajlbol(Adatbazis *adatbazis);

int main() {
    Adatbazis adatbazis = {NULL, 0};
    bool fut = true;

    while (fut) {
        fut = menu(&adatbazis);
    }
    adatbazis_felszabaditasa(&adatbazis);

    return 0;
}

/**
 * A megadott adatbazis esemeny tombjenek, valamint az adatbazisnak a felszabaditasa
 * @param adatbazis Az adatbazis, ami felszabaditasra kerul
 */
void adatbazis_felszabaditasa(Adatbazis *adatbazis) {
    for (int i = 0; i < adatbazis->meret; ++i) {
        free(adatbazis->esemenyek[i].nev);
        free(adatbazis->esemenyek[i].hely);
        free(adatbazis->esemenyek[i].megjegyzes);
    }
    free(adatbazis->esemenyek);
    adatbazis->esemenyek = NULL;
}

/**
 * Egy uj esemeny adatait keri be a felhasznalotol a strandard bemeneten, majd elmenti az adatbazisban
 * @param adatbazis Az adatbazis pointere, amihez az uj esemeny hozzaadodik.
 *                  A fuggveny hivo feladata a memoria felszabaditasa.
 */
void rekord_hozzaadasa(Adatbazis *adatbazis) {
    Esemeny esemeny;
    uj_nev(&esemeny);
    uj_datum(&esemeny);
    uj_ido(&esemeny);
    uj_hely(&esemeny);
    uj_megjegyzes(&esemeny);

    Esemeny *bovitett_esemenyek = realloc(adatbazis->esemenyek, (adatbazis->meret + 1) * sizeof(Esemeny));
    if (bovitett_esemenyek == NULL) {
        printf("Hiba tortent a memoria foglalasa soran!\n");
        return;
    }

    adatbazis->esemenyek = bovitett_esemenyek;

    adatbazis->esemenyek[adatbazis->meret] = esemeny;
    adatbazis->meret++;
    printf("Uj esemeny hozaadasa sikeres!\n");
    printf("---------------------------------\n");
}

/**
 * Egy tomb resztombjet masolja at egy celtombbe
 * @param eredeti az eredeti tomb, amelybol egy résztömböt szeretnénk kimásolni
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

/**
 * Ellenorzi, hogy egy ev szokoev-e
 * @param ev A vizsgalando ev
 * @return true, ha az ev szokoev
 *         false, ha az ev nem szokoev
 */
bool szokoev(int ev) {
    return (ev % 4 == 0 && ev % 100 != 0) || (ev % 400 == 0);
}

/**
 * Ellenorzi, hogy egy datum megfelel-e a megadott formatumnak es az ertek helyes-e
 *
 * @param datum A vizsgalando datum karaktertombje (vart formatum: "EEEE. HH. NN.")
 * @return true, ha a datum ervenyes es helyes
 *         false ha a datum nem ervenyes vagy helytelen
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

    if (nap_int > napok_szama_honapban(ho_int, ev_int)) {
        return false;
    }

    return true;
}

/**
 * Kiszamolja, hogy egy megadott ev megadott honapja hany napbol all
 * @param ho a honap szama, 1-12
 * @param ev az ev
 * @return az ev honapjanak a napjainak szama
 */
int napok_szama_honapban(int ho, int ev) {
    int napok_szama_honapban[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (szokoev(ev)) {
        napok_szama_honapban[1] = 29;
    }
    return napok_szama_honapban[ho - 1];
}

/**
 * Ellenorzi, hogy egy idopont megfelel-e a megadott formatumnak es az ertek helyes-e
 * A formatum az OO:PP, ahol az ora 0 es 23 kozott van (a 0-t es a 23-t is megengedve) es a perc 0 es 59 kozott van (a 0-t es az 59-t is megengedve)
 *
 * @param ido Az idopont, amit ellenorzunk
 * @return true, ha az idopont ervenyes es helyes
 *         false, ha az idopont ervenytelen vagy helytelen
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

/**
 * Beker a felhasznalotol a standard bemeneten egy nevet, majd ezt eltarolja egy esemenyben
 * @param esemeny Az esemeny, ahova az uj nevet meg kell adni
 */
void uj_nev(Esemeny *esemeny) {
    printf("Adj meg egy nevet: ");
    char *nev = malloc(1 * sizeof(char));
    if (nev == NULL) {
        printf("Hiba tortent a memoria foglalasa soran!\n");
        return;
    }

    int hossz = 0;
    char c;
    while ((c = getchar()) != '\n') {
        nev[hossz] = c;
        hossz++;

        nev = realloc(nev, (hossz + 1) * sizeof(char));
        if (nev == NULL) {
            printf("Hiba tortent a memoria atmeretezese soran!\n");
            return;
        }
    }
    nev[hossz] = '\0';
    esemeny->nev = malloc((strlen(nev) + 1) * sizeof(char));
    if (esemeny->nev == NULL) {
        printf("Hiba tortent a memoria foglalasa soran!\n");
        free(nev);
        return;
    }

    strcpy(esemeny->nev, nev);
    free(nev);
}

/**
 * Beolvassa a felhasznalo altal megadott datumot a standard bemenetrol, ellenorzi annak formatumat, es
 * egy Datum strukturava alakitja.
 *
 * @param datum Egy karaktertomb pointere, ahol a beolvasott datum szovege van (elvart formatum: "EEEE-HH-NN")
 *
 * @return Egy Datum strukturat, amely:
 *         - Az ev, honap és nap ertekeit tartalmazza egesz szamkent, ha a bemenet helyes
 *         - {0, 0, 0} erteket tartalmaz, ha a bemenet ervenytelen
 */
Datum datum_beolvasas(char *datum) {
    int sikerult = scanf("%13s", datum);
    if (sikerult != 1) {
        printf("Helytelen bemenetet adtal meg!\n");
        //TODO ezt ellenorizni a fgv hivasnal
        Datum d = {0, 0, 0};
        return d;
    }

    if (!datum_ellenorzo(datum)) {
        printf("Helytelen datum formatum!\n");
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

/**
 * Beker a felhasznalotol a standard bemeneten egy datumot, majd ezt eltarolja egy esemenyben
 * @param esemeny Az esemeny, ahova az uj datumot meg kell adni
 */
void uj_datum(Esemeny *esemeny) {
    char datum[DATUMHOSSZ + 1];
    printf("Adj meg egy datumot az esemenyednek: ");

    Datum d = datum_beolvasas(datum);
    esemeny->datum = d;
}

/**
 * Beker a felhasznalotol a standard bemeneten egy idopontot, majd ezt eltarolja egy esemenyben
 * @param esemeny Az esemeny, ahova az uj datumot meg kell adni
 */
void uj_ido(Esemeny *esemeny) {
    char ido[IDOHOSSZ + 1];
    printf("Adj meg egy idopontot az esemenyednek: ");
    scanf("%5s", ido);

    if (!ido_ellenorzo(ido)) {
        printf("Helytelen az idopont bemenete");
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

/**
 * Beker a felhasznalotol a standard bemeneten egy helyszint, majd ezt eltarolja egy esemenyben
 * @param esemeny Az esemeny, ahova az uj helyszint meg kell adni
 */
void uj_hely(Esemeny *esemeny) {
    char *hely = malloc(1 * sizeof(char));
    if (hely == NULL) {
        printf("Hiba tortent a memoria foglalasa soran!\n");
    }

    printf("Adj meg egy helyet az esemenyednek: ");

    int hossz = 0;
    char c;
    while ((c = getchar()) != '\n') {
        hely[hossz] = c;
        hossz++;

        hely = realloc(hely, (hossz + 1) * sizeof(char));
        if (hely == NULL) {
            printf("Hiba tortent a memoria atmeretezese soran!\n");
            return;
        }
    }
    hely[hossz] = '\0';

    esemeny->hely = malloc(hossz + 1 * sizeof(char));
    if (esemeny->hely == NULL) {
        printf("Hiba tortent a memoria foglalasa soran!\n");
    }
    strcpy(esemeny->hely, hely);
    free(hely);
}

/**
 * Beker a felhasznalotol a standard bemeneten egy megjegyzest, majd ezt eltarolja egy esemenyben
 * @param esemeny Az esemeny, ahova az uj megjegyzest meg kell adni
 */
void uj_megjegyzes(Esemeny *esemeny) {
    char *megjegyzes = malloc(1 * sizeof(char));
    if (megjegyzes == NULL) {
        printf("Hiba tortent a memoria foglalasa soran!\n");
    }
    printf("Adj meg egy megjegyzest az esemenyednek: ");

    int hossz = 0;
    char c;
    while ((c = getchar()) != '\n') {
        megjegyzes[hossz] = c;
        hossz++;

        megjegyzes = realloc(megjegyzes, (hossz + 1) * sizeof(char));
        if (megjegyzes == NULL) {
            printf("Hiba tortent a memoria atmeretezese soran!\n");
            return;
        }
    }
    megjegyzes[hossz] = '\0';

    esemeny->megjegyzes = malloc(hossz + 1 * sizeof(char));
    if (esemeny->megjegyzes == NULL) {
        printf("Hiba tortent a memoria foglalasa soran!\n");
        exit(1);
    }

    strcpy(esemeny->megjegyzes, megjegyzes);
    free(megjegyzes);
}

/**
 * Kiirja egy Esemeny rekord adatait a standard kimenetre, az esemeny nev, datum, idopont, helyszin es megjegyzes mezoket.
 *
 * @param esemeny Az Esemeny tipusu strukturat tartalmazza, amelynek adatait ki kell irni.
 */
void rekord_kiirasa(Esemeny esemeny) {
    printf("Esemeny neve: %s\n", esemeny.nev);
    printf("Esemeny datuma: %d.%d.%d.\n", esemeny.datum.ev, esemeny.datum.ho, esemeny.datum.nap);
    printf("Esemeny idopontja: %d:%d\n", esemeny.ido.ora, esemeny.ido.perc);
    printf("Esemeny helyszine: %s\n", esemeny.hely);
    printf("Esemenyhez tartozo megjegyzes: %s\n", esemeny.megjegyzes);
}

/**
 * Egy felhasznalotol bekert nevet megkeres az adatbazisban es kilistazza az osszes talalatot
 * A nevet a szabvanyos bemenetrol keri be es a hossz nincs korlatozva
 *
 * @param adatbazis Az adatbazis, amiben keresni fogja a megadott nevet
 * @param talalatok Egy pointer pointere, a fuggveny a talalatokra allitja. Ha nincs talalat vagy hiba tortenik, akkor nem kerul modositasra
 *                  A fuggveny hivo feladata a tomb felszabaditasa
 * @return true, ha a keresesnek van legalabb egy talalata es nem tortent hiba a memoria foglalas kozben
 *         false, ha a keresesnek nincs talalata vagy hiba tortent a memoria foglalas kozben
 */
bool rekord_keresese_nev_alapjan(Adatbazis adatbazis, Esemeny **talalatok) {
    char *keresett = (char *) malloc(1 * sizeof(char));
    if (keresett == NULL) {
        printf("Hiba tortent a memoria foglalasa soran!\n");
        return false;
    }

    int hossz = 0;
    char c;
    while ((c = getchar()) != '\n') {
        keresett[hossz] = c;
        hossz++;

        keresett = realloc(keresett, (hossz + 1) * sizeof(char));
        if (keresett == NULL) {
            printf("Hiba tortent a memoria atmeretezese soran!\n");
            return false;
        }
    }
    keresett[hossz] = '\0';

    int kezdeti_max_talalat = 1;
    Esemeny *talalatok_tomb = (Esemeny *) malloc(kezdeti_max_talalat * sizeof(Esemeny));

    if (talalatok_tomb == NULL) {
        printf("Hiba tortent a memoria foglalasa soran!\n");
        free(talalatok);
        return false;
    }

    int talalat_szam = 0;
    for (int i = 0; i < adatbazis.meret; ++i) {
        if (strcmp(adatbazis.esemenyek[i].nev, keresett) == 0) {
            if (talalat_szam >= kezdeti_max_talalat) {
                kezdeti_max_talalat *= 2;
                talalatok_tomb = realloc(talalatok, kezdeti_max_talalat * sizeof(Esemeny));

                if (talalatok_tomb == NULL) {
                    printf("Hiba tortent a memoria foglalasa soran!\n");
                    free(talalatok);
                    return false;
                }
            }
            talalatok_tomb[talalat_szam++] = adatbazis.esemenyek[i];
        }
    }
    free(keresett);
    if (talalat_szam > 0) {
        *talalatok = talalatok_tomb;
        printf("============Talalatok============\n");
        printf("---------------------------------\n");
        for (int i = 0; i < talalat_szam; ++i) {
            printf("%d. esemeny: \n", i + 1);
            rekord_kiirasa(talalatok_tomb[i]);
            printf("---------------------------------\n");
        }
    } else {
        printf("Nincs talalat\n");
        return false;
    }

    free(talalatok_tomb);
    return true;
}

/**
* Torli az adott nevvel rendelkezo esemenyt az adatbazisbol
* A fuggveny megkeresi a felhasznalo altal megadott nevu esemenyt, majd a nevvel egyezo nevu esemenyeket sorszamozva kilistazza
* Ezutan a felhasznalo megad egy sorszamot, ez a rekord torlodik
*
* @param adatbazis Egy pointer az Adatbazis strukturara, amiben egy esemeny torleset vegezzuk
**/
void rekord_torlese(Adatbazis *adatbazis) {
    Esemeny *talalatok = NULL;
    int torlendo_talalat_index;

    printf("Add meg az esemeny nevet, amit torolni szeretnel: ");
    rekord_keresese_nev_alapjan(*adatbazis, &talalatok);

    printf("Hanyadik sorszamu esemenyt szeretned torolni? ");
    scanf("%d", &torlendo_talalat_index);

    if (torlendo_talalat_index < 1) {
        printf("Helytelen sorszamot adtal meg!");
        free(talalatok);
        return;
    }

    //TODO nagyon nem jo!
    int torlendo_adatbazis_index = torlendo_talalat_index - 1;


    free(adatbazis->esemenyek[torlendo_adatbazis_index].nev);
    free(adatbazis->esemenyek[torlendo_adatbazis_index].hely);
    free(adatbazis->esemenyek[torlendo_adatbazis_index].megjegyzes);


    for (int i = torlendo_adatbazis_index; i < adatbazis->meret - 1; ++i) {
        adatbazis->esemenyek[i] = adatbazis->esemenyek[i + 1];
    }

    adatbazis->meret--;
    adatbazis->esemenyek = realloc(adatbazis->esemenyek, adatbazis->meret * sizeof(Esemeny));
    if (adatbazis->esemenyek == NULL) {
        printf("Hiba tortent a memoria atmeretezese soran!\n");
    }

    printf("Esemeny torolve.\n");

    free(talalatok);
}

/**
 * Modosit egy adott esemenyt a felhasznalo valasztasa alapjan.
 * A felhasznalonak az esemeny nevet kell megadja, a nevben egyezo esemenyek sorszamozva kiiratasra kerulnek
 * A felhasznalo megad egy sorszamot, ez a rekord lesz modositva
 * Ezutan a rekord mezoinek elnevezesei kerul kilistazasra, ezek kozul a felhasznalonak kell valasztania, hogy melyiket kivanja modositai
 * A valasztott mezo uj erteket beirja a felhasznalo es modosul az esemeny
 * @param adatbazis Az adatbazis, amely az esemenyt tartalmazza
 */
void rekord_modositasa(const Adatbazis *adatbazis) {
    Esemeny *talalatok = NULL;
    int talalat_index;

    printf("Add meg az esemeny nevet, amit modositani szeretnel: ");
    rekord_keresese_nev_alapjan(*adatbazis, &talalatok);

    printf("Hanyadik sorszamut szeretned modositani? ");
    scanf("%d", &talalat_index);

    if (talalat_index < 1) {
        printf("Helytelen sorszamot adtal meg!");
        free(talalatok);
        return;
    }

    int ertek_index;
    // TODO ide akár a régi értékeket be lehetne tenni
    printf("Melyik erteket szeretned modositani?\n");
    printf("1. Esemeny neve\n");
    printf("2. Esemeny datuma\n");
    printf("3. Esemeny idopontja\n");
    printf("4. Esemeny helyszine\n");
    printf("5. Esemeny megjegyzese\n");
    scanf("%d", &ertek_index);

    if (ertek_index < 1 || ertek_index > 5) {
        printf("Helytelen sorszamot adtal meg!");
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
            printf("Helytelen bemenetet adtal meg!");
            return;
    }
    printf("Sikeresen módosult az esemeny!\n");
}
/**
 * Ellenorzi, hogy ket esemeny osszes mezojenek erteke megegyezik-e
 * @param esemeny1 Az egyik esemeny
 * @param esemeny2 A masik esemeny
 * @return true, ha megegyeznek
 *         false, ha nem egyeznek meg
 */
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

/**
 * Kilistazza az adatbazis esemenyeit a valasztott idoszak alapjan.
 * @param adatbazis Az adatbazis, amely az esemenyeket tartalmazza.
 */
void adatbazis_kilistazasa(const Adatbazis *adatbazis) {
    int idoszak;
    printf("Melyik idoszak esemenyeit szeretned kilistazni?\n");
    printf("1. Egy adott nap\n");
    printf("2. Egy adott het\n");
    printf("3. Egy adott hónap\n");

    scanf("%d", &idoszak);
    if (idoszak < 1 || idoszak > 3) {
        printf("Helytelen bemenetet adtal meg!\n");
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
            printf("Helytelen bemenetet adtal meg!\n");
    }
}

/**
 * Keresesi fuggveny egy nap alapjan torteno esemeny megjelenitesere
 * @param adatbazis Az adatbazis, amely az esemenyeket tartalmazza
 */
void rekord_keresese_nap_alapjan(Adatbazis adatbazis) {
    char datum[DATUMHOSSZ];
    printf("Add meg a nap datumat: ");
    Datum kezdo_datum = datum_beolvasas(datum);
    Datum veg_datum = kezdo_datum;
    rekord_keresese_idoszak_alapjan(adatbazis, kezdo_datum, veg_datum);
}
/**
 * Keresesi fuggveny egy het alapjan torteno esemeny megjelenitesere
 * @param adatbazis Az adatbazis, amely az esemenyeket tartalmazza
 */
void rekord_keresese_het_alapjan(Adatbazis adatbazis) {
    char datum[DATUMHOSSZ];
    //TODO ezt atnezni
    printf("Add meg a nap datumat: ");
    Datum kezdo_datum = datum_beolvasas(datum);
    Datum veg_datum = datum_hozzaado(kezdo_datum, 7);

    rekord_keresese_idoszak_alapjan(adatbazis, kezdo_datum, veg_datum);
}

/**
 * Keresesi fuggveny egy honap alapjan torteno esemeny megjelenitesere
 * @param adatbazis Az adatbazis, amely az esemenyeket tartalmazza
 */
void rekord_keresese_ho_alapjan(Adatbazis adatbazis) {
    char datum[DATUMHOSSZ];
    //TODO ezt atnezni
    printf("Add meg a honap datumat: ");
    Datum kezdo_datum = datum_beolvasas(datum);
    Datum veg_datum = datum_hozzaado(kezdo_datum, napok_szama_honapban(kezdo_datum.ho, kezdo_datum.ev));
    rekord_keresese_idoszak_alapjan(adatbazis, kezdo_datum, veg_datum);
}

/**
 * Kereses idoszak alapjan, esemenyeket talal meg es rendez
 * @param adatbazis Az adatbazis, amely az esemenyeket tartalmazza
 * @param kezdo_datum A keresesi idoszak kezdete
 * @param veg_datum A keresesi idoszak vege
 * @return true, ha talal esemenyt
 *         false, ha nem talal
 */
bool rekord_keresese_idoszak_alapjan(Adatbazis adatbazis, Datum kezdo_datum, Datum veg_datum) {
    int max_talalat = 3;
    //TODO p atnevezes
    Esemeny *p = (Esemeny *) malloc(max_talalat * sizeof(Esemeny));
    if (p == NULL) {
        printf("Hiba tortent a memoria foglalasa soran!\n");
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
                    printf("Hiba tortent a memoria foglalasa soran!\n");
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
    if (seged == NULL) {
        printf("Hiba tortent a memoria foglalas soran!\n");
    }
    rendez(p, 0, talalat_szam, seged);
    free(seged);

    if (talalat_szam > 0) {
        printf("============Talalatok============\n");
        printf("---------------------------------\n");
        for (int i = 0; i < talalat_szam; ++i) {
            printf("%d. esemeny: \n", i + 1);
            rekord_kiirasa(p[i]);
            printf("---------------------------------\n");
        }
    } else {
        printf("Nincs talalat.\n");
        free(p);
        return false;
    }

    free(p);
    return true;
}


/**
 * Ket datumot hasonlit ossze
 * @param d1 Az elso datum
 * @param d2 A masodik datum
 * @return Pozitiv, ha d1 kesobbi, negativ ha korabbi, 0 ha egyenloek
 */
int datumok_osszehasonlitasa(Datum d1, Datum d2) {
    if (d1.ev != d2.ev) {
        return d1.ev - d2.ev;
    }
    if (d1.ho != d2.ho) {
        return d1.ho - d2.ho;
    }
    return d1.nap - d2.nap;
}

/**
 * Ket esemenyt hasonlit ossze datum es ido alapjan
 * @param e1 Az elso esemeny
 * @param e2 A masodik esemeny
 * @return Pozitiv, ha e1 kesobbi, negativ ha korabbi, 0 ha egyenloek
 */
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
/**
 * Datumhoz napokat ad hozza
 * @param d Az alapdatum
 * @param nap A hozzaadando napok szama
 * @return Az uj datum
 */
Datum datum_hozzaado(Datum d, int nap) {
    if (nap > 0) {
        int napok_a_honapban = napok_szama_honapban(d.ho, d.ev);

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
        //TODO atnezni
        printf("Helytelen nap szamot adtal meg!");
    }
    return d;
}
/**
 * Rendez egy esemenytombot
 * @param tomb A rendezendo tomb
 * @param eleje A rendezett szakasz eleje
 * @param vege A rendezett szakasz vege
 * @param seged A segedtomb a rendezeshez
 */
void rendez(Esemeny *tomb, int eleje, int vege, Esemeny *seged) {
    if (vege - eleje < 2)
        return;
    int kozepe = (eleje + vege) / 2;
    rendez(tomb, eleje, kozepe, seged);
    rendez(tomb, kozepe, vege, seged);
    osszefesul(tomb, eleje, kozepe, vege, seged);
    masol(seged, eleje, vege, tomb);
}

/**
 * Ket tombot osszefesul rendezes kozben.
 * @param be A bemeneti tomb.
 * @param eleje A rendezett szakasz eleje.
 * @param kozepe A rendezett szakasz kozepe.
 * @param vege A rendezett szakasz vege.
 * @param ki A kimeneti tomb.
 */
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

/**
 * Masolja a tombot a kimeneti tombbe.
 * @param be A bemeneti tomb.
 * @param eleje A masolasi szakasz eleje.
 * @param vege A masolasi szakasz vege.
 * @param ki A kimeneti tomb.
 */
void masol(const Esemeny *be, int eleje, int vege, Esemeny *ki) {
    for (int c = eleje; c < vege; ++c)
        ki[c] = be[c];
}
/**
 * Az adatbazist fajlba irja.
 * @param adatbazis Az adatbazis.
 * @return true, ha sikeres, kulonben false.
 */
bool adatbazis_fajlba_irasa(Adatbazis adatbazis) {
    const char *file_nev = "adatbazis.txt";

    FILE *file = fopen(file_nev, "w");
    if (file == NULL) {
        printf("Sikertelen volt a file megnyitasa!\n");
        return false;
    }

    fprintf(file, "---------------------------------\n");
    for (int i = 0; i < adatbazis.meret; ++i) {
        Esemeny esemeny = adatbazis.esemenyek[i];
        //TODO osszevonni a rekord kilistazasaval
        fprintf(file, "%d. esemeny: \n", i + 1);
        fprintf(file, "Esemeny neve: %s\n", esemeny.nev);
        fprintf(file, "Esemeny datuma: %d.%d.%d.\n", esemeny.datum.ev, esemeny.datum.ho, esemeny.datum.nap);
        fprintf(file, "Esemeny idopontja: %d:%d\n", esemeny.ido.ora, esemeny.ido.perc);
        fprintf(file, "Esemeny helyszine: %s\n", esemeny.hely);
        fprintf(file, "Esemenyhez tartozo megjegyzes: %s\n", esemeny.megjegyzes);
        fprintf(file, "---------------------------------\n");
    }

    fclose(file);
    return true;
}
/**
 * Az adatbazist .txt fajlbol beolvassa. Ezt a fuggvenyt a program elso menu felsorolassa utan lehet csak valasztani
 * @param adatbazis Az adatbazis, ahova beolvassa az adatokat
 * @return true, ha sikeres, kulonben false
 */
//TODO megcsinalni hogy ne csak az elso menunel lehessen valasztani
bool adatbazis_beolvasasa_fajlbol(Adatbazis *adatbazis) {
    //TODO ezt az egeszet atnezni
    //TODO file -> fajl
    char file_nev[200];
    printf("Add meg a file nevet, amibol be szeretned olvasni az adatbazist: ");

    if (fgets(file_nev, sizeof(file_nev), stdin) == NULL) {
        printf("Hiba tortent a file nev beolvasasa soran!\n");
        return false;
    }
    getchar();

    file_nev[strcspn(file_nev, "\n")] = 0;

    FILE *file = fopen(file_nev, "r");
    if (file == NULL) {
        printf("Sikertelen volt a file megnyitasa!\n");
        return false;
    }

    adatbazis->esemenyek = NULL;
    adatbazis->meret = 0;

    char sor[365];

    while (fgets(sor, sizeof(sor), file) != NULL) {
        if (strncmp(sor, "---------------------------------", 33) == 0) {
            continue;
        }

        if (strncmp(sor + 3, "esemeny:", 8) == 0) {
            Esemeny *uj_memoria = realloc(adatbazis->esemenyek, (adatbazis->meret + 1) * sizeof(Esemeny));
            if (uj_memoria == NULL) {
                printf("Hiba tortent a memoria foglalasa soran!\n");
                fclose(file);
                return false;
            }
            adatbazis->esemenyek = uj_memoria;
            memset(&adatbazis->esemenyek[adatbazis->meret], 0, sizeof(Esemeny));
            continue;
        }

        Esemeny *uj_esemeny = &adatbazis->esemenyek[adatbazis->meret];

        if (strncmp(sor, "Esemeny neve:", 13) == 0) {
            sscanf(sor + 13, " %[^\n]", uj_esemeny->nev);
        } else if (strncmp(sor, "Esemeny datuma:", 15) == 0) {
            char d[DATUMHOSSZ];
            sscanf(sor + 15, " %[^\n]", d);
            if (!datum_ellenorzo(d)) {
                printf("Helytelen datum formatum!\n");
                fclose(file);
                return false;
            }
            char ev[5], ho[3], nap[3];
            substring(d, ev, 0, 4);
            substring(d, ho, 5, 7);
            substring(d, nap, 8, 10);
            uj_esemeny->datum.ev = atoi(ev);
            uj_esemeny->datum.ho = atoi(ho);
            uj_esemeny->datum.nap = atoi(nap);
        } else if (strncmp(sor, "Esemeny idopontja:", 18) == 0) {
            char i[IDOHOSSZ];
            sscanf(sor + 18, " %[^\n]", i);
            if (!ido_ellenorzo(i)) {
                printf("Helytelen az idopont bemenete\n");
                fclose(file);
                return false;
            }
            char ora[3], perc[3];
            substring(i, ora, 0, 2);
            substring(i, perc, 3, 5);
            uj_esemeny->ido.ora = atoi(ora);
            uj_esemeny->ido.perc = atoi(perc);
        } else if (strncmp(sor, "Esemeny helyszine:", 18) == 0) {
            sscanf(sor + 18, " %[^\n]", uj_esemeny->hely);
        } else if (strncmp(sor, "Esemenyhez tartozo megjegyzes:", 29) == 0) {
            sscanf(sor + 29, " %[^\n]", uj_esemeny->megjegyzes);
        }
    }

    fclose(file);
    return true;
}


/**
 * Kezeli a felhasznaloi menut es az opciokat. A felhasznalo a menupontokbol valaszthat, ezt a standard bemeneten adja meg
 * @param adatbazis Az adatbazis
 * @return true, ha folytatodik, false, ha kilepes
 */
bool menu(Adatbazis *adatbazis) {
    int opcio = 0;
    printf("---------------------------------\n");
    printf("1. Rekord hozzaadasa\n");
    printf("2. Rekord torlese\n");
    printf("3. Rekord modositasa\n");
    printf("4. Rekordok kilistazasa\n");
    printf("5. Rekord keresese\n");
    printf("6. Adatbazis kiirasa fajlba\n");
    printf("7. Adatbazis beolvasasa fajlbol\n");
    printf("8. Kilepes\n");

    printf("Adj meg egy menupontot: ");
    scanf("%d", &opcio);
    getchar();

    switch (opcio) {
        case 1:
            rekord_hozzaadasa(adatbazis);
            break;
        case 2:
            rekord_torlese(adatbazis);
            break;
        case 3:
            rekord_modositasa(adatbazis);
            break;
        case 4:
            adatbazis_kilistazasa(adatbazis);
            break;
        case 5:
            rekord_keresese_nev_alapjan(*adatbazis, NULL);
            break;

        case 6:
            adatbazis_fajlba_irasa(*adatbazis);
            break;
        case 7:
            if (adatbazis == NULL) {
                adatbazis_beolvasasa_fajlbol(adatbazis);
            } else {
                //TODO ezt dokumentacioban is leirni
                printf("Ezt csak a program elso lepesekent teheted meg!\n");
            }
            break;
        case 8:
            return false;
        default:
            printf("Helytelen bemenetet adtal meg!");
            return false;
    }
    return true;
}
