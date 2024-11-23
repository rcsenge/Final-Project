#include "seged.h"
#include "debugmalloc.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "datum.h"

/**
* Egy tomb resztombjet masolja at egy celtombbe
 * @param eredeti az eredeti tomb, amelybol egy résztömböt szeretnénk kimásolni
 * @param cel a céltömb pointere, ahova a résztömb kerül
 * @param kezdo a kezdőindex az eredeti tömbben, ahonnan a másolást kezdjük (az indexelés 0-tól kezdődik)
 * @param veg a végindex az eredeti tömbben, ameddig a másolás tart, ez még belekerül a résztömbbe
 */
void substring(char eredeti[], char *cel, int kezdo, int veg) {
    if (kezdo > veg || kezdo < 0 || veg >= strlen(eredeti)) {
        printf("Helytelen bemenetek!\n");
    }

    int cel_hossz = veg - kezdo + 1;
    for (int i = 0; i < cel_hossz; ++i) {
        cel[i] = eredeti[kezdo + i];
    }

    cel[cel_hossz] = '\0';
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
 * Kereses idoszak alapjan, esemenyeket talal meg es rendez
 * @param adatbazis Az adatbazis, amely az esemenyeket tartalmazza
 * @param kezdo_datum A keresesi idoszak kezdete
 * @param veg_datum A keresesi idoszak vege
 * @return true, ha talal esemenyt
 *         false, ha nem talal
 */
bool rekord_keresese_idoszak_alapjan(Adatbazis adatbazis, Datum kezdo_datum, Datum veg_datum) {
    int max_talalat = 3;
    Esemeny *talalatok_tomb = (Esemeny *) malloc(max_talalat * sizeof(Esemeny));
    if (talalatok_tomb == NULL) {
        printf("Hiba tortent a memoria foglalasa soran!\n");
        free(talalatok_tomb);
        return false;
    }

    int talalat_szam = 0;
    for (int i = 0; i < adatbazis.meret; ++i) {
        if (datumok_osszehasonlitasa(kezdo_datum, adatbazis.esemenyek[i].datum) <= 0
            && datumok_osszehasonlitasa(veg_datum, adatbazis.esemenyek[i].datum) >= 0) {
            if (talalat_szam >= max_talalat) {
                max_talalat *= 2;
                talalatok_tomb = (Esemeny *) realloc(talalatok_tomb, max_talalat * sizeof(Esemeny));

                if (talalatok_tomb == NULL) {
                    printf("Hiba tortent a memoria foglalasa soran!\n");
                    free(talalatok_tomb);
                    return false;
                }
            }
            talalatok_tomb[talalat_szam++] = adatbazis.esemenyek[i];
        }
    }

    Esemeny *seged = (Esemeny *) malloc(talalat_szam * sizeof(Esemeny));
    if (seged == NULL) {
        printf("Hiba tortent a memoria foglalas soran!\n");
    }
    rendez(talalatok_tomb, 0, talalat_szam, seged);
    free(seged);

    if (talalat_szam > 0) {
        printf("============Talalatok============\n");
        printf("---------------------------------\n");
        for (int i = 0; i < talalat_szam; ++i) {
            printf("%d. esemeny: \n", i + 1);
            rekord_kiirasa(talalatok_tomb[i]);
            printf("---------------------------------\n");
        }
    } else {
        printf("Nincs talalat.\n");
        free(talalatok_tomb);
        return false;
    }

    free(talalatok_tomb);
    return true;
}

/**
 * Ket esemenyt hasonlit ossze datum es ido alapjan
 * @param e1 Az elso esemeny
 * @param e2 A masodik esemeny
 * @return Pozitiv, ha e1 kesobbi, negativ ha korabbi, 0 ha egyenloek
 */
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
