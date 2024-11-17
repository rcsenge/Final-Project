#ifndef ESEMENY_H
#define ESEMENY_H

#include "strukturak.h"
#include "esemeny_kezelo.h"


void rekord_hozzaadasa(Adatbazis *adatbazis);
void rekord_torlese(Adatbazis *adatbazis);
void rekord_modositasa(const Adatbazis *adatbazis);
void uj_nev(Esemeny *esemeny);
void uj_datum(Esemeny *esemeny);
void uj_ido(Esemeny *esemeny);
void uj_hely(Esemeny *esemeny);
void uj_megjegyzes(Esemeny *esemeny);

#endif

