#include "ido.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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