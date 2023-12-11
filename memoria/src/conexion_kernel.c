#include "../include/conexion_kernel.h"


void conectar_con_kernel(int *socket_kernel)
{   

    int pid;
    int desplazamiento;
  
    while(1)
    {
        t_buffer *buffer = crear_buffer();
        codigo_operacion cod_op = recibir_operacion(*socket_kernel);
        buffer->stream = recibir_buffer(&(buffer->size), *socket_kernel);
        desplazamiento = 0;
        switch (cod_op)
        {
        case CREAR_PROCESO:

            crear_proceso(buffer);

            break;
        
        case CARGAR_PAGINA:

            int num_pagina;
            
            memcpy(&num_pagina, buffer->stream + desplazamiento, sizeof(int));
            desplazamiento += sizeof(int);
            memcpy(&pid, buffer->stream + desplazamiento , sizeof(int));
            desplazamiento += sizeof(int);
            solicitar_pagina(num_pagina, pid);
            log_info(logger_memoria,"PID: %d - Pagina: %d - Marco: %d",pid,num_pagina,obtener_frame(num_pagina,pid));
            t_paquete *paquete = crear_paquete(OK);
            enviar_paquete(paquete,*socket_kernel);
            
            break;
        
        case LIBERAR_ESTRUCTURAS:

            memcpy(&pid, buffer->stream + desplazamiento , sizeof(int));
            finalizar_proceso(pid);
           //  sem_post(&libera_pcb);

            break;
        case -1:
        
            log_error(memoria, "El cliente KERNEL se desconectó.");
            break;
        
        default:
            break;
        }
        liberar_buffer(buffer);
    }
   
    

    
}
