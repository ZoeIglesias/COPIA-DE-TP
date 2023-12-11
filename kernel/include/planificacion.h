#ifndef PLANIFICACION_H_
#define PLANIFICACION_H_

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/temporal.h>


//NUESTRAS
#include "../include/pcb.h"
//#include "kernel.h"
#include "conexion_kernel_cpu.h"
#include "conexion_kernel_fs.h"
#include "sockets/sockets.h"
#include "../../shared/src/contexto_ejecucion/contexto_ejecucion.h"
#include "recursos.h"
#include "../../cpu/include/variables_globales_cpu.h"
#include "../../shared/src/estructuras_compartidas/estructuras_compartidas.h"
#include "archivos_fs.h"
#include "consola.h"



typedef struct{
    char* identificador;
    t_queue* cola_bloqueados;//puede ser una cola de un struct que tiene pcb y motivo de bloqueo
    char* tipo;
}t_cola_block;

typedef struct{
    t_pcb *pcb;
    int tiempo_dormido;
}t_sleep;

// -----------------------------------
//Funciones generales
// -----------------------------------

void inicializar_estructuras();
void inicializar_hilos();
void inicializar_colas_bloqueo();
void crear_cola_del_archivo_abierto(t_pcb *pcb, char *nombre_archivo);
t_cola_block* buscar_cola_del_archivo_abierto(char *nombre_archivo);
t_cola_block* buscar_cola_del_recurso(char *nombre_recurso);
t_cola_block* buscar_cola_block(char *tipo_cola, char *nombre);

void inicializar_semaforos();


typedef enum{
    FIFO,
    ROUND_ROBIN,
    PRIORIDADES
}t_planificador;

// -----------------------------------
//  Implementaciones para los estados
// -----------------------------------

//Colas y listas asociadas a cada estado
extern t_list *lista_new,*lista_ready, *lista_exit,*lista_exec, *lista_block; //lista_block sera una lista con multiples colas 
extern t_cola_block *archivo_abierto;
extern t_cola_block *recurso_existente;
extern t_list *procesosTotales;
extern t_list *pcbs_en_deadlock;

//Semaforos
extern sem_t new_sem;
extern sem_t ready_sem;
extern sem_t exec_sem;
extern sem_t block_sem;
extern sem_t exit_sem;
extern sem_t grado_multiprogramacion_sem;
extern sem_t elemento_exit;
extern sem_t hayPCBsEnNew;
extern sem_t hayPCBsEnReady;
extern sem_t finalizarProceso;
extern sem_t cortoPlazoHablilitado;
extern sem_t largoPlazoHabilitado;
extern sem_t hayPCBsEnBlocked;
extern sem_t solicitud_enviada;
extern sem_t SeEjecutaProceso;
extern sem_t contextoActualizado;
extern sem_t nuevoPcbEnReady;
extern sem_t finDeCiclo;  //Para que una vex termina el ciclo atienda la interrupcion en caso de que haya (DE CPU)
extern sem_t libera_pcb;

//variable global para guardar el pcb que estaba ready a execute, y ejecutar lo que sigue (enviar contex a CPU) 
extern t_pcb *pcb_en_execute; 
extern bool hayPcbEnExec; 
extern bool hayMayorPrioridad;
//puntero que nos servira para eliminar elementos de la cola NEW
extern t_queue *new_queue;

// ------------------------------------
//  Funciones para los estados
// ------------------------------------


//Para poder devolver cosas en consola, utilizo esta lista para saber todos los procesos que estan presentes en el sistema al mismo tiempo
void agregar_a_procesos_totales(t_pcb *nuevo_pcb);
void sacar_de_procesos_totales(t_pcb *pcb_a_sacar);

//Estado: New
void agregar_a_new(t_pcb *pcb);
t_pcb *sacar_de_new();

//Estado: Ready
void agregar_a_ready(t_pcb *pcb);
t_pcb *sacar_de_ready();
void algoritmo_RR(t_pcb *pcb_a_sacar);
void algoritmo_prioridades(t_pcb *pcb_a_sacar);

//Estado: Execute
void agregar_a_execute();
void sacar_de_execute();

//Estado: Exit
void agregar_a_exit(t_pcb *pcb, t_msj_error motivo);
void *sacar_de_exit();

//Estado: Blocked
void agregar_a_blocked(t_pcb *pcb, char *tipo_cola, char *nombre);
t_pcb *sacar_de_blocked(char *tipo_cola, char *nombre); 

// ------------------------------------
//  Implementaciones para Planificacion
// ------------------------------------

void *planificador_largo_plazo(); 
void *planificador_corto_plazo(); 
void *dormir_pcb(t_sleep *pcb_a_dormir);

//void obtener_procesos_ready(t_list *lista_ready, codigo_operacion cod_op); 
void algoritmo_RR(t_pcb *pcb_a_sacar);

//Para cronometrar tiempo en CPU
void semaforoQuantum(struct timespec *tiempo);

void *round_robin();

//analiza si el ultimo proceso tiene mayor prioridad 
bool mayor_prioridad_nuevo(t_pcb* pcb, t_pcb* nuevo_pcb);

//enviar interrupcion a cpu por algun algoritmo que desaloja
void enviar_interrupcion(t_msj_error motivo);

//transforma el mensaje error que es un enum en un char*
char *mensaje_a_string(t_msj_error motivo);

//HILO PF
void *atender_page_fault(void* parametros);

void *chequear_prioridades();

t_planificador planificador();

void algoritmo_planificador(t_planificador alg_planificacion);

int contar_recursos(char** recursos_sistema);

char *listar_pids(t_list *lista_a_referenciar);

int pid_bloqueado(t_cola_block *cola_a_sacar);


#endif