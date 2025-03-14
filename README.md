# Simulador de Sistema Operativo - tp-2023-2c-Los-Voltionautas

Este proyecto fue desarrollado como parte de la materia **Sistemas Operativos**, con el objetivo de simular el funcionamiento de un sistema operativo real. Implementamos los principales módulos que conforman un SO: **CPU, Kernel, Memoria y Filesystem**.

## 🛠️ Tecnologías utilizadas
- **Lenguaje:** C
- **Sistema Operativo:** Linux (ejecutado en una máquina virtual con VirtualBox)

## 📂 Módulos del Proyecto
### 1️⃣ CPU
  - Encargado de ejecutar los procesos según la planificación establecida.
  - Comunicación con el Kernel para recibir, interpretar y ejecutar las instrucciones.
   
### 2️⃣ Kernel
  - Actúa como el coordinador principal del sistema y encargado de iniciar los procesos del sistema.
  - Maneja la comunicación entre los módulos (CPU, Memoria y Filesystem).
  - Implementa algoritmos de planificación y sincronización.
   
### 3️⃣ Memoria
  - Administra la asignación de memoria a los procesos.
  - Encargada de brindarle las instrucciones a ejecutar a la CPU y recibir instrucciones de la misma para leer y/o escribir los datos del proceso en ejecución.
  - Esquema de paginación bajo demanda.
   
### 4️⃣ Filesystem
  - Controla el almacenamiento y recuperación de archivos.
  - Simula una estructura de sistema de archivos con operaciones básicas.
