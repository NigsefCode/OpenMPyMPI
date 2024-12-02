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

En caso de no tener instalador **_OpenMP_**, puede instalarlo de la siguiente manera:
  ```bash
  sudo apt-get install -y build-essential
  sudo apt-get install -y openmpi-bin openmpi-common libopenmpi-dev
  sudo apt-get install -y gcc-multilib
  ```

### Configuración y Ejecución
Una vez corroborada la instalación de los requisitos previos, puede seguir los siguientes pasos:

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

## Estructura del Proyecto

- `src/main.c`: Archivo principal con la implementación
- `Makefile`: Archivo de configuración para compilación

## Detalles de Implementación
- Cada país (nodo MPI) maneja 20 equipos
- Cada equipo tiene 25 jugadores
- Los jugadores tienen un rendimiento aleatorio por partido

## Funcionamiento
- Cada nodo MPI representa un país
- Los partidos se juegan en formato todos contra todos
- OpenMP paraleliza el cálculo del rendimiento de jugadores
- Sistema de puntos: 3 por victoria, 1 por empate, 0 por derrota

## Compilación y Ejecución
```bash
    make
    mpirun -np 8 ./torneo
```

## Clasificación
- Los 2 mejores equipos de cada país clasifican
- Proceso de clasificación:
  1. Ordenamiento por puntos
  2. Selección de mejores equipos
  3. Envío de clasificados al nodo principal

## Fase Eliminatoria
- Los equipos clasificados compiten en eliminación directa
- El rendimiento se recalcula en cada partido
- El ganador avanza a la siguiente ronda
- La fase continúa hasta tener un campeón
