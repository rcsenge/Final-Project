#include "esemeny.h"
#include <stdio.h>
#include "debugmalloc.h"

/**
 * Egy uj esemeny adatait keri be a felhasznalotol a strandard bemeneten, majd elmenti az adatbazisban
 * @param adatbazis Az adatbazis pointere, amihez az uj esemeny hozzaadodik. A fuggveny hivo feladata a memoria felszabaditasa.
 * @return true, ha sikeresen lefutott a fuggveny, egyebkent false
 */
bool rekord_hozzaadasa(Adatbazis *adatbazis) {
    Esemeny esemeny;

    bool sikeres_nev = uj_nev(&esemeny);
    if (sikeres_nev == false) {
        return false;
    }

    bool sikeres_datum = uj_datum(&esemeny);
    if (sikeres_datum == false) {
        free(esemeny.nev);
        return false;
    }

    bool sikeres_ido = uj_ido(&esemeny);
    if (sikeres_ido == false) {
        free(esemeny.nev);
        return false;
    }

    bool sikeres_hely = uj_hely(&esemeny);
    if (sikeres_hely == false) {
        free(esemeny.nev);
        return false;
    }

    bool sikeres_megjegyzes = uj_megjegyzes(&esemeny);
    if (sikeres_megjegyzes == false) {
        free(esemeny.nev);
        free(esemeny.hely);
        return false;
    }
    adatbazis->esemenyek = (Esemeny *) realloc(adatbazis->esemenyek, (adatbazis->meret + 1) * sizeof(Esemeny));
    if (adatbazis->esemenyek == NULL) {
        printf("Hiba tortent a memoria foglalasa soran!\n");
        return false;
    }

    adatbazis->esemenyek[adatbazis->meret] = esemeny;
    adatbazis->meret++;
    printf("Uj esemeny hozaadasa sikeres!\n");
    printf("---------------------------------\n");
    return true;
}

/**
* Torli az adott nevvel rendelkezo esemenyt az adatbazisbol
* A fuggveny megkeresi a felhasznalo altal megadott nevu esemenyt, majd a nevvel egyezo nevu esemenyeket sorszamozva kilistazza
* Ezutan a felhasznalo megad egy sorszamot, ez a rekord torlodik
*
* @param adatbazis Egy pointer az Adatbazis strukturara, amiben egy esemeny torleset vegezzuk
* @return true, ha sikeresen lefutott a fuggveny, egyebkent false
**/
bool rekord_torlese(Adatbazis *adatbazis) {
    Esemeny *talalatok = NULL;
    int *talalatok_szama = (int *) malloc(sizeof(int));
    int torlendo_talalat_index;

    printf("Add meg az esemeny nevet, amit torolni szeretnel: ");
    bool sikeres_talalat = rekord_keresese_nev_alapjan(*adatbazis, &talalatok, talalatok_szama);

    if (sikeres_talalat == false) {
        free(talalatok);
        free(talalatok_szama);
        return false;
    }

    printf("Hanyadik sorszamu esemenyt szeretned torolni? ");
    if (scanf("%d", &torlendo_talalat_index) < 1) {
        printf("Sikertelen volt a beolvasas!\n");
        free(talalatok);
        free(talalatok_szama);
        return false;
    }
    getchar();

    if (torlendo_talalat_index < 1 || torlendo_talalat_index > *talalatok_szama) {
        printf("Helytelen sorszamot adtal meg!\n");
        free(talalatok);
        free(talalatok_szama);
        return false;
    }


    int torlendo_adatbazis_index = 0;
    for (int i = 0; i < adatbazis->meret; ++i) {
        if (egyezo_esemenyek(&adatbazis->esemenyek[i], &talalatok[torlendo_talalat_index - 1])) {
            torlendo_adatbazis_index = i;
            free(adatbazis->esemenyek[i].nev);
            free(adatbazis->esemenyek[i].hely);
            free(adatbazis->esemenyek[i].megjegyzes);
            break;
        }
    }

    for (int i = torlendo_adatbazis_index; i < adatbazis->meret - 1; ++i) {
        adatbazis->esemenyek[i] = adatbazis->esemenyek[i + 1];
    }

    adatbazis->meret--;
    adatbazis->esemenyek = (Esemeny *) realloc(adatbazis->esemenyek, adatbazis->meret * sizeof(Esemeny));
    if (adatbazis->esemenyek == NULL) {
        printf("Hiba tortent a memoria atmeretezese soran!\n");
        free(talalatok);
        free(talalatok_szama);
        return false;
    }

    printf("Esemeny torolve.\n");

    free(talalatok);
    free(talalatok_szama);
    return true;
}

