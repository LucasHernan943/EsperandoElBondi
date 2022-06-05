#include "ejercicios.h"
#include "auxiliares.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <iomanip>

using namespace std;

/******++++**************************** EJERCICIO tiempoTotal ***********+++***********************/
tiempo tiempoTotal(viaje v) {
    tiempo t=0;
    tiempo tmin=minTiempoViaje(v);
    tiempo tmax=maxTiempoViaje(v);
    t=tmax-tmin;
    return t;
}

/************++*********************** EJERCICIO distanciaTotal ************++*********************/
distancia distanciaTotal(viaje v) {
    distancia d=0;
    ordenarViaje(v);
    d=distanciaViaje(v);
    return d;
}

/*****************************+***** EJERCICIO excesoDeVelocidad **********************************/
bool excesoDeVelocidad(viaje v) {
    bool resp = false;
    ordenarViaje(v);
    for(int i=0; i< v.size()-1; i=i+1){
        if(vel(v[i], v[i+1])>80){
            resp=true;
        }
    }
    return resp;
}

/************************************ EJERCICIO recorridoCubierto *******************************/

vector<gps> recorridoNoCubierto(viaje v, recorrido r, distancia u) {
    vector<gps> resp;
    bool foundCoverer;
    double distanciaEntreAmbos;
    for (int i=0; i<r.size();i++) {

        foundCoverer = false;

        for (int j = 0; j < v.size(); j++) {
            auto a=obtenerPosicion(v[j]);
            distanciaEntreAmbos = distEnKM( r[i],obtenerPosicion(v[j]));
            if (distanciaEntreAmbos < u) {
                foundCoverer = true;
                break;
            }
        }
        if (not foundCoverer)
            resp.push_back(r[i]);
    }

    return resp;
}

/***************************************** EJERCICIO flota ***************************************/

/** auxiliar **/

vector<double> tiemposMinYMax(viaje v) {
    double tiempoMin = obtenerTiempo(v[0]);
    double tiempoMax = obtenerTiempo(v[0]);
    for(int i = 0; i < v.size(); ++i) {
        if(tiempoMin > obtenerTiempo(v[i])) {
            tiempoMin = obtenerTiempo(v[i]);
        }
        if(tiempoMax < obtenerTiempo(v[i])) {
            tiempoMax = obtenerTiempo(v[i]);
        }
    }
    vector<double> tiemposMinYMax{tiempoMin,tiempoMax};

    return tiemposMinYMax;
}

/** auxiliar **/

int flota(vector<viaje> f, tiempo t0, tiempo tf) {
    int resp = 0;
    vector<double> tiempoMinimoYMaximo;
    double tiempoMin = 0;
    double tiempoMax = 0;
    for(int i = 0; i < f.size(); ++i) {
        tiempoMinimoYMaximo = tiemposMinYMax(f[i]);
        tiempoMin = tiempoMinimoYMaximo[0];
        tiempoMax = tiempoMinimoYMaximo[1];
        if(not(tiempoMax <= t0 || tf <= tiempoMin)) {
            ++resp;
        }
    }

    return resp;
}

/************************************** EJERCICIO construirGrilla *******************************/
grilla construirGrilla(gps esq1, gps esq2, int n, int m) {
    grilla resp = {};
    int cantColumnas = m;
    int cantFilas = n;
    double anchoCelda = (obtenerLongitud(esq2) - obtenerLongitud(esq1)) / cantColumnas;
    double altoCelda = (obtenerLatitud(esq1) - obtenerLatitud(esq2)) / cantFilas;
    gps esqSupIzq = esq1;
    gps esqInfDer = puntoGps(obtenerLatitud(esq1) - altoCelda, obtenerLongitud(esq1) + anchoCelda);
    for(int i = 0; i < cantFilas; ++i) {
        for(int j = 0; j < cantColumnas; ++j) {
            gps nuevaEsqSupIzq = puntoGps(obtenerLatitud(esqSupIzq) - (i * altoCelda), obtenerLongitud(esqSupIzq) + (j * anchoCelda));
            gps nuevaEsqInfDer = puntoGps(obtenerLatitud(esqInfDer) - (i * altoCelda), obtenerLongitud(esqInfDer) + (j * anchoCelda));
            nombre nuevoNombre = make_tuple(i+1,j+1);
            resp.push_back(make_tuple(nuevaEsqSupIzq, nuevaEsqInfDer, nuevoNombre));
        }
    }

    return resp;
}

/************************************* EJERCICIO cantidadDeSaltos ******************************/

/** auxiliares **/

