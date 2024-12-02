#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include <time.h>
#include <string.h>

#define NUM_JUGADORES 25
#define NUM_EQUIPOS 20
#define NUM_PAISES 8

typedef struct {
   int id;
   int rendimiento;
} Jugador;

typedef struct {
   int id;
   char nombre[50];
   Jugador jugadores[NUM_JUGADORES];
   int puntos;
} Equipo;

typedef struct {
   Equipo equipo;
   int pais_origen;
} EquipoClasificado;

typedef struct {
   EquipoClasificado equipo;
   int activo;
} EquipoEliminatoria;

void inicializar_jugadores(Jugador *jugadores) {
   for(int i = 0; i < NUM_JUGADORES; i++) {
       jugadores[i].id = i;
       jugadores[i].rendimiento = 0;
   }
}

int calcular_rendimiento_equipo(Equipo *equipo) {
   int total = 0;
   #pragma omp parallel for reduction(+:total)
   for(int i = 0; i < NUM_JUGADORES; i++) {
       equipo->jugadores[i].rendimiento = rand() % 99 + 1;
       total += equipo->jugadores[i].rendimiento;
   }
   return total;
}

void jugar_partido(Equipo *equipo1, Equipo *equipo2) {
   int rendimiento1 = calcular_rendimiento_equipo(equipo1);
   int rendimiento2 = calcular_rendimiento_equipo(equipo2);
   
   if(rendimiento1 > rendimiento2) {
       equipo1->puntos += 3;
   } else if(rendimiento2 > rendimiento1) {
       equipo2->puntos += 3;
   } else {
       equipo1->puntos += 1;
       equipo2->puntos += 1;
   }
}

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

void jugar_partido_eliminatorio(EquipoEliminatoria *equipo1, EquipoEliminatoria *equipo2) {
   int rend1 = calcular_rendimiento_equipo(&equipo1->equipo.equipo);
   int rend2 = calcular_rendimiento_equipo(&equipo2->equipo.equipo);
   
   if(rend1 >= rend2) {
       equipo2->activo = 0;
   } else {
       equipo1->activo = 0;
   }
}

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
   srand(time(NULL));
   
   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   
   Equipo equipos[NUM_EQUIPOS];
   
   for(int i = 0; i < NUM_EQUIPOS; i++) {
       equipos[i].id = i;
       sprintf(equipos[i].nombre, "Equipo%d_Pais%d", i, rank);
       equipos[i].puntos = 0;
       inicializar_jugadores(equipos[i].jugadores);
   }
   
   // Liga local
   for(int i = 0; i < NUM_EQUIPOS; i++) {
       for(int j = i + 1; j < NUM_EQUIPOS; j++) {
           jugar_partido(&equipos[i], &equipos[j]);
       }
   }
   
   ordenar_equipos(equipos);
   
   // Preparar equipos clasificados
   EquipoClasificado clasificados[2];
   for(int i = 0; i < 2; i++) {
       clasificados[i].equipo = equipos[i];
       clasificados[i].pais_origen = rank;
   }
   
   // Enviar clasificados al proceso 0
   if(rank != 0) {
       MPI_Send(clasificados, sizeof(EquipoClasificado) * 2, MPI_BYTE, 0, 0, MPI_COMM_WORLD);
   } else {
       EquipoClasificado todos_clasificados[NUM_PAISES * 2];
       memcpy(&todos_clasificados[0], clasificados, sizeof(EquipoClasificado) * 2);
       
       for(int i = 1; i < size; i++) {
           MPI_Recv(&todos_clasificados[i * 2], sizeof(EquipoClasificado) * 2, 
                   MPI_BYTE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
       }
       
       // Fase eliminatoria
       EquipoEliminatoria eliminatoria[NUM_PAISES * 2];
       for(int i = 0; i < NUM_PAISES * 2; i++) {
           eliminatoria[i].equipo = todos_clasificados[i];
           eliminatoria[i].activo = 1;
       }

       printf("\nEmparejamientos aleatorios - Fase eliminatoria:\n");
       mezclar_equipos_eliminatoria(eliminatoria, NUM_PAISES * 2);
       
       int ronda = 1;
       int equipos_activos = NUM_PAISES * 2;
       while(equipos_activos > 1) {
           printf("\nRonda %d:\n", ronda++);
           for(int i = 0; i < NUM_PAISES * 2; i += 2) {
               if(eliminatoria[i].activo && eliminatoria[i+1].activo) {
                   printf("Partido: %s (País %d) vs %s (País %d)\n", 
                       eliminatoria[i].equipo.equipo.nombre,
                       eliminatoria[i].equipo.pais_origen,
                       eliminatoria[i+1].equipo.equipo.nombre,
                       eliminatoria[i+1].equipo.pais_origen);
                   jugar_partido_eliminatorio(&eliminatoria[i], &eliminatoria[i+1]);
               }
           }
           
           equipos_activos = 0;
           printf("\nClasificados siguiente ronda:\n");
           for(int i = 0; i < NUM_PAISES * 2; i++) {
               if(eliminatoria[i].activo) {
                   printf("%s (País %d)\n", 
                       eliminatoria[i].equipo.equipo.nombre,
                       eliminatoria[i].equipo.pais_origen);
                   equipos_activos++;
               }
           }
       }
       
       // Imprimir campeón
       for(int i = 0; i < NUM_PAISES * 2; i++) {
           if(eliminatoria[i].activo) {
               printf("\n¡CAMPEÓN DEL TORNEO!\n");
               printf("%s del País %d\n", 
                   eliminatoria[i].equipo.equipo.nombre,
                   eliminatoria[i].equipo.pais_origen);
               break;
           }
       }
   }
   
   MPI_Finalize();
   return 0;
}
