#include "../include/leer_configs.h"

t_config* crear_config(char* config_path){
    return config_create(config_path);
}

void destruir_config(){
    config_destroy(config_kernel);
}

char *ip_memoria(){
    return config_get_string_value(config_kernel, "IP_MEMORIA");
} 

char *puerto_memoria(){
    return config_get_string_value(config_kernel, "PUERTO_MEMORIA");
} 

char *ip_filesystem(){
    return config_get_string_value(config_kernel, "IP_FILESYSTEM");
} 

char *puerto_filesystem(){
    return config_get_string_value(config_kernel, "PUERTO_FILESYSTEM");
} 

char *ip_cpu(){
    return config_get_string_value(config_kernel, "IP_CPU");
} 

char *puerto_cpu_dispatch(){
    return config_get_string_value(config_kernel, "PUERTO_CPU_DISPATCH");
} 

char *puerto_cpu_interrupt(){
    return config_get_string_value(config_kernel, "PUERTO_CPU_INTERRUPT");
} 

char *algoritmo_planificacion(){
    return config_get_string_value(config_kernel, "ALGORITMO_PLANIFICACION");
} 

int quantum(){
    return config_get_int_value(config_kernel, "QUANTUM");
}

char **recursos_config(){
    return config_get_array_value(config_kernel, "RECURSOS");
}

char **instancias_recursos_config(){
    return config_get_array_value(config_kernel, "INSTANCIAS_RECURSOS");
}

int grado_multiprogramacion(){
    return config_get_int_value(config_kernel, "GRADO_MULTIPROGRAMACION_INI");
}