#ifndef STRUKTURAK_H
#define STRUKTURAK_H

typedef struct Ido {
    int ora, perc;
} Ido;

typedef struct Datum {
    int ev, ho, nap;
} Datum;

typedef struct Esemeny {
    char *nev;
    Datum datum;
    Ido ido;
    char *hely;
    char *megjegyzes;
} Esemeny;

typedef struct Adatbazis {
    Esemeny* esemenyek;
    int meret;
} Adatbazis;

#endif
