#include "../include/leer_configs.h"

t_config* crear_config(char* config_path){
    return config_create(config_path);
}

void destruir_config(){
    config_destroy(config_cpu);
}

char *ip_memoria(){
    return config_get_string_value(config_cpu, "IP_MEMORIA");
} 

char *puerto_memoria(){
    return config_get_string_value(config_cpu, "PUERTO_MEMORIA");
} 

char *puerto_escucha_dispatch(){
    return config_get_string_value(config_cpu, "PUERTO_ESCUCHA_DISPATCH");
} 

char *puerto_escucha_interrupt(){
    return config_get_string_value(config_cpu, "PUERTO_ESCUCHA_INTERRUPT");
} 