/**
 * Modosit egy adott esemenyt a felhasznalo valasztasa alapjan.
 * A felhasznalonak az esemeny nevet kell megadja, a nevben egyezo esemenyek sorszamozva kiiratasra kerulnek
 * A felhasznalo megad egy sorszamot, ez a rekord lesz modositva
 * Ezutan a rekord mezoinek elnevezesei kerul kilistazasra, ezek kozul a felhasznalonak kell valasztania, hogy melyiket kivanja modositai
 * A valasztott mezo uj erteket beirja a felhasznalo es modosul az esemeny
 * @param adatbazis Az adatbazis, amely az esemenyt tartalmazza
 * @return true, ha sikeresen lefutott a fuggveny, egyebkent false
 */
bool rekord_modositasa(const Adatbazis *adatbazis) {
    Esemeny *talalatok = NULL;
    int *talalatok_szama = (int *) malloc(sizeof(int));

    int talalat_index;

    printf("Add meg az esemeny nevet, amit modositani szeretnel: ");
    bool sikeres_talalat = rekord_keresese_nev_alapjan(*adatbazis, &talalatok, talalatok_szama);

    if (sikeres_talalat == false) {
        free(talalatok);
        free(talalatok_szama);
        return false;
    }

    printf("Hanyadik sorszamut szeretned modositani? ");
    if (scanf("%d", &talalat_index) < 1) {
        printf("Sikertelen volt a beolvasas!\n");
        free(talalatok);
        free(talalatok_szama);

        return false;
    }
    getchar();

    if (talalat_index < 1 || talalat_index > *talalatok_szama) {
        printf("Helytelen sorszamot adtal meg!\n");
        free(talalatok);
        free(talalatok_szama);
        return false;
    }

    int ertek_index;
    printf("Melyik erteket szeretned modositani?\n");
    printf("1. Esemeny neve\n");
    printf("2. Esemeny datuma\n");
    printf("3. Esemeny idopontja\n");
    printf("4. Esemeny helyszine\n");
    printf("5. Esemeny megjegyzese\n");
    printf("Hanyadik sorszamut szeretned modositani? ");

    if (scanf("%d", &ertek_index) < 1) {
        printf("Sikertelen volt a beolvasas!\n");
        free(talalatok);
        free(talalatok_szama);
        return false;
    }
    getchar();

    if (ertek_index < 1 || ertek_index > 5) {
        printf("Helytelen sorszamot adtal meg!\n");
        free(talalatok);
        free(talalatok_szama);
        return false;
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
        free(talalatok);
        free(talalatok_szama);
        return false;
    }

    free(talalatok);
    free(talalatok_szama);

    switch (ertek_index) {
        case 1: {
            free(adatbazisban_levo_esemeny->nev);
            bool sikeres_uj_nev = uj_nev(adatbazisban_levo_esemeny);
            if (sikeres_uj_nev == false) {
                return false;
            }
            break;
        }
        case 2: {
            bool sikeres_uj_datum = uj_datum(adatbazisban_levo_esemeny);
            if (sikeres_uj_datum == false) {
                return false;
            }
            break;
        }
        case 3: {
            bool sikeres_uj_ido = uj_ido(adatbazisban_levo_esemeny);
            if (sikeres_uj_ido == false) {
                return false;
            }
            break;
        }
        case 4: {
            free(adatbazisban_levo_esemeny->hely);
            bool sikeres_uj_hely = uj_hely(adatbazisban_levo_esemeny);
            if (sikeres_uj_hely == false) {
                return false;
            }
            break;
        }
        case 5: {
            free(adatbazisban_levo_esemeny->megjegyzes);
            bool sikeres_uj_megjegyzes = uj_megjegyzes(adatbazisban_levo_esemeny);
            if (sikeres_uj_megjegyzes == false) {
                return false;
            }
            break;
        }
        default:
            printf("Helytelen bemenetet adtal meg!\n");
            return false;
    }

    printf("Sikeresen modosult az esemeny!\n");
    return true;
}

/**
* Beker a felhasznalotol a standard bemeneten egy nevet, majd ezt eltarolja egy esemenyben
 * @param esemeny Az esemeny, ahova az uj nevet meg kell adni
 * @return true, ha sikeresen lefutott a fuggveny, egyebkent false
 */
bool uj_nev(Esemeny *esemeny) {
    printf("Adj meg egy nevet: ");

    char *nev = (char *) malloc(1 * sizeof(char));
    if (nev == NULL) {
        printf("Hiba tortent a memoria foglalasa soran!\n");
        return false;
    }

    int hossz = 0;
    char c;
    while ((c = getchar()) != '\n') {
        nev[hossz] = c;
        hossz++;

        nev = (char *) realloc(nev, (hossz + 1) * sizeof(char));
        if (nev == NULL) {
            printf("Hiba tortent a memoria atmeretezese soran!\n");
            return false;
        }
    }
    nev[hossz] = '\0';

    esemeny->nev = nev;
    return true;
}

