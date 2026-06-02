#include "SimuladorMemoria.h"

// Constructor
SimuladorMemoria::SimuladorMemoria(int capacidad, ModoDefragmentacion modo, SegundoCriterio criterio) {
    capacidad_total = capacidad;
    modo_df = modo;
    criterio_espera = criterio;
    cabeza_memoria = new BloqueMemoria(0, capacidad_total, true, -1); 
    tiempo_actual = 0;
}

// Destructor para limpiar la memoria dinamica
SimuladorMemoria::~SimuladorMemoria() {
    BloqueMemoria* actual = cabeza_memoria;
    while (actual != nullptr) {
        BloqueMemoria* temp = actual;
        actual = actual->siguiente;
        delete temp;
    }
}

// Agregar un nuevo proceso a la lista general
void SimuladorMemoria::agregar_proceso(int id, int llegada, int tam, int dur) {
    procesos.push_back({id, llegada, tam, dur, -1, NO_LLEGADO});
}

// Control del tiempo del simulador
bool SimuladorMemoria::avanzar_un_instante() {
    bool hubo_evento = false;

    // Fase 1: Salida de procesos terminados
    for (auto& p : procesos) {
        if (p.estado == EJECUTANDO && p.tiempo_salida == tiempo_actual) {
            p.estado = TERMINADO;
            BloqueMemoria* actual = cabeza_memoria;
            while (actual != nullptr) {
                if (!actual->esta_libre && actual->id_proceso == p.id) {
                    actual->esta_libre = true;
                    actual->id_proceso = -1;
                    break;
                }
                actual = actual->siguiente;
            }
            consolidar_bloques_libres();
            hubo_evento = true;
        }
    }

    // Fase 2: Llegada de nuevos procesos a la cola
    for (auto& p : procesos) {
        if (p.tiempo_llegada == tiempo_actual && p.estado == NO_LLEGADO) {
            p.estado = ESPERANDO;
            cola_espera.push_back(p.id);
            hubo_evento = true;
        }
    }

    // Fase 3: Asignacion de RAM a los procesos en espera
    if (!cola_espera.empty()) {
        ordenar_cola_espera();
        auto it = cola_espera.begin();
        while (it != cola_espera.end()) {
            int id_proc = *it;
            Proceso& p = procesos[id_proc - 1];

            if (intentar_asignacion_first_fit(id_proc)) {
                it = cola_espera.erase(it);
                hubo_evento = true;
            } else if (modo_df == CON_DF && obtener_espacio_libre_total() >= p.tamano) {
                ejecutar_desfragmentacion();
                if (intentar_asignacion_first_fit(id_proc)) {
                    it = cola_espera.erase(it);
                    hubo_evento = true;
                } else {
                    ++it;
                }
            } else {
                ++it;
            }
        }
    }

    // Verificacion de fin de simulacion
    bool todos_terminados = true;
    for (const auto& p : procesos) {
        if (p.estado != TERMINADO) {
            todos_terminados = false;
            break;
        }
    }

    if (todos_terminados) {
        return false; 
    } else {
        tiempo_actual++;
        return true;  
    }
}

// Fusiona bloques libres adyacentes
void SimuladorMemoria::consolidar_bloques_libres() {
    BloqueMemoria* actual = cabeza_memoria;
    while (actual != nullptr && actual->siguiente != nullptr) {
        if (actual->esta_libre && actual->siguiente->esta_libre) {
            BloqueMemoria* borrar = actual->siguiente;
            actual->tamano += borrar->tamano;
            actual->siguiente = borrar->siguiente;
            if (borrar->siguiente != nullptr) {
                borrar->siguiente->anterior = actual;
            }
            delete borrar;
        } else {
            actual = actual->siguiente;
        }
    }
}

// Busca el primer hueco disponible (First-Fit)
bool SimuladorMemoria::intentar_asignacion_first_fit(int id_proceso) {
    Proceso& p = procesos[id_proceso - 1];
    BloqueMemoria* actual = cabeza_memoria;

    while (actual != nullptr) {
        if (actual->esta_libre && actual->tamano >= p.tamano) {
            if (actual->tamano > p.tamano) {
                BloqueMemoria* nuevo_bloque = new BloqueMemoria(actual->direccion_inicio + p.tamano, actual->tamano - p.tamano, true, -1);
                nuevo_bloque->siguiente = actual->siguiente;
                nuevo_bloque->anterior = actual;
                if (actual->siguiente != nullptr) {
                    actual->siguiente->anterior = nuevo_bloque;
                }
                actual->siguiente = nuevo_bloque;
            }
            actual->tamano = p.tamano;
            actual->esta_libre = false;
            actual->id_proceso = p.id;

            p.estado = EJECUTANDO;
            p.tiempo_salida = tiempo_actual + p.duracion;
            return true;
        }
        actual = actual->siguiente;
    }
    return false;
}

// Compacta la memoria empujando los procesos al inicio
void SimuladorMemoria::ejecutar_desfragmentacion() {
    int direccion_actual = 0;
    BloqueMemoria* actual = cabeza_memoria;
    BloqueMemoria* ultimo_ocupado = nullptr;

    while (actual != nullptr) {
        if (!actual->esta_libre) {
            actual->direccion_inicio = direccion_actual;
            direccion_actual += actual->tamano;
            ultimo_ocupado = actual;
        }
        actual = actual->siguiente;
    }

    if (direccion_actual < capacidad_total) {
        BloqueMemoria* bloque_libre = new BloqueMemoria(direccion_actual, capacidad_total - direccion_actual, true, -1);
        if (ultimo_ocupado == nullptr) {
            while (cabeza_memoria != nullptr) {
                BloqueMemoria* temp = cabeza_memoria;
                cabeza_memoria = cabeza_memoria->siguiente;
                delete temp;
            }
            cabeza_memoria = bloque_libre;
        } else {
            BloqueMemoria* borrar = ultimo_ocupado->siguiente;
            while (borrar != nullptr) {
                BloqueMemoria* temp = borrar;
                borrar = borrar->siguiente;
                delete temp;
            }
            ultimo_ocupado->siguiente = bloque_libre;
            bloque_libre->anterior = ultimo_ocupado;
        }
    }
}

// Calcula suma de memoria libre fragmentada
int SimuladorMemoria::obtener_espacio_libre_total() {
    int total = 0;
    BloqueMemoria* actual = cabeza_memoria;
    while (actual != nullptr) {
        if (actual->esta_libre) total += actual->tamano;
        actual = actual->siguiente;
    }
    return total;
}

// Ordenamiento de burbuja para la cola de espera segun criterio
void SimuladorMemoria::ordenar_cola_espera() {
    if (criterio_espera == FIFO) return; 

    for (size_t i = 0; i < cola_espera.size(); i++) {
        for (size_t j = i + 1; j < cola_espera.size(); j++) {
            int id1 = cola_espera[i];
            int id2 = cola_espera[j];
            bool intercambiar = false;

            if (criterio_espera == MENOR_TAMANO) {
                if (procesos[id2 - 1].tamano < procesos[id1 - 1].tamano) intercambiar = true;
            } else if (criterio_espera == MAYOR_TAMANO) {
                if (procesos[id2 - 1].tamano > procesos[id1 - 1].tamano) intercambiar = true;
            }

            if (intercambiar) {
                int temp = cola_espera[i];
                cola_espera[i] = cola_espera[j];
                cola_espera[j] = temp;
            }
        }
    }
}

// Funciones vacias requeridas por el header
void SimuladorMemoria::graficar_memoria() {}
void SimuladorMemoria::liberar_memoria_fisica() {}