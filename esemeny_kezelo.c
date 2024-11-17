#include "esemeny_kezelo.h"
#include "debugmalloc.h"
#include <stdbool.h>
#include <stdio.h>


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