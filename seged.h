#ifndef SEGED_H
#define SEGED_H
#include <stdbool.h>

bool egyezo_esemenyek(const Esemeny *esemeny1, const Esemeny *esemeny2);

void substring(char eredeti[], char *cel, int kezdo, int veg);

bool rekord_keresese_idoszak_alapjan(Adatbazis adatbazis, Datum kezdo_datum, Datum veg_datum);

int esemenyek_osszehasonlitasa(Esemeny e1, Esemeny e2);

void rendez(Esemeny *tomb, int eleje, int vege, Esemeny *seged);

void osszefesul(const Esemeny *be, int eleje, int kozepe, int vege, Esemeny *ki);

void masol(const Esemeny *be, int eleje, int vege, Esemeny *ki);

#endif
