#ifndef ESEMENY_KEZELO_H
#define ESEMENY_KEZELO_H
#include <stdbool.h>

#include "strukturak.h"
#include "datum.h"
#include "debugmalloc.h"
#include "seged.h"

void rekord_kiirasa(Esemeny esemeny);

bool rekord_keresese_nev_alapjan(Adatbazis adatbazis, Esemeny **talalatok, int *talalatok_szama);

void rekord_keresese_nap_alapjan(Adatbazis adatbazis);

void rekord_keresese_het_alapjan(Adatbazis adatbazis);

void rekord_keresese_ho_alapjan(Adatbazis adatbazis);

#endif
