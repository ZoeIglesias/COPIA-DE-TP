#include "../include/peticiones.h"


void iniciar_semaforos(){
    sem_init(&sem_bloques,0,1);
    sem_init(&sem_fat,0,1);
}


int abrir_archivo(char* nombreArchivo){
    t_FCB * fcb = buscarFCB(nombreArchivo);
    if(fcb != NULL){
        log_abrir_archivo(nombreArchivo);
        return fcb->TAMANIO_ARCHIVO;
    }
    else{
        crear_fcb(nombreArchivo);
        return abrir_archivo(nombreArchivo);
    }
}

bool existe_fcb(char* nombreArchivo){
    return buscarFCB(nombreArchivo) != NULL;
}

void truncar_fcb(char* nombreArchivo, int longitud){
    t_FCB* fcb = buscarFCB(nombreArchivo);
    int diff_bloques;
    if(longitud == fcb->TAMANIO_ARCHIVO){ 
        return;
        //return longitud;
    }
    if(longitud > fcb->TAMANIO_ARCHIVO){
        diff_bloques = ceil((longitud - fcb->TAMANIO_ARCHIVO) / tam_bloque());
        agregar_bloques(fcb, diff_bloques);
        fcb->TAMANIO_ARCHIVO = longitud;
    }

    if(longitud < fcb->TAMANIO_ARCHIVO){
        diff_bloques = floor((fcb->TAMANIO_ARCHIVO - longitud) / tam_bloque());
        sacar_bloques(fcb, diff_bloques);
        fcb->TAMANIO_ARCHIVO = longitud;
    }
    actualizar_fcb(fcb->NOMBRE_ARCHIVO, longitud, fcb->BLOQUE_INICIAL);
    actualizarFAT();
    log_truncar_archivo(nombreArchivo, longitud);

    return;
}

void crear_fcb(char* nombre_archivo){    
    char* linea = string_new();
    FILE* archivo;
    char* extension = "fcb";
    //t_FCB* nuevo_fcb = (t_FCB*) malloc(sizeof(t_FCB));

    char* path_archivo = string_new();

    string_append_with_format(&path_archivo, "%s/%s.%s", path_fcb(), nombre_archivo,extension);
    
    archivo = fopen(path_archivo, "w+");
    
    string_append_with_format(&linea, "%s=%s\n", "NOMBRE_ARCHIVO", nombre_archivo);
    string_append_with_format(&linea, "%s=%s\n", "TAMANIO_ARCHIVO", "0"); 
    string_append_with_format(&linea, "%s=%s\n", "BLOQUE_INICIAL", "0");

    fwrite(linea, strlen(linea), 1, archivo);   
    
    fclose(archivo);
/*
    nuevo_fcb->NOMBRE_ARCHIVO = nombreArchivo;
    nuevo_fcb->TAMANIO_ARCHIVO = 0;
    nuevo_fcb->BLOQUE_INICIAL = NULL
    list_add(fcbs, nuevo_fcb);
*/
    free(path_archivo);
    log_crear_archivo(nombre_archivo);
}

void agregar_bloques(t_FCB* fcb,int diff_bloques){
    
    int bloqueActual;
    int bloqueParaLog;

    if(fcb->BLOQUE_INICIAL == 0){ //puse 0 en vez de NULL para que sea mas directo (NULL es de tipo ((void *)0) y soluciona el warning)
        sem_wait(&sem_fat);
        fcb->BLOQUE_INICIAL = bloque_libre_FAT();
        fat[fcb->BLOQUE_INICIAL] = UINT32_MAX;
        sem_post(&sem_fat);
        log_acceso_FAT(fcb->BLOQUE_INICIAL, fat[fcb->BLOQUE_INICIAL]);
    }

    bloqueActual = fcb->BLOQUE_INICIAL;

    while( fat[bloqueActual] != UINT32_MAX){
        sem_wait(&sem_fat);
        bloqueActual = (int) fat[bloqueActual];
        sem_post(&sem_fat);
    }

    while(diff_bloques > 0){
        bloqueParaLog = bloqueActual;
        sem_wait(&sem_fat);
        fat[bloqueActual] = (uint32_t) bloque_libre_FAT();
        bloqueActual = (int) fat[bloqueActual];
        sem_post(&sem_fat);
        diff_bloques--;
        log_acceso_FAT(bloqueParaLog, fat[bloqueParaLog]);
    }
    sem_wait(&sem_fat);
    fat[bloqueActual] = UINT32_MAX;
    sem_post(&sem_fat);
    log_acceso_FAT(bloqueActual, UINT32_MAX);
};

void sacar_bloques(t_FCB* fcb,int diff_bloques){
    int bloqueActual;
    int bloqueALiberar;
    int totalFinalBloques = ceil(fcb->TAMANIO_ARCHIVO / tam_bloque())-diff_bloques;
    bloqueActual = fcb->BLOQUE_INICIAL;
    for(int i=1; i<totalFinalBloques; i++){
        sem_wait(&sem_fat);
        bloqueActual = (int) fat[bloqueActual];
        sem_post(&sem_fat);
    }
    sem_wait(&sem_fat);
    bloqueALiberar = (int) fat[bloqueActual];
    fat[bloqueActual] = UINT32_MAX;
    sem_post(&sem_fat);

    log_acceso_FAT(bloqueActual, UINT32_MAX);
    
    while(diff_bloques > 0){
        bloqueActual = bloqueALiberar;
        sem_wait(&sem_fat);
        bloqueALiberar = fat[bloqueActual];
        fat[bloqueActual] = (uint32_t) 0;
        sem_post(&sem_fat);
        diff_bloques--;
        log_acceso_FAT(bloqueActual, (uint32_t) 0);
    }
}

