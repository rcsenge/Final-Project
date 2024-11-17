#include "adatbazis.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debugmalloc.h"

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