/**
 * Beker a felhasznalotol a standard bemeneten egy datumot, majd ezt eltarolja egy esemenyben
 * @param esemeny Az esemeny, ahova az uj datumot meg kell adni
 * @return true, ha sikeresen lefutott a fuggveny, egyebkent false
 */
bool uj_datum(Esemeny *esemeny) {
    char *datum = (char *) malloc(1 * sizeof(char));
    printf("Adj meg egy datumot az esemenyednek (EEEE. HH. NN.): ");

    Datum d = datum_beolvasas(datum);
    Datum ervenytelen = {0, 0, 0};
    if (datumok_osszehasonlitasa(d, ervenytelen) == 0) {
        printf("Hiba tortent a datum beolvasasa soran!\n");
        return false;
    }
    esemeny->datum = d;
    return true;
}

/**
 * Beker a felhasznalotol a standard bemeneten egy idopontot, majd ezt eltarolja egy esemenyben
 * @param esemeny Az esemeny, ahova az uj datumot meg kell adni
* @return true, ha sikeresen lefutott a fuggveny, egyebkent false
 */
bool uj_ido(Esemeny *esemeny) {
    char *ido = (char *) malloc(1 * sizeof(char));

    if (ido == NULL) {
        printf("Hiba tortent a memoria foglalasa soran!\n");
        return false;
    }
    printf("Adj meg egy idopontot (OO:PP): ");

    int hossz = 0;
    char c;
    while ((c = getchar()) != '\n') {
        ido[hossz] = c;
        hossz++;

        ido = (char *) realloc(ido, (hossz + 1) * sizeof(char));
        if (ido == NULL) {
            printf("Hiba tortent a memoria atmeretezese soran!\n");
            free(ido);
            return false;
        }
    }
    ido[hossz] = '\0';

    if (!ido_ellenorzo(ido)) {
        printf("Helytelen az idopont bemenete\n");
        free(ido);
        return false;
    }

    char ora[3], perc[3];

    substring(ido, ora, 0, 1);
    substring(ido, perc, 3, 4);
    free(ido);

    int ora_int = atoi(ora);
    int perc_int = atoi(perc);

    Ido i = {ora_int, perc_int};
    esemeny->ido = i;
    return true;
}

/**
 * Beker a felhasznalotol a standard bemeneten egy helyszint, majd ezt eltarolja egy esemenyben
 * @param esemeny Az esemeny, ahova az uj helyszint meg kell adni
* @return true, ha sikeresen lefutott a fuggveny, egyebkent false
 */
bool uj_hely(Esemeny *esemeny) {
    char *hely = (char *) malloc(1 * sizeof(char));
    if (hely == NULL) {
        printf("Hiba tortent a memoria foglalasa soran!\n");
        return false;
    }

    printf("Adj meg egy helyet az esemenyednek: ");

    int hossz = 0;
    char c;
    while ((c = getchar()) != '\n') {
        hely[hossz] = c;
        hossz++;

        hely = (char *) realloc(hely, (hossz + 1) * sizeof(char));
        if (hely == NULL) {
            printf("Hiba tortent a memoria atmeretezese soran!\n");
            return false;
        }
    }
    hely[hossz] = '\0';

    esemeny->hely = (char *) malloc(hossz + 1 * sizeof(char));
    if (esemeny->hely == NULL) {
        printf("Hiba tortent a memoria foglalasa soran!\n");
    }
    strcpy(esemeny->hely, hely);
    free(hely);
    return true;
}

/**
 * Beker a felhasznalotol a standard bemeneten egy megjegyzest, majd ezt eltarolja egy esemenyben
 * @param esemeny Az esemeny, ahova az uj megjegyzest meg kell adni
 * @return true, ha sikeresen lefutott a fuggveny, egyebkent false
 */
bool uj_megjegyzes(Esemeny *esemeny) {
    char *megjegyzes = (char *) malloc(1 * sizeof(char));
    if (megjegyzes == NULL) {
        printf("Hiba tortent a memoria foglalasa soran!\n");
        return false;
    }
    printf("Adj meg egy megjegyzest az esemenyednek: ");

    int hossz = 0;
    char c;
    while ((c = getchar()) != '\n') {
        megjegyzes[hossz] = c;
        hossz++;

        megjegyzes = (char *) realloc(megjegyzes, (hossz + 1) * sizeof(char));
        if (megjegyzes == NULL) {
            printf("Hiba tortent a memoria atmeretezese soran!\n");
            return false;;
        }
    }
    megjegyzes[hossz] = '\0';

    esemeny->megjegyzes = (char *) malloc(hossz + 1 * sizeof(char));
    if (esemeny->megjegyzes == NULL) {
        printf("Hiba tortent a memoria foglalasa soran!\n");
        return false;
    }

    strcpy(esemeny->megjegyzes, megjegyzes);
    free(megjegyzes);
    return true;
}
