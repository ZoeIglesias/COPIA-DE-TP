#include "../include/estructuras_memoria.h"

void *memoria_usuario;
t_list *tablas_procesos_globales;
t_list *tablas_por_proceso;
sem_t espacio_usuario;

void iniciar_estructuras()
{

    memoria_usuario = calloc(1, tam_memoria());// inicializa todos sus bytes a cero.
    sem_init(&espacio_usuario, 0, 1);
    sem_init(&terminaron_conexiones, 0, 0);
    log_info(memoria, "Memoria para el espacio de usuario creada!");
    
    tablas_por_proceso = list_create();//t_paginas
    //tablas_procesos_globales = list_create();//t_tabla_proceso
    procesos_de_memoria = list_create();
    lista_instrucciones_globales = list_create();

    vector_marcos = calloc(tam_memoria() / tam_pagina(), sizeof(int));

    for(int i=0; i< tam_memoria() / tam_pagina(); i++) //creo los marcos/paginas 
    {
       //t_pagina* pagina = NULL;
       //list_add(tablas_por_proceso,pagina);
       vector_marcos[i] =  0;
    }
    log_info(memoria, "Tabla de planificacion de %d marcos creada",(tam_memoria() / tam_pagina()));

}

int obtener_cant_pags(int tam_proceso)
{
     return ceil(tam_proceso / tam_pagina());
}

int asignar_frame_libre()
{
     for(int i=0; i<tam_memoria() / tam_pagina(); i++)
     {
          if(vector_marcos[i] == 0)
          {
               return i;
               vector_marcos[i] = 1;
          }
     }
     exit(EXIT_FAILURE);
}

void inicializar_tabla_paginas(t_proceso_memoria *proceso)
{
     
     int cant_pags = obtener_cant_pags(proceso->tamanio_proceso_bytes);
     
     int* bloques_asignados = solicitar_SWAP(proceso);     
     
     t_tabla_por_proceso *tabla_proceso = malloc(sizeof(t_tabla_por_proceso)); 
     tabla_proceso->pid = proceso->pcb->contexto_ejecucion->pid;
     tabla_proceso->tabla_paginas = list_create();

     int bloque;
     int posicion_en_swap;


     for(int i = 0; i < cant_pags; i++)
     {
          t_pagina *pagina = (t_pagina*)malloc(sizeof(t_pagina));
          pagina -> marco = -1;
          pagina -> presencia = false;
          pagina -> modificado = false;
          bloque = bloques_asignados[i];
          posicion_en_swap = bloques_asignados[i] * tam_pagina();
          pagina -> pos_en_swap = posicion_en_swap;
          pagina -> tiempo_carga = 0; 
          pagina -> ultima_referencia = 0;
          pagina -> PID = proceso->pcb->contexto_ejecucion->pid;
          list_add(tabla_proceso->tabla_paginas, pagina);
     }
    
     list_add(tablas_por_proceso, tabla_proceso);

     log_info(logger_memoria,"PID: %d - Tamaño: %d",proceso->pcb->contexto_ejecucion->pid, cant_pags);
     
     
}

void finalizar_tabla_paginas(t_proceso_memoria *proceso)
{

     liberar_SWAP(proceso); //aca se hacer el free de bloques

     int PID = proceso->pcb->contexto_ejecucion->pid;
     
     t_tabla_por_proceso* tabla_proceso = obtener_tabla_proceso(PID);

     if(tabla_proceso != NULL)
     {    
          for(int i=0; i < (list_size(tabla_proceso->tabla_paginas)); i++)
          {    
               free(list_remove(tabla_proceso->tabla_paginas,i));
               
          }
          free(list_remove(tablas_por_proceso, indice_en_tabla(tabla_proceso)));

          
          log_info(logger_memoria,"PID: %d - Tamaño: %d",PID, obtener_cant_pags(proceso -> tamanio_proceso_bytes));

          liberar_frames_asignados(proceso);
     }

     log_info(memoria,"No se ha encontrado la tabla de paginas asociada al proceso de PID: %d", PID);
     
}


void liberar_frames_asignados(t_proceso_memoria *proceso)
{
     int pid = proceso -> pcb -> contexto_ejecucion -> pid;

     t_list *tabla_paginas = obtener_tabla_pagina(pid);

     for(int i=0; i< (list_size(tabla_paginas)); i++)
     {
          t_pagina *pagina = list_get(tabla_paginas,i);

          if(pagina -> presencia)
          {
               int marco = pagina -> marco;
               vector_marcos[marco] = 0;

               if(pagina -> modificado)
               {
                    t_paquete *paquete = crear_paquete(ESCRIBIR_SWAP);
                    void* contenido_pagina;
                    memcpy(contenido_pagina, memoria_usuario + tam_pagina() * marco, tam_pagina());
                    agregar_a_paquete(paquete, &contenido_pagina, tam_pagina());
                    agregar_a_paquete(paquete, &pagina -> pos_en_swap, sizeof(int));
                    enviar_paquete(paquete, conexion_filesystem);
                    pagina -> modificado = 0;
                    log_info(logger_memoria, "SWAP OUT - PID: %d - Marco: %d - Page Out: %d-%d", pid, marco, pid, i);
               }

               pagina -> presencia = 0;
          }
     }
}