//la funcion estaba tipo int, pero como no retornaba nada y en ningun lado pedia algo que retornada la cambie a void
void actualizar_fcb(char* nombreArchivo, int longitud, int bloque_inicial){
    char* extension = "fcb";

    char* path = string_new();

    string_append_with_format(&path, "%s/%s.%s", path_fcb(), nombreArchivo,extension);
    FILE *archivoFCB = fopen(path, "w+");
    char* linea = string_new();
    string_append_with_format(&linea, "%s=%s\n", "NOMBRE_ARCHIVO", nombreArchivo);
    string_append_with_format(&linea, "%s=%d\n", "TAMANIO_ARCHIVO", longitud); 
    string_append_with_format(&linea, "%s=%d\n", "BLOQUE_INICIAL", bloque_inicial);
    fwrite(linea, strlen(linea), 1, archivoFCB);   

    fclose(archivoFCB);
    free(path);
}

int bloque_libre_FAT(){
    int cantidadBloques = cant_bloques_total()-cant_bloques_swap();
    for(int i = 1; i<cantidadBloques; i++){
        if(fat[i] == 0){
            return i;
        }
    }
    exit(EXIT_FAILURE); //caso extremo que nunca llega, pero faltaba si es que no entra al for
}

int bloque_libre_SWAP(){
    int cantidadBloques = cant_bloques_swap();
    int tamBloque = tam_bloque();
    char contenido;
    for(int i = 0; i < cantidadBloques; i++){
        memcpy(&contenido,bloques+ tamBloque * i *sizeof(char), sizeof(char));
        if(contenido == '\244'){
            return i;
        }
    }
    exit(EXIT_FAILURE); //caso extremo que nunca llega, pero faltaba si es que no entra al for
}

int* asignar_bloques_SWAP(int cantidad){
    int* bloquesAsignados = (int*)malloc(cantidad * sizeof(int));
    int num_bloque;
    int tamBloque = tam_bloque();
    for(int i = 0; i<cantidad; i++){
        sem_wait(&sem_bloques);
        num_bloque = bloque_libre_SWAP();
        memset(bloques + tamBloque * num_bloque*sizeof(char), '\0', sizeof(char));
        sem_post(&sem_bloques);
        log_acceso_Bloque_SWAP(num_bloque);
    }

    actualizarBLOQUES();
    return bloquesAsignados;
}

void liberar_bloques_SWAP(int* bloquesAsignados, int cantidad_bloques){
    int tamBloque = tam_bloque();
    for(int i =0; i<cantidad_bloques;i++){
        sem_wait(&sem_bloques);
        log_info(filesystem, "%d", bloquesAsignados[i]);
        memset(bloques + tamBloque * bloquesAsignados[i]*sizeof(char), '\244', tamBloque * sizeof(char));
        sem_post(&sem_bloques);
        log_acceso_Bloque_SWAP(bloquesAsignados[i]);
    }
    actualizarBLOQUES();
}

int buscar_bloque_archivo(int bloque_puntero, char* nombre_archivo){
    int bloque_fat = bloque_archivo_FAT(nombre_archivo, bloque_puntero);
    int bloque_fs = cant_bloques_swap() + bloque_fat;
    return bloque_fs;
}


void acceder_bloque_archivo(int bloque_fs, char* nombre_archivo){
    int bloque_fat = bloque_fs - cant_bloques_swap();
    log_acceso_Bloque_Archivo(nombre_archivo, bloque_fat, bloque_fs);
}

int bloque_archivo_FAT(char* nombre_archivo, int posicion){
    t_FCB* fcb = buscarFCB(nombre_archivo);
    int bloque_actual = fcb->BLOQUE_INICIAL;
    for(int i = 0; i< posicion; i++){
        if(bloque_actual == UINT32_MAX){
            return -1;
        }
        bloque_actual = fat[bloque_actual];
    }
    log_acceso_FAT(bloque_actual, fat[bloque_actual]);
    return bloque_actual;
}

void escribir_bloque(void* contenido_pagina,int num_bloque){
    int tamBloque = tam_bloque();
    memcpy(bloques + tamBloque * num_bloque *sizeof(char), contenido_pagina, tam_bloque()* sizeof(char));
    actualizarBLOQUES();
}

void* leer_bloque(int num_bloque){
    int tamBloque = tam_bloque();
    void* contenido_bloque = (void*)malloc(sizeof(void)); //le doy espacio en memoria porque me tiro un warning
    memcpy(contenido_bloque, bloques + tamBloque * num_bloque * sizeof(char), tam_bloque()* sizeof(char));
    return contenido_bloque;
}