viaje ordenarPorTiempo(viaje v) {
    tuple<tiempo,gps> elMayor;
    for(int i = 0; i < v.size()-1; ++i) {
        for(int j = 0; j < v.size()-1; ++j) {
            if(get<0>(v[j+1]) <= get<0>(v[j])){
                elMayor = v[j];
                v[j] = v[j+1];
                v[j+1] = elMayor;
            }
        }
    }
    return v;
}

celda celdaDelGps(grilla g, gps p) {
    int i = 0;
    double latMin = 0;
    double latMax = 0;
    double longMin = 0;
    double longMax = 0;
    double pLat = obtenerLatitud(p);
    double pLong = obtenerLongitud(p);
    bool seEncontroLaCelda = false;
    while(not(seEncontroLaCelda)) {
        latMin = obtenerLatitud(get<1>(g[i]));
        latMax = obtenerLatitud(get<0>(g[i]));
        longMin = obtenerLongitud(get<0>(g[i]));
        longMax = obtenerLongitud(get<1>(g[i]));
        seEncontroLaCelda = ((latMin <= pLat < latMax) && (longMin <= pLong < longMax));
        ++i;
    }
    --i;
    return g[i];
}

double distanciaEntreCeldas(celda celda1, celda celda2) {
    nombre nombre1 = get<2>(celda1);
    nombre nombre2 = get<2>(celda2);
    double distancia = sqrt(pow(get<0>(nombre1) - get<0>(nombre2),2) + pow(get<1>(nombre1) - get<1>(nombre2),2));
    return distancia;
}

/** auxiliares **/

int cantidadDeSaltos(grilla g, viaje v) {
    int resp = 0;
    viaje viajeOrdenado = ordenarPorTiempo(v);
    for(int i = 0; i < v.size()-1; ++i) {
        celda celdaDeGps1 = celdaDelGps(g,obtenerPosicion(viajeOrdenado[i]));
        celda celdaDeGps2 = celdaDelGps(g,obtenerPosicion(viajeOrdenado[i+1]));
        if(distanciaEntreCeldas(celdaDeGps1,celdaDeGps2) > 1) {
            ++resp;
        }
    }

    return resp;
}


/************************************* EJERCICIO corregirViaje ******************************/

/** auxiliares **/

int busquedaBinariaEnViajes(viaje v, tiempo t) {
    int lowIndice = 0;
    int highIndice = v.size() - 1;
    int resultado = -1;
    bool buscando = true;
    while(buscando){
        int midIndice = (highIndice - lowIndice) / 2;

        if(obtenerTiempo(v[lowIndice + midIndice]) < t && midIndice != 0) {
            lowIndice = lowIndice + midIndice;
        }
        else if(obtenerTiempo(v[lowIndice + midIndice]) > t && midIndice != 0){
            highIndice = lowIndice + midIndice;
        }

        else{
            if(obtenerTiempo(v[0]) == t){
                resultado = 0;
                buscando = false;
            }
            else if(obtenerTiempo(v[v.size() - 1]) == t){
                resultado = v.size() - 1;
                buscando = false;
            }
            else{
                resultado = (lowIndice+midIndice);
                buscando = false;
            }
        }
    }
    return resultado;
}

bool busquedaBinaria(vector<tiempo> tiempos, double n) {
    int lowIndice = 0;
    int highIndice = tiempos.size() - 1;
    bool resultado = false;
    bool buscando = true;
    while(buscando){
        int midIndice = (highIndice - lowIndice) / 2;

        if(tiempos[lowIndice + midIndice] < n && midIndice != 0) {
            lowIndice = lowIndice + midIndice;
        }
        else if(tiempos[lowIndice + midIndice] > n && midIndice != 0){
            highIndice = lowIndice + midIndice;
        }

        else{
            if(tiempos[0] == n){
                resultado = true;
                buscando = false;
            }
            else if(tiempos[tiempos.size() - 1] == n){
                resultado = true;
                buscando = false;
            }
            else{
                resultado = tiempos[lowIndice+midIndice] == n;
                buscando = false;
            }
        }
    }
    return resultado;
}

