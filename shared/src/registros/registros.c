#include "registros.h"

void asignar_a_registro(char* registro, char* valor, t_registros *registros)
{
     uint32_t *registro_obtenido = obtener_registro(registro, registros);
     
     uint32_t valor_entero = strtoul(valor, NULL, 10);//por que le valor lo recibe como char*

     
     if(registro_obtenido == NULL)
     {  
        return;//No se que retornar
     }
     
        *registro_obtenido = valor_entero;
     
        return;
}
 
uint32_t *obtener_registro(const char *nombre, t_registros *registros)
{
    if(strcmp(nombre, "AX") == 0)
        return &(registros->AX);
    else if(strcmp(nombre,"BX") == 0)
        return &(registros->BX);
    else if(strcmp(nombre,"CX") == 0)
        return &(registros->CX);
    else if(strcmp(nombre,"DX") == 0)
        return &(registros->DX);
    else return NULL;

}

void operar_registros(codigo_operacion operacion, char* registro_destino, char* registro_origen, t_registros *registros)
{

    uint32_t *reg_destino  = obtener_registro(registro_destino, registros); 
    uint32_t *reg_origen = obtener_registro(registro_origen , registros);

    if (reg_destino != NULL && reg_origen != NULL) {
        switch (operacion) {
            case SUM:
                *reg_destino += *reg_origen;
                break;
            case SUB:
                *reg_destino -= *reg_origen;
                break;
            default:
                break;
        }
    }

}

void crear_registros(t_pcb* pcb){
    
    pcb->contexto_ejecucion->registros = malloc(sizeof(t_registros));
    pcb->contexto_ejecucion->registros->AX = (uint32_t)0;
    pcb->contexto_ejecucion->registros->BX = (uint32_t)0;
    pcb->contexto_ejecucion->registros->CX = (uint32_t)0;
    pcb->contexto_ejecucion->registros->DX = (uint32_t)0;

}

