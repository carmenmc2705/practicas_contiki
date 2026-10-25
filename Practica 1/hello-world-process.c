#include "contiki.h"
#include <stdio.h> /* For printf() */

/* 1. Declaración de ambos procesos */
PROCESS(hello_world_process, "Hello world process");
PROCESS(periodic_process, "Periodic process");

/* 2. Ambos procesos se cargan en el arranque */
AUTOSTART_PROCESSES(&hello_world_process, &periodic_process);

/*---------------------------------------------------------------------------*/
/* PROCESO 1: hello_world_process */
PROCESS_THREAD(hello_world_process, ev, data)
{
  static int counter = 0; /* Variable estática para no perder el valor */

  PROCESS_BEGIN();

  while(1) {
    /* Queda a la espera de recibir un evento */
    PROCESS_WAIT_EVENT();

    /* Si el evento recibido es el que enviamos desde el otro proceso... */
    if(ev == PROCESS_EVENT_CONTINUE) {
      printf("Hello World! (number %d)\n", counter);
      
      /* Incrementamos el contador y lo reiniciamos si pasa de 20 */
      counter++;
      if(counter > 20) {
        counter = 0;
      }
    }
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
/* PROCESO 2: periodic_process */
PROCESS_THREAD(periodic_process, ev, data)
{
  static struct etimer timer;

  PROCESS_BEGIN();

  /* Configurar el temporizador para que expire cada 5 segundos */
  etimer_set(&timer, CLOCK_SECOND * 5);

  while(1) {
    /* Esperar a que el temporizador expire */
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    
    /* Reiniciar el temporizador para el próximo ciclo */
    etimer_reset(&timer);
    
    /* Enviar evento asíncrono hacia hello_world_process */
    process_post(&hello_world_process, PROCESS_EVENT_CONTINUE, NULL);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
