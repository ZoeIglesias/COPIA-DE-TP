#include "../include/conectar_cliente_kernel.h"

void conectar_cliente_kernel_dispatch(int server_cpu_dispatch)
{
    cliente_kernel_dispatch = esperar_cliente(server_cpu_dispatch,cpu); 

    if(!verificar_cliente(cliente_kernel_dispatch,cpu)){
        log_error(cpu, "Ocurrio un error al reconocer al cliente KERNEL");
        exit(1);
    }
    
    //creo un hilo para ESPERAR LOS CONTEXTOS
    pthread_t conexion_dispatch;
    pthread_create(&conexion_dispatch, NULL,(void*)esperar_contextos, &cliente_kernel_dispatch);
 
}

void conectar_cliente_kernel_interrupt(int server_cpu_interrupt){

    cliente_kernel_interrupt = esperar_cliente(server_cpu_interrupt,cpu);

    if(!verificar_cliente(cliente_kernel_interrupt,cpu)){
        log_error(cpu, "Ocurrio un error al reconocer al cliente KERNEL");
        exit(1);
    }
    
}

void *esperar_contextos(int *socket_cliente_kernel)
{

    while(1)
    {   
        
        codigo_operacion cod_op = recibir_operacion(*socket_cliente_kernel);
        log_info(cpu,"Lrecibo operacion");
         switch(cod_op)
        {
        case CONTEXTO_EJECUCION: 

            log_info(cpu,"Llego un contexto de ejecucion");
            t_contexto_ejecucion *ctx = recibir_contexto(*socket_cliente_kernel);
            log_info(cpu,"Contexto deserializado.");
            comenzar_ciclo_instruccion(ctx);
            break;
            
        default:
        
            log_error(cpu,"No se reconocio la operacion.");
            break; 

        }
    }
}

void devolver_contexto(t_contexto_ejecucion *ctx,codigo_operacion cod_op)//SIN PARAMETROS
{
    t_paquete *paquete = crear_paquete(cod_op);
    
    serializar_contexto(paquete,ctx);
    
    enviar_paquete(paquete,cliente_kernel_dispatch);
    
}

void devolver_contexto_con_parametro(t_contexto_ejecucion *ctx,codigo_operacion cod_op, char *par1)//CON UN PARAMETRO
{
    int tamanio_parametro = strlen(par1);

    t_paquete *paquete = crear_paquete(cod_op);

    serializar_contexto(paquete,ctx);

    agregar_a_paquete(paquete, &tamanio_parametro, sizeof(int));

    agregar_a_paquete(paquete,par1,tamanio_parametro);

    enviar_paquete(paquete,cliente_kernel_dispatch);
}

void devolver_contexto_con_int(t_contexto_ejecucion *ctx,codigo_operacion cod_op, char *par1, int dir_fisica)//CON DOS PARAMETROS (char, int)
{
    int tamanio_parametro = strlen(par1);

    t_paquete *paquete = crear_paquete(cod_op);

    serializar_contexto(paquete,ctx);

    agregar_a_paquete(paquete, &tamanio_parametro, sizeof(int));

    agregar_a_paquete(paquete,par1,tamanio_parametro);

    agregar_a_paquete(paquete,&dir_fisica,tamanio_parametro);

    enviar_paquete(paquete,cliente_kernel_dispatch);
}

void devolver_contexto_con_parametros(t_contexto_ejecucion *ctx,codigo_operacion cod_op, char *par1, char *par2)//CON DOS PARAMETROS
{
    int tamanio_parametro1 = strlen(par1);
    int tamanio_parametro2 = strlen(par2);

    t_paquete *paquete = crear_paquete(cod_op);

    serializar_contexto(paquete,ctx);

    agregar_a_paquete(paquete, &tamanio_parametro1, sizeof(int));

    agregar_a_paquete(paquete,par1,tamanio_parametro1);

    agregar_a_paquete(paquete, &tamanio_parametro2, sizeof(int));

    agregar_a_paquete(paquete,par2,tamanio_parametro2);

    enviar_paquete(paquete,cliente_kernel_dispatch);
}

void devolver_contexto_page_fault(t_contexto_ejecucion *ctx, int num_pagina)//CON UN PARAMETRO ESPECIFICO --> NUMERO DE PAGINA
{
    t_paquete *paquete = crear_paquete(PAGE_FAULT);

    serializar_contexto(paquete,ctx);

    agregar_a_paquete(paquete, &num_pagina, sizeof(int));

    enviar_paquete(paquete,cliente_kernel_dispatch);
}



