#ifndef ADATBAZIS_H
#define ADATBAZIS_H

#include "Esemeny.h"

typedef struct Adatbazis {
    Esemeny *esemenyek;
    int meret;
} Adatbazis;


void adatbazis_kilistazasa(const Adatbazis *adatbazis);
void adatbazis_felszabaditasa(Adatbazis *adatbazis);
bool adatbazis_fajlba_irasa(Adatbazis adatbazis);
bool adatbazis_beolvasasa_fajlbol(Adatbazis *adatbazis);

#endif
