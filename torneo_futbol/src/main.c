#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include <time.h>
#include <string.h>

/* Definición de constantes del torneo */
#define NUM_JUGADORES 25  // Jugadores por equipo
#define NUM_EQUIPOS 20    // Equipos por país
#define NUM_PAISES 8      // Total de países participantes

/* Estructuras de datos principales */
typedef struct {
   int id;
   int rendimiento;
} Jugador;

typedef struct {
   int id;
   char nombre[50];
   Jugador jugadores[NUM_JUGADORES];  // Plantel completo
   Jugador titulares[11];             // 11 mejores para cada partido
   int puntos;
} Equipo;

/* Estructuras para estadísticas y seguimiento */
typedef struct {
   int victorias;
   int empates;
   int derrotas;
   int goles_favor;
} Estadisticas;

typedef struct {
   Equipo equipo;
   int pais_origen;
   Estadisticas stats;
} EquipoClasificado;

typedef struct {
   EquipoClasificado equipo;
   int activo;  // Control de equipos eliminados
} EquipoEliminatoria;

/* Inicialización del plantel de jugadores */
void inicializar_jugadores(Jugador *jugadores) {
   #pragma omp parallel for
   for(int i = 0; i < NUM_JUGADORES; i++) {
       jugadores[i].id = i;
       jugadores[i].rendimiento = 0;
   }
}

/* Selección optimizada de los 11 mejores jugadores */
void seleccionar_titulares(Equipo *equipo) {
   // Generación paralela de rendimientos
   #pragma omp parallel for
   for(int i = 0; i < NUM_JUGADORES; i++) {
       equipo->jugadores[i].rendimiento = rand() % 99 + 1;
   }
   
   // Ordenamiento para seleccionar los 11 mejores
   for(int i = 0; i < NUM_JUGADORES - 1; i++) {
       for(int j = 0; j < NUM_JUGADORES - i - 1; j++) {
           if(equipo->jugadores[j].rendimiento < equipo->jugadores[j + 1].rendimiento) {
               Jugador temp = equipo->jugadores[j];
               equipo->jugadores[j] = equipo->jugadores[j + 1];
               equipo->jugadores[j + 1] = temp;
           }
       }
   }
   
   // Selección de los 11 mejores
   #pragma omp parallel for
   for(int i = 0; i < 11; i++) {
       equipo->titulares[i] = equipo->jugadores[i];
   }
}

/* Cálculo paralelo del rendimiento del equipo */
int calcular_rendimiento_equipo(Equipo *equipo) {
   seleccionar_titulares(equipo);
   int total = 0;
   
   // Suma paralela de rendimientos usando reduction
   #pragma omp parallel for reduction(+:total)
   for(int i = 0; i < 11; i++) {
       total += equipo->titulares[i].rendimiento;
   }
   return total;
}

/* Simulación de partido con gestión de resultados */
void jugar_partido(Equipo *equipo1, Equipo *equipo2, Estadisticas *stats1, Estadisticas *stats2) {
   int rendimiento1 = calcular_rendimiento_equipo(equipo1);
   int rendimiento2 = calcular_rendimiento_equipo(equipo2);
   
   // Asignación de puntos y actualización de estadísticas
   if(rendimiento1 > rendimiento2) {
       equipo1->puntos += 3;
       stats1->victorias++;
       stats2->derrotas++;
       stats1->goles_favor++;
   } else if(rendimiento2 > rendimiento1) {
       equipo2->puntos += 3;
       stats2->victorias++;
       stats1->derrotas++;
       stats2->goles_favor++;
   } else {
       // Manejo de empates
       equipo1->puntos += 1;
       equipo2->puntos += 1;
       stats1->empates++;
       stats2->empates++;
   }
}

/* Ordenamiento de equipos por puntos */
void ordenar_equipos(Equipo *equipos) {
   for(int i = 0; i < NUM_EQUIPOS - 1; i++) {
       for(int j = 0; j < NUM_EQUIPOS - i - 1; j++) {
           if(equipos[j].puntos < equipos[j + 1].puntos) {
               Equipo temp = equipos[j];
               equipos[j] = equipos[j + 1];
               equipos[j + 1] = temp;
           }
       }
   }
}

