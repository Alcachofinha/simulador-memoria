# Simulador de Asignacion de Memoria RAM

Un simulador grafico e interactivo desarrollado en C++ utilizando la biblioteca Raylib. Este proyecto simula la gestion de memoria de un sistema operativo, aplicando concepto de asignacion continua, empleando defragmentacion o no y criterios de prioridad como FIFO, mas largo o mas corto.
Autores: Alvaro Matias Salazar Zapata & Diego Alejandro Sotelo Atuncar
Facultad: FISI - Ciencias de la Computacion (UNMSM)

---

## Ver la Simulacion en Vivo (WebAssembly)

El codigo de C++ ha sido compilado nativamente a WebAssembly para ejecutarse directamente en el navegador.
[Clic aqui para abrir el Simulador Web](https://alcachofinha.github.io/simulador-memoria/)

---

## Caracteristicas del Proyecto

El programa permite configurar un entorno de memoria RAM y una serie de procesos dinamicos para observar en tiempo real como el sistema operativo los maneja. Sus caracteristicas principales incluyen:

* **Asignacion Dinamica:** Utiliza el criterio de seleccion First-Fit (Primer Ajuste) para ubicar procesos en la RAM.
* **Gestion de Colas de Espera:** Si un proceso no entra en la memoria, se encola bajo criterios personalizables (FIFO, Menor Tamano o Mayor Tamano).
* **Desfragmentacion:** Opcion activable para compactar la memoria, desplazando los bloques ocupados al principio y unificando la memoria libre fragmentada.
* **Interfaz Grafica (GUI):** Renderizado en tiempo real a 60 FPS con Raylib, mostrando la memoria fisica, la cola de espera y una tabla de estados interactiva.

---

## Instrucciones de Compilacion Local (Windows)

Si deseas descargar el codigo fuente para revisarlo, modificarlo o compilarlo en tu propia maquina Windows, sigue estos pasos:

### 1. Clonar el repositorio
Abre tu terminal (Git Bash o CMD) y ejecuta:

```bash
git clone [https://github.com/alcachofinha/simulador-memoria.git](https://github.com/alcachofinha/simulador-memoria.git)
cd simulador-memoria/src
```

2. Preparar la libreria (Raylib)
- Asegurate de tener instalado el compilador MinGW (g++) para Windows.
- Descarga la libreria Raylib 5.0 (MinGW w64). Especificamente el archivo raylib-5.0_win64_mingw-w64.zip.
- Abre el archivo .zip que descargaste, busca los archivos raylib.h (en la carpeta include) y libraylib.a (en la carpeta lib).
- Pega esos dos archivos exactamente dentro de la carpeta src a la que acabas de entrar.

3. COMPILAR
# Opcion A: Comando estandar 
g++ main.cpp SimuladorMemoria.cpp -o simulador_gui.exe -I. -L. -lraylib -lopengl32 -lgdi32 -lwinmm

# Opcion B: Comando portable (Ideal para correr en otras PC sin instalar nada)
g++ main.cpp SimuladorMemoria.cpp -o simulador_portable.exe -I. -L. -lraylib -lopengl32 -lgdi32 -lwinmm -static-libgcc -static-libstdc++ -static

4.EJECUTAR
./simulador_gui.exe
