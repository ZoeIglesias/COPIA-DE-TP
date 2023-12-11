#ifndef VARIABLES_GLOBALES_CPU_H_
#define VARIABLES_GLOBALES_CPU_H_

//NUESTRAS
#include "../../shared/src/sockets/sockets.h"
#include "../../shared/src/configs/configs.h"

//VARIABLES GLOBALES//

//CONFIG Y LOG
extern t_log* logger_cpu;
extern t_log* cpu;
extern t_config* config_cpu;

//SOCKETS
extern int cliente_kernel_dispatch;
extern int cliente_kernel_interrupt;
extern int tam_pag;


#endif