# Evaluación 5: OpenMP y MPI
- Integrante: Nicolás Sepúlveda Falcón
  
- Curso: Administración de Sistemas



## Introducción
Este proyecto tiene como objetivo implementar y evaluar diferentes algoritmos utilizando programación paralela con OpenMP y MPI. Se generará una simulación de torneo, en donde, en este torneo de fútbol, los equipos competirán en dos fases distintas: una liga local de todos contra todos. Este modelo permite simular una competencia en la que los equipos se clasifican a nivel nacional.

## Instrucciones de Instalación y Ejecución
### Requisitos Previos
Antes de comenzar con la ejecución del proyecto, asegurarse de tener lo siguiente instalado en su equipo de trabajo:
1. **Sistema Operativo Linux**
2. **OpenMP**
3. **GCC con Soporte OpenMP**

### Configuración y Ejecución
Una vez corroborada la instalación de los requisitos previos, o puede seguir los siguientes pasos:

1. **Actualizar el Sistema Operativo.**
    ```bash
      sudo apt update
      sudo apt upgrade
    ```
2. **Clonar el Repositorio.**
    ```bash
      git clone <url-del-repositorio>
      cd <nombre-del-directorio>
    ```
3. **OpenMP**

   En caso de no tener instalador **_OpenMP_**, puede instalarlo de la siguiente manera:
    ```bash
      sudo apt-get install -y build-essential
      sudo apt-get install -y openmpi-bin openmpi-common libopenmpi-dev
      sudo apt-get install -y gcc-multilib
    ```

## Caracteristicas del Proyecto
### Estructura

- `src/main.c`: Archivo principal con la implementación
- `Makefile`: Archivo de configuración para compilación

### Detalles de Implementación
- Cada país (nodo MPI) maneja 20 equipos
- Cada equipo tiene 25 jugadores
- Los jugadores tienen un rendimiento aleatorio por partido

### Funcionamiento
- Cada nodo MPI representa un país
- Los partidos se juegan en formato todos contra todos
- OpenMP paraleliza el cálculo del rendimiento de jugadores
- Sistema de puntos: 3 por victoria, 1 por empate, 0 por derrota

### Compilación y Ejecución
```bash
    make
    mpirun --oversubscribe -np 8 ./torneo
```

### Clasificación
- Los 2 mejores equipos de cada país clasifican
- Proceso de clasificación:
  1. Ordenamiento por puntos
  2. Selección de mejores equipos
  3. Envío de clasificados al nodo principal

### Fases del torneo
1. Liga Local
   - Formato todos contra todos
   - 3 puntos victoria, 1 empate, 0 derrota
   - Clasifican 2 mejores equipos por país
     
3. Fase Eliminatoria
   - Los equipos clasificados compiten en eliminación directa
   - Participan 16 equipos (2 por país)
   - Emparejamientos aleatorios
   - Un solo campeón

### Paralelización
- `MPI:` Distribuye los países en diferentes nodos
- `OpenMP:` Paraleliza cálculos de rendimiento de jugadores

## Resultados
El programa muestra:
- Progreso de la liga local por país
- Tabla de clasificación
- Árbol de eliminatorias
- Estadísticas del campeón

## Limitaciones
- Requiere exactamente 8 procesos MPI
- No persiste resultados entre ejecuciones
- Emparejamientos aleatorios pueden repetirse

## Parámetros Modificables

El programa permite la modificación de varios parámetros para personalizar el torneo, en caso de querer hacerlo, por ejemplo el ajuste del tamaño del torneo, cambiar el sistema de puntuación, entre otros (cabe mencionar que cualquier error ocacionado, deberá arreglarlo la persona que modificó el código):
> **Nota**: Al modificar estos valores, asegúrese de mantener la coherencia en todo el código. Por ejemplo, si cambia NUM_PAISES, debe ejecutar el programa con la misma cantidad de procesos MPI (-np).

### Constantes Principales
```c
#define NUM_JUGADORES 25  // Cantidad de jugadores por equipo
#define NUM_EQUIPOS 20    // Cantidad de equipos por país
#define NUM_PAISES 8      // Total de países participantes
```

### Cálculo de Rendimiento
```c
// En la función seleccionar_titulares()
equipo->jugadores[i].rendimiento = rand() % 99 + 1;  // Rango de rendimiento: 1-99
```

### Sistema de puntos
```c
// En la función jugar_partido()
if(rendimiento1 > rendimiento2) {
    equipo1->puntos += 3;    // Puntos por victoria
} else if(rendimiento2 > rendimiento1) {
    equipo2->puntos += 3;    // Puntos por victoria
} else {
    equipo1->puntos += 1;    // Puntos por empate
    equipo2->puntos += 1;    // Puntos por empate
}
```
> **Nota**: Cada cambio queda a responsabilidad del usuario.
