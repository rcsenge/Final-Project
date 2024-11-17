#include "datum.h"
#include "debugmalloc.h"

#include "seged.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
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
    if (datum == NULL) {
        printf("Hiba tortent a memoria foglalasa soran!\n");
        free(datum);
        Datum d = {0, 0, 0};
        return d;
    }

    int hossz = 0;
    char c;
    while ((c = getchar()) != '\n') {
        datum[hossz] = c;
        hossz++;

        datum = realloc(datum, (hossz + 1) * sizeof(char));
        if (datum == NULL) {
            printf("Hiba tortent a memoria atmeretezese soran!\n");
            free(datum);
            Datum d = {0, 0, 0};
            return d;
        }
    }
    datum[hossz] = '\0';

    if (!datum_ellenorzo(datum)) {
        printf("Helytelen datum formatum!\n");
        //TODO ezt ellenorizni a fgv hivasnal
        free(datum);
        Datum d = {0, 0, 0};
        return d;
    }

    char ev[5], ho[3], nap[3];

    substring(datum, ev, 0, 3);
    substring(datum, ho, 6, 7);
    substring(datum, nap, 10, 11);

    free(datum);
    int ev_int = atoi(ev);
    int ho_int = atoi(ho);
    int nap_int = atoi(nap);

    Datum d = {ev_int, ho_int, nap_int};
    return d;
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