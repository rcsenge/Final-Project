#ifndef ADATBAZIS_H
#define ADATBAZIS_H
#include <stdbool.h>

#include "strukturak.h"
#include "seged.h"
#include "esemeny_kezelo.h"
#include "datum.h"
#include "ido.h"

bool adatbazis_kilistazasa(const Adatbazis *adatbazis);
void adatbazis_felszabaditasa(Adatbazis *adatbazis);
bool adatbazis_fajlba_irasa(Adatbazis adatbazis);
bool adatbazis_beolvasasa_fajlbol(Adatbazis **adatbazis);

#endif
