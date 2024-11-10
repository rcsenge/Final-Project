#ifndef ESEMENY_KEZELO_H
#define ESEMENY_KEZELO_H
#include <stdbool.h>

void rekord_kiirasa(Esemeny esemeny);

bool rekord_keresese_nev_alapjan(Adatbazis adatbazis, Esemeny **talalatok);

void rekord_keresese_nap_alapjan(Adatbazis adatbazis);

void rekord_keresese_het_alapjan(Adatbazis adatbazis);

void rekord_keresese_ho_alapjan(Adatbazis adatbazis);

#endif
