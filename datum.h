#ifndef DATUM_H
#define DATUM_H
#define DATUMHOSSZ 13
#include <stdbool.h>
#include "strukturak.h"

bool datum_ellenorzo(char datum[]);

Datum datum_beolvasas(char *datum);

bool szokoev(int ev);

int napok_szama_honapban(int ho, int ev);

int datumok_osszehasonlitasa(Datum d1, Datum d2);

Datum datum_hozzaado(Datum d, int nap);
#endif
