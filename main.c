#include "debugmalloc.h"
#include "strukturak.h"
#include "esemeny.h"
#include "esemeny_kezelo.h"
#include "adatbazis.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//TODO olvashatobb valtozo nevek
//TODO modulokra bontas
//TODO malloc, realloc ellenorzesek
//TODO reszletesebb dokumentacio
//TODO teszteles

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
