#ifndef TRANSPORTEURBANO_DEFINICIONES_H
#define TRANSPORTEURBANO_DEFINICIONES_H

#include <vector>
#include <tuple>
#include <cmath>

using namespace std;

#define T0              1651596890.0

// DEFINICIONES DE TIPO
typedef double velocidad;
typedef double tiempo;
typedef double distancia;
typedef tuple<double,double> gps;
typedef vector<gps> recorrido;
typedef vector<tuple<tiempo, gps> > viaje;
typedef tuple<tiempo, gps> elementoDeViaje;
typedef tuple<int,int> nombre;
typedef tuple<gps,gps,nombre> celda;
typedef vector<celda> grilla;

#endif //TRANSPORTEURBANO_DEFINICIONES_H