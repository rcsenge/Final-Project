#ifndef ESEMENY_H
#define ESEMENY_H

#include "strukturak.h"
#include "esemeny_kezelo.h"


bool rekord_hozzaadasa(Adatbazis *adatbazis);
bool rekord_torlese(Adatbazis *adatbazis);
bool rekord_modositasa(const Adatbazis *adatbazis);
bool uj_nev(Esemeny *esemeny);
bool uj_datum(Esemeny *esemeny);
bool uj_ido(Esemeny *esemeny);
bool uj_hely(Esemeny *esemeny);
bool uj_megjegyzes(Esemeny *esemeny);

#endif