viaje buscarPuntosMasCercanos(viaje v, vector<tiempo> errores, tiempo tiempoDeError) {
    viaje puntosMasCercanos;
    int indiceDeErrorEnViaje = busquedaBinariaEnViajes(v, tiempoDeError);
    bool seEncontro1 = false;
    bool seEncontro2 = false;
    bool seguirBuscando1 = true;
    bool seguirBuscando2 = true;
    bool seEncontraronAmbos = seEncontro1 && seEncontro2;
    int i = 1;

    while(not(seEncontraronAmbos)){
        if(indiceDeErrorEnViaje != 0 && indiceDeErrorEnViaje != (v.size()-1)) {
            seEncontro1 = seEncontro1 || not(busquedaBinaria(errores, obtenerTiempo(v[indiceDeErrorEnViaje - i])));
            seEncontro2 = seEncontro2 || not(busquedaBinaria(errores, obtenerTiempo(v[indiceDeErrorEnViaje + i])));
            seEncontraronAmbos = seEncontro1 && seEncontro2;
            if (seEncontro1 && seguirBuscando1) {
                puntosMasCercanos.push_back(v[indiceDeErrorEnViaje - i]);
                seguirBuscando1 = false;
            }
            if (seEncontro2 && seguirBuscando2) {
                puntosMasCercanos.push_back(v[indiceDeErrorEnViaje + i]);
                seguirBuscando2 = false;
            }
            ++i;
        }
        else if(indiceDeErrorEnViaje == 0){
            seEncontro1 = seEncontro1 || not(busquedaBinaria(errores, obtenerTiempo(v[indiceDeErrorEnViaje + i])));
            seEncontro2 = seEncontro2 || not(busquedaBinaria(errores, obtenerTiempo(v[indiceDeErrorEnViaje + (i+1)])));
            seEncontraronAmbos = seEncontro1 && seEncontro2;
            if (seEncontro1 && seguirBuscando1) {
                puntosMasCercanos.push_back(v[indiceDeErrorEnViaje + i]);
                seguirBuscando1 = false;
            }
            if (seEncontro2 && seguirBuscando2) {
                puntosMasCercanos.push_back(v[indiceDeErrorEnViaje + (i+1)]);
                seguirBuscando2 = false;
            }
            ++i;
        }
        else if(indiceDeErrorEnViaje == (v.size()-1)){
            seEncontro1 = seEncontro1 || not(busquedaBinaria(errores, obtenerTiempo(v[indiceDeErrorEnViaje - i])));
            seEncontro2 = seEncontro2 || not(busquedaBinaria(errores, obtenerTiempo(v[indiceDeErrorEnViaje - (i+1)])));
            seEncontraronAmbos = seEncontro1 && seEncontro2;
            if (seEncontro1 && seguirBuscando1) {
                puntosMasCercanos.push_back(v[indiceDeErrorEnViaje - i]);
                seguirBuscando1 = false;
            }
            if (seEncontro2 && seguirBuscando2) {
                puntosMasCercanos.push_back(v[indiceDeErrorEnViaje - (i+1)]);
                seguirBuscando2 = false;
            }
            ++i;
        }
    }
    return puntosMasCercanos;
}

tuple<tiempo, gps> corregirPunto(viaje puntosMasCercanos, tiempo tiempoDeError){
    tiempo pTiempo = obtenerTiempo(puntosMasCercanos[0]);
    tiempo qTiempo = obtenerTiempo(puntosMasCercanos[1]);
    double pLat = obtenerLatitud(obtenerPosicion(puntosMasCercanos[0]));
    double pLong = obtenerLongitud(obtenerPosicion(puntosMasCercanos[0]));
    double qLat = obtenerLatitud(obtenerPosicion(puntosMasCercanos[1]));
    double qLong = obtenerLongitud(obtenerPosicion(puntosMasCercanos[1]));
    tiempo tiempoDesdePuntoMasCercano = tiempoDeError - pTiempo;
    gps direccionDeCorreccion = puntoGps((qLat - pLat) / (qTiempo - pTiempo), (qLong - pLong) / (qTiempo - pTiempo));
    gps correccion = puntoGps(pLat + (tiempoDesdePuntoMasCercano * obtenerLatitud(direccionDeCorreccion)), pLong + (tiempoDesdePuntoMasCercano * obtenerLongitud(direccionDeCorreccion)));
    return medicion(tiempoDeError,correccion);
}

/** auxiliares **/

void corregirViaje(viaje& v, vector<tiempo> errores){
    viaje viajeOrdenado = ordenarPorTiempo(v);
    viaje puntosMasCercanos;
    tiempo tiempoDeError;
    int indiceDeErrorEnViaje;
    for(int i = 0; i < errores.size(); ++i){
        tiempoDeError = errores[i];
        indiceDeErrorEnViaje = busquedaBinariaEnViajes(viajeOrdenado, tiempoDeError);
        puntosMasCercanos = buscarPuntosMasCercanos(viajeOrdenado, errores, tiempoDeError);
        v[indiceDeErrorEnViaje] = corregirPunto(puntosMasCercanos, tiempoDeError);
    }
}
