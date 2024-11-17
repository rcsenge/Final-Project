#include "esemeny.h"

#include <stdio.h>

#include "debugmalloc.h"


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
    scanf("%d", &ertek_index); //TODO ide

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
 * Beker a felhasznalotol a standard bemeneten egy datumot, majd ezt eltarolja egy esemenyben
 * @param esemeny Az esemeny, ahova az uj datumot meg kell adni
 */
void uj_datum(Esemeny *esemeny) {
    char *datum = malloc(1 * sizeof(char));
    printf("Adj meg egy datumot az esemenyednek (EEEE. HH. NN.): ");

    Datum d = datum_beolvasas(datum);
    esemeny->datum = d;
}

/**
 * Beker a felhasznalotol a standard bemeneten egy idopontot, majd ezt eltarolja egy esemenyben
 * @param esemeny Az esemeny, ahova az uj datumot meg kell adni
 */
void uj_ido(Esemeny *esemeny) {
    char *ido = malloc(1 * sizeof(char));
    if (ido == NULL) {
        printf("Hiba tortent a memoria foglalasa soran!\n");
        return;
    }
    printf("Adj meg egy idopontot: ");

    int hossz = 0;
    char c;
    while ((c = getchar()) != '\n') {
        ido[hossz] = c;
        hossz++;

        ido = realloc(ido, (hossz + 1) * sizeof(char));
        if (ido == NULL) {
            printf("Hiba tortent a memoria atmeretezese soran!\n");
            free(ido);
            return;
        }
    }
    ido[hossz] = '\0';

    if (!ido_ellenorzo(ido)) {
        printf("Helytelen az idopont bemenete");
        return;
    }

    char ora[3], perc[3];

    substring(ido, ora, 0, 1);
    substring(ido, perc, 3, 4);
    free(ido);

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
