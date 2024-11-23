#include "adatbazis.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debugmalloc.h"
#include "esemeny.h"

/**
 * Az adatbazist egy felhasznalo altal megadott fajlba irja, ez a projektmappaban vagy a fajl eleresi utvonalan jelenik meg.
 * Ha a fajl nevvel egyezo fajl mar letezik, akkor azt felulirja, ha nem akkor letrehozza.
 * A fajlnak .txt kiterjesztesunek kell lennie.
 * @param adatbazis Az adatbazis
 * @return true, ha sikeres, kulonben false
 */
bool adatbazis_fajlba_irasa(Adatbazis adatbazis) {
    printf("Add meg a fajl nevet/eleresi utvonalat! (txt kiterjesztesu legyen): ");

    char *fajl_nev = (char *) malloc(1 * sizeof(char));
    if (fajl_nev == NULL) {
        printf("Hiba tortent a memoria foglalasa soran!\n");
        return false;
    }

    int hossz = 0;
    char c;
    while ((c = getchar()) != '\n') {
        fajl_nev[hossz] = c;
        hossz++;

        fajl_nev = (char *) realloc(fajl_nev, (hossz + 1) * sizeof(char));
        if (fajl_nev == NULL) {
            printf("Hiba tortent a memoria atmeretezese soran!\n");
            return false;
        }
    }
    fajl_nev[hossz] = '\0';

    char fajl_nev_vege[4];
    substring(fajl_nev, fajl_nev_vege, hossz - 4, hossz - 1);
    if (strcmp(fajl_nev_vege, ".txt") != 0) {
        printf("Helytelen fajl nevet adtal meg!\n");
        free(fajl_nev);
        return false;
    }

    FILE *file = fopen(fajl_nev, "w");
    if (file == NULL) {
        printf("Sikertelen volt a file megnyitasa!\n");
        return false;
    }

    fprintf(file, "---------------------------------\n");
    for (int i = 0; i < adatbazis.meret; ++i) {
        Esemeny esemeny = adatbazis.esemenyek[i];
        fprintf(file, "%d. esemeny: \n", i + 1);
        fprintf(file, "Esemeny neve: %s\n", esemeny.nev);
        fprintf(file, "Esemeny datuma: %d. %02d. %02d.\n", esemeny.datum.ev, esemeny.datum.ho, esemeny.datum.nap);
        fprintf(file, "Esemeny idopontja: %02d:%02d\n", esemeny.ido.ora, esemeny.ido.perc);
        fprintf(file, "Esemeny helyszine: %s\n", esemeny.hely);
        fprintf(file, "Esemenyhez tartozo megjegyzes: %s\n", esemeny.megjegyzes);
        fprintf(file, "---------------------------------\n");
    }

    fclose(file);
    printf("Sikeresen megtortent a fajlba iras!\n");
    return true;
}

/**
 * Kilistazza az adatbazis esemenyeit a valasztott idoszak (nap, het, honap) alapjan.
 * A megadott naphoz kepest szamitja a kovetkezo egy hetet vagy honapot.
 * Pl: megadott nap: 2024. 11. 24. megadott idoszak: het -> 2024. 11. 23. es 2024. 11. 30. kozott listazza ki az esemenyeket.
 * @param adatbazis Az adatbazis, amely az esemenyeket tartalmazza.
 */
bool adatbazis_kilistazasa(const Adatbazis *adatbazis) {
    int idoszak;
    printf("Melyik idoszak esemenyeit szeretned kilistazni?\n");
    printf("1. Egy adott nap\n");
    printf("2. Egy adott het\n");
    printf("3. Egy adott honap\n");

    printf("Add meg a felsorolt sorszamok egyiket: ");
    if (scanf("%d", &idoszak) < 1) {
        printf("Sikertelen volt a beolvasas!\n");
        return false;
    }
    getchar();

    if (idoszak < 1 || idoszak > 3) {
        printf("Helytelen bemenetet adtal meg!\n");
        return false;
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
            return false;
    }
    return true;
}


/**
 * Az adatbazist .txt fajlbol beolvassa. Ezt a fuggvenyt a program elso menu felsorolassa utan lehet csak valasztani.
 * @param adatbazis Az adatbazis, ahova beolvassa az adatokat
 * @return true, ha sikeres, kulonben false
 */
