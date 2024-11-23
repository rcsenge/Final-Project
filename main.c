#include "strukturak.h"
#include "debugmalloc.h"
#include "esemeny.h"
#include "esemeny_kezelo.h"
#include "adatbazis.h"

#include <stdbool.h>
#include <stdio.h>

bool menu(Adatbazis *adatbazis);

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
    if (scanf("%d", &opcio) < 1) {
        printf("Sikertelen volt a beolvasas!\n");
        return false;
    }
    getchar();
    printf("---------------------------------\n");

    switch (opcio) {
        case 1: {
            bool sikeres_hozzaadas = rekord_hozzaadasa(adatbazis);
            if (sikeres_hozzaadas == false) {
                printf("Sikertelen volt az uj rekord hozzaadasa!\n");
            }
            break;
        }
        case 2: {
            bool sikeres_torles = rekord_torlese(adatbazis);
            if (sikeres_torles == false) {
                printf("Sikertelen volt a rekord torlese!\n");
            }
            break;
        }
        case 3: {
            bool sikeres_modositas = rekord_modositasa(adatbazis);
            if (sikeres_modositas == false) {
                printf("Sikertelen volt a rekord modositasa!\n");
            }
            break;
        }
        case 4: {
            bool sikeres_kilistazas = adatbazis_kilistazasa(adatbazis);
            if (sikeres_kilistazas == false) {
                printf("Sikertelen volt a rekordok kiiratasa!\n");
            }
            break;
        }
        case 5: {
            Esemeny *talalatok = NULL;
            int *talalatok_szama = (int *) malloc(sizeof(int));

            printf("Add meg a keresett nevet: ");
            bool sikeres_kereses = rekord_keresese_nev_alapjan(*adatbazis, &talalatok, talalatok_szama);

            free(talalatok);
            free(talalatok_szama);

            if (sikeres_kereses == false) {
                printf("Sikertelen volt a rekord keresese!\n");
            }

            break;
        }
        case 6: {
            bool sikeres_fajlba_iras = adatbazis_fajlba_irasa(*adatbazis);
            if (sikeres_fajlba_iras == false) {
                printf("Sikertelen volt az adatbazis fajlba irasa!\n");
            }
            break;
        }
        case 7: {
            if (adatbazis->esemenyek == NULL) {
                bool sikeres_adatbazis_beolvasas = adatbazis_beolvasasa_fajlbol(&adatbazis);
                if (sikeres_adatbazis_beolvasas == false) {
                    printf("Sikertelen volt az adatbazis beolvasasa!\n");
                } else {
                    printf("Sikeres volt az adatbazis beolvasasa!\n");
                    for (int i = 0; i < adatbazis->meret; ++i) {
                        rekord_kiirasa(adatbazis->esemenyek[i]);
                    }
                }
            } else {
                printf("Ezt csak a program elso lepesekent teheted meg!\n");
            }
            break;
        }
        case 8:
            return false;
        default:
            printf("Helytelen bemenetet adtal meg!");
            return false;
    }
    return true;
}