/* Simulación de partido eliminatorio */
void jugar_partido_eliminatorio(EquipoEliminatoria *equipo1, EquipoEliminatoria *equipo2) {
   int rend1 = calcular_rendimiento_equipo(&equipo1->equipo.equipo);
   int rend2 = calcular_rendimiento_equipo(&equipo2->equipo.equipo);
   
   printf("Rendimiento %s: %d vs %s: %d\n",
          equipo1->equipo.equipo.nombre, rend1,
          equipo2->equipo.equipo.nombre, rend2);
          
   // Determinación del ganador y actualización de estado
   if(rend1 >= rend2) {
       equipo2->activo = 0;
       equipo1->equipo.stats.victorias++;
       equipo1->equipo.stats.goles_favor++;
   } else {
       equipo1->activo = 0;
       equipo2->equipo.stats.victorias++;
       equipo2->equipo.stats.goles_favor++;
   }
}

/* Mezcla aleatoria para emparejamientos */
void mezclar_equipos_eliminatoria(EquipoEliminatoria *equipos, int n) {
   for(int i = n-1; i > 0; i--) {
       int j = rand() % (i + 1);
       EquipoEliminatoria temp = equipos[i];
       equipos[i] = equipos[j];
       equipos[j] = temp;
   }
}

int main(int argc, char *argv[]) {
   int rank, size;
   
   // Inicialización MPI
   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   
   srand(time(NULL) + rank); // Semilla única por proceso
   
   if(rank == 0) {
       printf("\n=== INICIO DEL TORNEO ===\n\n");
       printf("[FASE DE GRUPOS]\n");
   }
   
   // Inicialización de estructuras locales
   Equipo equipos[NUM_EQUIPOS];
   Estadisticas stats[NUM_EQUIPOS] = {0};
   
   // Inicialización de equipos
   for(int i = 0; i < NUM_EQUIPOS; i++) {
       equipos[i].id = i;
       sprintf(equipos[i].nombre, "Equipo%d_Pais%d", i, rank);
       equipos[i].puntos = 0;
       inicializar_jugadores(equipos[i].jugadores);
   }
   
   // Sincronización para inicio de liga local
   MPI_Barrier(MPI_COMM_WORLD);
   for(int i = 0; i < size; i++) {
       if(rank == i) {
           printf("\nPaís %d: Iniciando liga local\n", rank);
       }
       MPI_Barrier(MPI_COMM_WORLD);
   }
   
   // Liga local: todos contra todos
   int num_partidos = 0;
   for(int i = 0; i < NUM_EQUIPOS; i++) {
       for(int j = i + 1; j < NUM_EQUIPOS; j++) {
           jugar_partido(&equipos[i], &equipos[j], &stats[i], &stats[j]);
           num_partidos++;
       }
   }
   
   ordenar_equipos(equipos);
   
   // Mostrar resultados de liga local sincronizadamente
   MPI_Barrier(MPI_COMM_WORLD);
   for(int i = 0; i < size; i++) {
       if(rank == i) {
           printf("\nPaís %d:\n", rank);
           printf("- Partidos jugados: %d/190\n", num_partidos);
           printf("- Mejor equipo: %s (%d pts)\n", equipos[0].nombre, equipos[0].puntos);
           printf("- Segundo: %s (%d pts)\n", equipos[1].nombre, equipos[1].puntos);
       }
       MPI_Barrier(MPI_COMM_WORLD);
   }
   
   // Preparación de clasificados
   EquipoClasificado clasificados[2];
   for(int i = 0; i < 2; i++) {
       clasificados[i].equipo = equipos[i];
       clasificados[i].pais_origen = rank;
       clasificados[i].stats = stats[i];
   }
   
   // Envío de clasificados al proceso principal
   if(rank != 0) {
       MPI_Send(clasificados, sizeof(EquipoClasificado) * 2, MPI_BYTE, 0, 0, MPI_COMM_WORLD);
   } else {
       printf("\n[CLASIFICADOS A ELIMINATORIAS]\n");
       EquipoClasificado todos_clasificados[NUM_PAISES * 2];
       memcpy(&todos_clasificados[0], clasificados, sizeof(EquipoClasificado) * 2);
       
       // Recolección de clasificados
       for(int i = 1; i < size; i++) {
           MPI_Recv(&todos_clasificados[i * 2], sizeof(EquipoClasificado) * 2, 
                   MPI_BYTE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
           printf("País %d: %s (%d pts) - V:%d E:%d D:%d GF:%d, %s (%d pts) - V:%d E:%d D:%d GF:%d\n", 
                  i,
                  todos_clasificados[i*2].equipo.nombre, 
                  todos_clasificados[i*2].equipo.puntos,
                  todos_clasificados[i*2].stats.victorias,
                  todos_clasificados[i*2].stats.empates,
                  todos_clasificados[i*2].stats.derrotas,
                  todos_clasificados[i*2].stats.goles_favor,
                  todos_clasificados[i*2+1].equipo.nombre,
                  todos_clasificados[i*2+1].equipo.puntos,
                  todos_clasificados[i*2+1].stats.victorias,
                  todos_clasificados[i*2+1].stats.empates,
                  todos_clasificados[i*2+1].stats.derrotas,
                  todos_clasificados[i*2+1].stats.goles_favor);
       }
       
       printf("\n[FASE ELIMINATORIA]\n");
       
       // Preparación fase eliminatoria
       EquipoEliminatoria eliminatoria[NUM_PAISES * 2];
       for(int i = 0; i < NUM_PAISES * 2; i++) {
           eliminatoria[i].equipo = todos_clasificados[i];
           eliminatoria[i].activo = 1;
       }

       // Mezcla aleatoria para emparejamientos
       mezclar_equipos_eliminatoria(eliminatoria, NUM_PAISES * 2);
       
       // Ejecución de rondas eliminatorias
       int ronda = 1;
       int equipos_activos = NUM_PAISES * 2;
       while(equipos_activos > 1) {
           printf("\nRonda %d:\n", ronda++);
           int partidos_jugados = 0;
           
           // Jugar partidos de la ronda
           for(int i = 0; i < NUM_PAISES * 2 - 1; i += 2) {
               if(eliminatoria[i].activo && eliminatoria[i+1].activo) {
                   printf("\nPartido %d:\n", partidos_jugados + 1);
                   printf("%s (País %d) vs %s (País %d)\n", 
                       eliminatoria[i].equipo.equipo.nombre,
                       eliminatoria[i].equipo.pais_origen,
                       eliminatoria[i+1].equipo.equipo.nombre,
                       eliminatoria[i+1].equipo.pais_origen);
                   jugar_partido_eliminatorio(&eliminatoria[i], &eliminatoria[i+1]);
                   partidos_jugados++;
               }
           }
           
           if(partidos_jugados == 0) break;
           
           // Mostrar clasificados a siguiente ronda
           equipos_activos = 0;
           printf("\nClasificados a siguiente ronda:\n");
           for(int i = 0; i < NUM_PAISES * 2; i++) {
               if(eliminatoria[i].activo) {
                   printf("- %s (País %d) | V:%d E:%d D:%d GF:%d\n", 
                       eliminatoria[i].equipo.equipo.nombre,
                       eliminatoria[i].equipo.pais_origen,
                       eliminatoria[i].equipo.stats.victorias,
                       eliminatoria[i].equipo.stats.empates,
                       eliminatoria[i].equipo.stats.derrotas,
                       eliminatoria[i].equipo.stats.goles_favor);
                   equipos_activos++;
               }
           }
       }
       
       // Mostrar campeón
       printf("\n=== CAMPEÓN DEL TORNEO ===\n");
       for(int i = 0; i < NUM_PAISES * 2; i++) {
           if(eliminatoria[i].activo) {
               printf("%s del País %d\n", 
                   eliminatoria[i].equipo.equipo.nombre,
                   eliminatoria[i].equipo.pais_origen);
               printf("Estadísticas finales:\n");
               printf("- Victorias: %d\n", eliminatoria[i].equipo.stats.victorias);
               printf("- Empates: %d\n", eliminatoria[i].equipo.stats.empates);
               printf("- Derrotas: %d\n", eliminatoria[i].equipo.stats.derrotas);
               printf("- Goles a favor: %d\n", eliminatoria[i].equipo.stats.goles_favor);
               break;
           }
       }
       
       printf("\n=== FIN DEL TORNEO ===\n");
   }
   
   MPI_Finalize();
   return 0;
}
