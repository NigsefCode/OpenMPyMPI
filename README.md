# Evaluación 5: OpenMP y MPI
- Integrante: Nicolás Sepúlveda Falcón
  
- Curso: Administración de Sistemas



## Introducción
Este proyecto tiene como objetivo implementar y evaluar diferentes algoritmos utilizando programación paralela con OpenMP y MPI. Se generará una simulación de torneo, en donde, en este torneo de fútbol, los equipos competirán en dos fases distintas: una liga local de todos contra todos. Este modelo permite simular una competencia en la que los equipos se clasifican a nivel nacional.

## Instrucciones de Instalación y Ejecución
### Requisitos Previos
Antes de comenzar con la ejecución del proyecto, asegurarse de tener lo siguiente instalado en su equipo de trabajo:
1. **OpenMP**
2. **Python y Dependencias**

En caso de no tener instalador **_OpenMP_**, puede instalarlo de la siguiente manera:
  ```bash
  sudo apt-get install -y build-essential
  sudo apt-get install -y openmpi-bin openmpi-common libopenmpi-dev
  sudo apt-get install -y gcc-multilib
  ```
En caso de no tener instalador **_Python_**, puede instalarlo de la siguiente manera:
  ```bash
  sudo apt install python3-full python3-venv
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
3. **Configurar el Entorno Virtual en Python.**
    ```bash
      python3 -m venv venv
      source venv/bin/activate
      pip install matplotlib numpy
      pip install pandas
    ```

### Ejercicio 1: Cálculo de Factoriales en Paralelo