void liberar_instrucciones_proceso(t_instrucciones_proceso *instrucciones_proceso)
{
     for(int i=0; i<(list_size(instrucciones_proceso -> lista_instrucciones)); i++)
     {
          t_instruccion *instruccion = list_get(instrucciones_proceso -> lista_instrucciones, i);
          liberar_instruccion(instruccion);
     }

     bool resultado = list_remove_element(lista_instrucciones_globales,instrucciones_proceso);

     if(!resultado)
     {
          log_error(memoria,"No se encontro la estructura de instrucciones del proceso a liberar");
     }

     free(instrucciones_proceso);
}

//ALGORITMOS
int aplicar_algoritmo_FIFO()
{
     int victima;
     t_list *paginas_presentes = obtener_paginas_en_memoria();
     t_pagina* pagina = ((t_pagina*)list_get(paginas_presentes,0));
     time_t tiempo_primer_elemento = pagina -> tiempo_carga;

     for(int i=0; i< (list_size(paginas_presentes)); i++)
     {
       time_t tiempo_otro_elemento = ((t_pagina*)list_get(paginas_presentes,i)) -> tiempo_carga;
       if(tiempo_otro_elemento < tiempo_primer_elemento)
       {
          tiempo_primer_elemento = tiempo_otro_elemento;
          victima = i;
       }
     }

     return victima;
}

int aplicar_algoritmo_LRU()
{
     int victima;
     time_t acceso_primer_elemento = ((t_pagina*)list_get(tablas_por_proceso,0)) -> ultima_referencia;

     for(int i=0; i< (list_size(tablas_por_proceso)); i++)
     {
          time_t acceso_otro_elemento = ((t_pagina*)list_get(tablas_por_proceso,i)) -> ultima_referencia;
          if(acceso_otro_elemento > acceso_primer_elemento)//un valor de time_t mayor representa un momento más antiguo en el tiempo.
          {
               acceso_primer_elemento = acceso_otro_elemento;
               victima = i;
          }
     }
     return victima;
}



//ATAJOS
t_list *obtener_paginas_en_memoria()
{
     t_list *paginas_en_memoria = list_create();

     for(int i=0; i< (list_size(tablas_por_proceso));i++)
     {
          t_pagina *pagina = list_get(tablas_por_proceso,i);
          if(pagina -> presencia == 1)
          {
               list_add(paginas_en_memoria, pagina);
          }
     }

     return paginas_en_memoria;
}

t_pagina *obtener_pagina(int frame)
{
     t_list *pags_en_memoria =  obtener_paginas_en_memoria();

     for(int i=0; i < (list_size(pags_en_memoria)); i++)
     {
          t_pagina *pagina = list_get(pags_en_memoria,i);

          if(pagina -> marco == frame)
          {
               return pagina;
          }
     }

     exit(EXIT_FAILURE); //Si llega a devolver esto es porque hubo un error
}

t_list *obtener_tabla_pagina(int pid)
{
     int cant_elem_list = list_size(tablas_por_proceso);
     t_tabla_por_proceso *tabla_pagina_proceso;

     for(int i=0 ; i< cant_elem_list ; i++){
          tabla_pagina_proceso = list_get(tablas_por_proceso,i);
          if(tabla_pagina_proceso->pid == pid){
               return tabla_pagina_proceso->tabla_paginas;
          }
     }
     exit(EXIT_FAILURE);
}

int obtener_frame(int num_pag, int pid)
{
     t_list *tabla_paginas = obtener_tabla_pagina(pid);

     t_pagina *pagina = list_get(tabla_paginas,num_pag);

     return pagina -> marco;
}

int get_index(int pid, int frame)
{    
     t_list *tabla_paginas = obtener_tabla_pagina(pid);

    for(int i=0; i<(list_size(tabla_paginas)); i++)
    {   
        t_pagina *pagina = list_get(tablas_por_proceso, i);
        if(pagina -> marco == frame)
        {
            return i;
        }
    }

    exit(EXIT_FAILURE);
}

t_tabla_por_proceso *obtener_tabla_proceso(int pid_asociado)
{   
     for(int i=0; i<(list_size(tablas_por_proceso));i++)
     {
          t_tabla_por_proceso *tabla_proceso = list_get(tablas_por_proceso,i);
          if(tabla_proceso -> pid == pid_asociado)
          {
               return tabla_proceso;
          }
     }
     exit(EXIT_FAILURE);
}

int indice_en_tabla(t_tabla_por_proceso *tabla_proceso){

     for(int i=0; i < (list_size(tablas_por_proceso)); i++){
          t_tabla_por_proceso *tabla_aux = list_get(tablas_por_proceso,i);
          if( tabla_aux -> pid == (tabla_proceso -> pid)){
               return i;
          }
     }

     exit(EXIT_FAILURE); //si llego aca error fatal
}






