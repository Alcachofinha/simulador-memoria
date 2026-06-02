#include "raylib.h"
#include "SimuladorMemoria.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Estados de la interfaz grafica
enum EstadoPantalla { MENU_PRINCIPAL, MENU_PROCESOS, SIMULACION };

struct ProcesoTemp {
    int llegada;
    int tamano;
    int duracion;
};

int main() {
    const int screenWidth = 1000;
    const int screenHeight = 750;
    InitWindow(screenWidth, screenHeight, "Gestor de Memoria");
    SetTargetFPS(60);

    EstadoPantalla pantallaActual = MENU_PRINCIPAL;

    // Variables de configuracion principal
    int ram_menu = 100;
    int desfrag_menu = 1; 
    int criterio_menu = 0; 
    int opcion_seleccionada = 0; 

    // Variables del constructor de procesos
    vector<ProcesoTemp> lista_procesos;
    int p_llegada = 0;
    int p_tamano = 20;
    int p_duracion = 5;
    int opcion_proc = 0; 

    SimuladorMemoria* simulador = nullptr;
    bool simulacionActiva = true;

    while (!WindowShouldClose()) {
        
        // Pantalla 1: Configuracion general del simulador
        if (pantallaActual == MENU_PRINCIPAL) {
            
            // Navegacion con flechas
            if (IsKeyPressed(KEY_DOWN)) opcion_seleccionada = (opcion_seleccionada + 1) % 4;
            if (IsKeyPressed(KEY_UP)) opcion_seleccionada = (opcion_seleccionada - 1 + 4) % 4;

            if (opcion_seleccionada == 0) {
                if (IsKeyPressed(KEY_RIGHT)) ram_menu += 10;
                if (IsKeyPressed(KEY_LEFT) && ram_menu > 50) ram_menu -= 10;
            } 
            else if (opcion_seleccionada == 1) {
                if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_LEFT)) desfrag_menu = !desfrag_menu;
            }
            else if (opcion_seleccionada == 2) {
                if (IsKeyPressed(KEY_RIGHT)) criterio_menu = (criterio_menu + 1) % 3;
                if (IsKeyPressed(KEY_LEFT)) criterio_menu = (criterio_menu - 1 + 3) % 3;
            }
            else if (opcion_seleccionada == 3) {
                if (IsKeyPressed(KEY_ENTER)) {
                    pantallaActual = MENU_PROCESOS; 
                }
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawText("PASO 1: CONFIGURACION DEL SIMULADOR", 200, 100, 30, DARKBLUE);
            DrawText("Usa las FLECHAS para moverte/cambiar. ENTER para continuar.", 150, 150, 20, GRAY);

            Color colorOp0 = (opcion_seleccionada == 0) ? MAROON : DARKGRAY;
            DrawText(TextFormat("Memoria RAM Total: < %d >", ram_menu), 250, 250, 25, colorOp0);

            Color colorOp1 = (opcion_seleccionada == 1) ? MAROON : DARKGRAY;
            string strDesfrag = (desfrag_menu == 1) ? "ACTIVADA (SI)" : "DESACTIVADA (NO)";
            DrawText(TextFormat("Desfragmentacion: < %s >", strDesfrag.c_str()), 250, 320, 25, colorOp1);

            Color colorOp2 = (opcion_seleccionada == 2) ? MAROON : DARKGRAY;
            string strCrit = (criterio_menu == 0) ? "FIFO" : (criterio_menu == 1) ? "Menor Tamano" : "Mayor Tamano";
            DrawText(TextFormat("Criterio de Espera: < %s >", strCrit.c_str()), 250, 390, 25, colorOp2);

            Color colorOp3 = (opcion_seleccionada == 3) ? GREEN : LIGHTGRAY;
            DrawText("[ CONTINUAR A CREAR PROCESOS ]", 250, 500, 25, colorOp3);

            DrawText("Alvaro Salazar & Diego Sotelo - FISI CIENCIAS DE LA COMPUTACION", 20, 700, 15, GRAY);
            EndDrawing();
        }

        // Pantalla 2: Insercion dinamica de procesos
        else if (pantallaActual == MENU_PROCESOS) {
            
            if (IsKeyPressed(KEY_DOWN)) opcion_proc = (opcion_proc + 1) % 5;
            if (IsKeyPressed(KEY_UP)) opcion_proc = (opcion_proc - 1 + 5) % 5;

            if (opcion_proc == 0) {
                if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_UP)) p_llegada++;
                if ((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_DOWN)) && p_llegada > 0) p_llegada--;
            } else if (opcion_proc == 1) {
                if (IsKeyPressed(KEY_RIGHT)) p_tamano += 5;
                if (IsKeyPressed(KEY_LEFT) && p_tamano > 5) p_tamano -= 5;
            } else if (opcion_proc == 2) {
                if (IsKeyPressed(KEY_RIGHT)) p_duracion++;
                if (IsKeyPressed(KEY_LEFT) && p_duracion > 1) p_duracion--;
            } else if (opcion_proc == 3) { 
                if (IsKeyPressed(KEY_ENTER)) {
                    lista_procesos.push_back({p_llegada, p_tamano, p_duracion});
                    p_llegada += 2; 
                }
            } else if (opcion_proc == 4) { 
                if (IsKeyPressed(KEY_ENTER) && !lista_procesos.empty()) {
                    ModoDefragmentacion modo = (desfrag_menu == 1) ? CON_DF : SIN_DF;
                    SegundoCriterio crit = (SegundoCriterio)criterio_menu;
                    
                    simulador = new SimuladorMemoria(ram_menu, modo, crit);
                    
                    for (size_t i = 0; i < lista_procesos.size(); i++) {
                        simulador->agregar_proceso(i + 1, lista_procesos[i].llegada, lista_procesos[i].tamano, lista_procesos[i].duracion);
                    }
                    pantallaActual = SIMULACION;
                }
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawText("PASO 2: CONSTRUCTOR DE PROCESOS", 50, 50, 30, DARKBLUE);
            DrawText("Configura los valores con las FLECHAS y presiona ENTER en Agregar", 50, 90, 20, GRAY);

            Color col0 = (opcion_proc == 0) ? MAROON : DARKGRAY;
            DrawText(TextFormat("T. Llegada: < %d >", p_llegada), 50, 150, 25, col0);

            Color col1 = (opcion_proc == 1) ? MAROON : DARKGRAY;
            DrawText(TextFormat("Tamano:     < %d >", p_tamano), 50, 200, 25, col1);

            Color col2 = (opcion_proc == 2) ? MAROON : DARKGRAY;
            DrawText(TextFormat("Duracion:   < %d >", p_duracion), 50, 250, 25, col2);

            Color col3 = (opcion_proc == 3) ? ORANGE : LIGHTGRAY;
            DrawText("[ AGREGAR PROCESO ]", 50, 330, 25, col3);

            Color col4 = (opcion_proc == 4 && !lista_procesos.empty()) ? GREEN : LIGHTGRAY;
            DrawText("[ INICIAR SIMULACION ]", 50, 400, 25, col4);

            if (lista_procesos.empty() && opcion_proc == 4) {
                DrawText("Agrega al menos 1 proceso", 50, 430, 15, RED);
            }

            // Tabla visual de procesos encolados
            DrawRectangle(500, 130, 450, 550, LIGHTGRAY);
            DrawRectangleLines(500, 130, 450, 550, DARKGRAY);
            DrawText("PROCESOS AGREGADOS:", 520, 150, 20, BLACK);
            
            DrawText("PID   Llegada   Tamano   Duracion", 520, 190, 18, DARKBLUE);
            DrawLine(520, 215, 900, 215, GRAY);

            int listY = 230;
            for (size_t i = 0; i < lista_procesos.size(); i++) {
                DrawText(TextFormat("P%d", i+1), 530, listY, 18, BLACK);
                DrawText(TextFormat("%d", lista_procesos[i].llegada), 600, listY, 18, BLACK);
                DrawText(TextFormat("%d", lista_procesos[i].tamano), 700, listY, 18, BLACK);
                DrawText(TextFormat("%d", lista_procesos[i].duracion), 810, listY, 18, BLACK);
                listY += 25;
            }

            DrawText("Alvaro Salazar & Diego Sotelo - FISI CIENCIAS DE LA COMPUTACION", 20, 700, 15, GRAY);
            EndDrawing();
        }

        // Pantalla 3: Simulacion en vivo 
        else if (pantallaActual == SIMULACION && simulador != nullptr) {
            
            // Avanza el tiempo al presionar espacio
            if (IsKeyPressed(KEY_SPACE) && simulacionActiva) {
                simulacionActiva = simulador->avanzar_un_instante();
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawText("SIMULADOR DE ASIGNACION DE MEMORIA", 20, 20, 20, DARKBLUE);
            DrawText("Presiona [ESPACIO] para avanzar 1 segundo", 20, 50, 20, GRAY);
            
            string textoTiempo = "Tiempo Actual (t): " + to_string(simulador->get_tiempo());
            DrawText(textoTiempo.c_str(), 20, 90, 30, MAROON);

            if (!simulacionActiva) {
                DrawText("¡SIMULACION TERMINADA!", 400, 90, 30, DARKGREEN);
            }

            // Renderizado de la RAM
            int ramX = 50;
            int ramY = 150;
            int escalaPixeles = 800 / simulador->get_capacidad(); 
            if (escalaPixeles < 1) escalaPixeles = 1;

            DrawRectangleLines(ramX, ramY, simulador->get_capacidad() * escalaPixeles, 80, BLACK);

            BloqueMemoria* actual = simulador->get_memoria();
            while (actual != nullptr) {
                int posX = ramX + (actual->direccion_inicio * escalaPixeles);
                int ancho = actual->tamano * escalaPixeles;

                if (actual->esta_libre) {
                    DrawRectangle(posX, ramY, ancho, 80, LIGHTGRAY);
                    DrawRectangleLines(posX, ramY, ancho, 80, DARKGRAY);
                    if (ancho > 40) DrawText("Libre", posX + 5, ramY + 30, 10, DARKGRAY);
                } else {
                    DrawRectangle(posX, ramY, ancho, 80, SKYBLUE);
                    DrawRectangleLines(posX, ramY, ancho, 80, BLUE);
                    if (ancho > 30) {
                        string textoProceso = "P" + to_string(actual->id_proceso);
                        DrawText(textoProceso.c_str(), posX + ancho/2 - 10, ramY + 30, 20, DARKBLUE);
                    }
                }
                actual = actual->siguiente;
            }

            // Renderizado de la cola de espera
            DrawText("COLA DE ESPERA:", 50, 260, 20, DARKGRAY);
            vector<int> cola = simulador->get_cola_espera();
            vector<Proceso> procs = simulador->get_procesos();
            int colaX = 50;
            int colaY = 290;
            
            if (cola.empty()) {
                DrawText("Vacia (Ningun proceso esperando)", colaX, colaY + 10, 20, LIGHTGRAY);
            } else {
                for (size_t i = 0; i < cola.size(); i++) {
                    int pid = cola[i];
                    Proceso p = procs[pid - 1];
                    DrawRectangle(colaX, colaY, 60, 40, ORANGE);
                    DrawRectangleLines(colaX, colaY, 60, 40, RED);
                    DrawText(("P" + to_string(pid)).c_str(), colaX + 15, colaY + 10, 20, WHITE);
                    DrawText(("Tam: " + to_string(p.tamano)).c_str(), colaX, colaY + 45, 15, DARKGRAY);
                    colaX += 80;
                }
            }

            // Renderizado de la tabla de resultados
            int tablaY = 380;
            DrawLine(50, tablaY - 10, 950, tablaY - 10, LIGHTGRAY);
            DrawText("TABLA DE ESTADO DE PROCESOS:", 50, tablaY, 20, DARKGRAY);
            tablaY += 35;
            
            DrawText("PID", 50, tablaY, 15, BLACK);
            DrawText("Llegada", 120, tablaY, 15, BLACK);
            DrawText("Tamano", 220, tablaY, 15, BLACK);
            DrawText("Duracion", 320, tablaY, 15, BLACK);
            DrawText("T. Salida", 420, tablaY, 15, BLACK);
            DrawText("Estado Actual", 520, tablaY, 15, BLACK);
            DrawLine(50, tablaY + 20, 650, tablaY + 20, GRAY);
            
            tablaY += 30;
            
            for (const auto& p : procs) {
                DrawText(TextFormat("P%d", p.id), 50, tablaY, 15, DARKGRAY);
                DrawText(TextFormat("%d", p.tiempo_llegada), 120, tablaY, 15, DARKGRAY);
                DrawText(TextFormat("%d", p.tamano), 220, tablaY, 15, DARKGRAY);
                DrawText(TextFormat("%d", p.duracion), 320, tablaY, 15, DARKGRAY);
                
                if (p.tiempo_salida != -1) DrawText(TextFormat("%d", p.tiempo_salida), 420, tablaY, 15, DARKBLUE);
                else DrawText("-", 435, tablaY, 15, LIGHTGRAY);

                string estadoStr = "";
                Color estadoColor = GRAY;
                if (p.estado == NO_LLEGADO) { estadoStr = "No Llegado"; estadoColor = LIGHTGRAY; }
                else if (p.estado == ESPERANDO) { estadoStr = "En Cola (Esperando)"; estadoColor = ORANGE; }
                else if (p.estado == EJECUTANDO) { estadoStr = "En RAM (Ejecutando)"; estadoColor = BLUE; }
                else if (p.estado == TERMINADO) { estadoStr = "Terminado"; estadoColor = DARKGREEN; }

                DrawText(estadoStr.c_str(), 520, tablaY, 15, estadoColor);
                tablaY += 25;
            }

            DrawText("Alvaro Salazar & Diego Sotelo - FISI CIENCIAS DE LA COMPUTACION", 20, 700, 15, GRAY);

            EndDrawing();
        }
    }

    // Liberacion de memoria dinamica
    if (simulador != nullptr) delete simulador;
    
    CloseWindow();
    return 0;
}