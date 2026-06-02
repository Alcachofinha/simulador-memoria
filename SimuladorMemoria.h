#ifndef SIMULADOR_MEMORIA_H
#define SIMULADOR_MEMORIA_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;

enum ModoDefragmentacion { SIN_DF, CON_DF };
enum SegundoCriterio { FIFO, MENOR_TAMANO, MAYOR_TAMANO };
enum EstadoProceso { NO_LLEGADO, ESPERANDO, EJECUTANDO, TERMINADO };

struct Proceso {
    int id;
    int tiempo_llegada;
    int tamano;
    int duracion;
    int tiempo_salida;
    EstadoProceso estado;
};

struct BloqueMemoria {
    int direccion_inicio;
    int tamano;
    bool esta_libre;
    int id_proceso;
    BloqueMemoria* anterior;
    BloqueMemoria* siguiente;

    BloqueMemoria(int inicio, int tam, bool libre, int id_proc) {
        direccion_inicio = inicio;
        tamano = tam;
        esta_libre = libre;
        id_proceso = id_proc;
        anterior = nullptr;
        siguiente = nullptr;
    }
};

class SimuladorMemoria {
private:
    int capacidad_total;
    ModoDefragmentacion modo_df;
    SegundoCriterio criterio_espera;
    BloqueMemoria* cabeza_memoria;
    vector<Proceso> procesos;
    vector<int> cola_espera;
    int tiempo_actual;

    // Metodos internos de gestion
    void liberar_memoria_fisica();
    void consolidar_bloques_libres();
    bool intentar_asignacion_first_fit(int id_proceso);
    void ejecutar_desfragmentacion();
    int obtener_espacio_libre_total();
    void graficar_memoria();
    void ordenar_cola_espera();

public:
    SimuladorMemoria(int capacidad, ModoDefragmentacion modo, SegundoCriterio criterio);
    ~SimuladorMemoria();
    
    void agregar_proceso(int id, int llegada, int tam, int dur);
    
    // Metodos para la interfaz grafica
    bool avanzar_un_instante(); 
    BloqueMemoria* get_memoria() { return cabeza_memoria; } 
    int get_tiempo() { return tiempo_actual; }
    int get_capacidad() { return capacidad_total; }
    vector<int> get_cola_espera() { return cola_espera; }
    vector<Proceso> get_procesos() { return procesos; }
};

#endif