bool adatbazis_beolvasasa_fajlbol(Adatbazis **adatbazis) {
    Esemeny *temp_esemenyek = NULL;
    int temp_esemenyek_meret = 0;

    printf("Adj meg a nevet/eleresi utvonalat a fajlnak amit be szeretnel olvasni! (txt kiterjesztesu legyen): ");

    char *fajl_nev = (char *) malloc(1 * sizeof(char));
    if (fajl_nev == NULL) {
        printf("Hiba tortent a memoria foglalasa soran!\n");
        return false;
    }

    int hossz = 0;
    char c;
    while ((c = getchar()) != '\n') {
        fajl_nev[hossz] = c;
        hossz++;

        fajl_nev = (char *) realloc(fajl_nev, (hossz + 1) * sizeof(char));
        if (fajl_nev == NULL) {
            printf("Hiba tortent a memoria atmeretezese soran!\n");
            return false;
        }
    }
    fajl_nev[hossz] = '\0';

    char fajl_nev_vege[4];
    substring(fajl_nev, fajl_nev_vege, hossz - 4, hossz - 1);
    if (strcmp(fajl_nev_vege, ".txt") != 0) {
        printf("Helytelen fajl nevet adtal meg!\n");
        free(fajl_nev);
        return false;
    }

    FILE *fajl = fopen(fajl_nev, "r");
    if (fajl == NULL) {
        printf("Sikertelen volt a file megnyitasa!\n");
        free(fajl_nev);
        return false;
    }

    char sor[365];

    while (fgets(sor, sizeof(sor), fajl) != NULL) {
        if (strncmp(sor, "---------------------------------", 33) == 0) {
            continue;
        }

        if (strncmp(sor + 3, "esemeny:", 8) == 0) {
            temp_esemenyek = (Esemeny *) realloc(temp_esemenyek, (temp_esemenyek_meret + 1) * sizeof(Esemeny));
            if (temp_esemenyek == NULL) {
                printf("Hiba tortent a memoria atmeretezese soran!\n");
                fclose(fajl);
                free(fajl_nev);
                return false;
            }

            temp_esemenyek_meret++;
            continue;
        }

        Esemeny *uj_esemeny = &temp_esemenyek[temp_esemenyek_meret - 1];

        if (strncmp(sor, "Esemeny neve:", 13) == 0) {
            char temp[256];

            if (sscanf(sor + 13, " %[^\n]", temp) == 1) {
                uj_esemeny->nev = (char *) malloc(strlen(temp) + 1);

                if (uj_esemeny->nev == NULL) {
                    printf("Sikertelen volt a memoria lefoglalasa!\n");
                    fclose(fajl);
                    free(fajl_nev);
                    return false;
                }
                strcpy(uj_esemeny->nev, temp);
            } else {
                printf("Nem sikerult beolvasni az esemeny nevet!\n");
                fclose(fajl);
                free(fajl_nev);
                return false;
            }
        } else if (strncmp(sor, "Esemeny datuma:", 15) == 0) {
            char d[DATUMHOSSZ];

            sscanf(sor + 15, " %[^\n]", d);
            if (!datum_ellenorzo(d)) {
                printf("Helytelen datum formatum!\n");
                fclose(fajl);
                free(fajl_nev);
                return false;
            }
            char ev[5], ho[3], nap[3];
            substring(d, ev, 0, 3);
            substring(d, ho, 6, 7);
            substring(d, nap, 10, 11);
            uj_esemeny->datum.ev = atoi(ev);
            uj_esemeny->datum.ho = atoi(ho);
            uj_esemeny->datum.nap = atoi(nap);
        } else if (strncmp(sor, "Esemeny idopontja:", 18) == 0) {
            char i[6];
            sscanf(sor + 18, " %[^\n]", i);
            if (!ido_ellenorzo(i)) {
                printf("Helytelen az idopont bemenete\n");
                fclose(fajl);
                free(fajl_nev);
                return false;
            }
            char ora[3], perc[3];
            substring(i, ora, 0, 1);
            substring(i, perc, 3, 4);
            uj_esemeny->ido.ora = atoi(ora);
            uj_esemeny->ido.perc = atoi(perc);
        } else if (strncmp(sor, "Esemeny helyszine:", 18) == 0) {
            char temp[256];

            if (sscanf(sor + 18, " %[^\n]", temp) == 1) {
                uj_esemeny->hely = (char *) malloc(strlen(temp) + 1);

                if (uj_esemeny->hely == NULL) {
                    printf("Sikertelen volt a memoria lefoglalasa!\n");
                    fclose(fajl);
                    free(fajl_nev);
                    return false;
                }
                strcpy(uj_esemeny->hely, temp);
            } else {
                printf("Nem sikerult beolvasni az esemeny helyszinet!\n");
                fclose(fajl);
                free(fajl_nev);
                return false;
            }
        } else if (strncmp(sor, "Esemenyhez tartozo megjegyzes:", 30) == 0) {
            char temp[256];

            if (sscanf(sor + 30, " %[^\n]", temp) == 1) {
                uj_esemeny->megjegyzes = (char *) malloc(strlen(temp) + 1);

                if (uj_esemeny->megjegyzes == NULL) {
                    printf("Sikertelen volt a memoria lefoglalasa!\n");
                    fclose(fajl);
                    free(fajl_nev);
                    return false;
                }
                strcpy(uj_esemeny->megjegyzes, temp);
            } else {
                printf("Nem sikerult beolvasni az esemeny megjegyzeset!\n");
                fclose(fajl);
                free(fajl_nev);
                return false;
            }
        }
    }

    fclose(fajl);
    free(fajl_nev);

    (*adatbazis)->esemenyek = temp_esemenyek;
    (*adatbazis)->meret = temp_esemenyek_meret;

    return true;
}


/**
 * A megadott adatbazis esemeny tombjenek, valamint az adatbazisnak a felszabaditasa
 * @param adatbazis Az adatbazis, ami felszabaditasra kerul
 */
void adatbazis_felszabaditasa(Adatbazis *adatbazis) {
    for (int i = 0; i < adatbazis->meret; ++i) {
        if (adatbazis->esemenyek[i].nev != NULL) {
            free(adatbazis->esemenyek[i].nev);
        }
        if (adatbazis->esemenyek[i].hely != NULL) {
            free(adatbazis->esemenyek[i].hely);
        }
        if (adatbazis->esemenyek[i].megjegyzes != NULL) {
            free(adatbazis->esemenyek[i].megjegyzes);
        }
    }
    if (adatbazis->esemenyek != NULL) {
        free(adatbazis->esemenyek);
        adatbazis->esemenyek = NULL;
    }
}
