#include "contiki.h"
#include "dev/leds.h"
#include <stdio.h>

/* Evento personalizado para sincronizar el arranque del parpadeo */
process_event_t start_blink_event;

/* Declaración de las 3 tareas/procesos */
PROCESS(tarea_1_process, "Tarea 1 - LED Verde");
PROCESS(tarea_2_process, "Tarea 2 - LED Amarillo");
PROCESS(tarea_3_process, "Tarea 3 - Temporizador Inicial");

/* Arrancamos los tres procesos al inicio */
AUTOSTART_PROCESSES(&tarea_1_process, &tarea_2_process, &tarea_3_process);

/*---------------------------------------------------------------------------*/
/* TAREA 1: Parpadeo del LED verde cada 2 segundos */
PROCESS_THREAD(tarea_1_process, ev, data)
{
  static struct etimer timer_led1;

  PROCESS_BEGIN();

  /* Esperar a recibir el evento de inicio desde la Tarea 3 */
  PROCESS_WAIT_EVENT_UNTIL(ev == start_blink_event);

  /* Configurar el temporizador periódico a 2 segundos */
  etimer_set(&timer_led1, CLOCK_SECOND * 2);

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_led1));
    etimer_reset(&timer_led1);
    
    /* Conmutar el estado del LED verde */
    leds_toggle(LEDS_GREEN);
    printf("Tarea 1: Toggle LED Verde\n");
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
/* TAREA 2: Parpadeo del LED amarillo cada 4 segundos */
PROCESS_THREAD(tarea_2_process, ev, data)
{
  static struct etimer timer_led2;

  PROCESS_BEGIN();

  /* Esperar a recibir el evento de inicio desde la Tarea 3 */
  PROCESS_WAIT_EVENT_UNTIL(ev == start_blink_event);

  /* Configurar el temporizador periódico a 4 segundos */
  etimer_set(&timer_led2, CLOCK_SECOND * 4);

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_led2));
    etimer_reset(&timer_led2);
    
    /* Conmutar el estado del LED amarillo (o rojo según la placa Z1) */
    leds_toggle(LEDS_YELLOW);
    printf("Tarea 2: Toggle LED Amarillo\n");
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
/* TAREA 3: Espera 3 segundos y lanza el evento a las Tareas 1 y 2 */
PROCESS_THREAD(tarea_3_process, ev, data)
{
  static struct etimer timer_init;

  PROCESS_BEGIN();

  /* Asignar un ID único al evento en el arranque */
  start_blink_event = process_alloc_event();

  /* Configurar espera inicial de 3 segundos */
  etimer_set(&timer_init, CLOCK_SECOND * 3);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer_init));

  printf("Tarea 3: 3 segundos cumplidos. Lanzando eventos...\n");

  /* Enviar el evento de forma asíncrona a ambas tareas */
  process_post(&tarea_1_process, start_blink_event, NULL);
  process_post(&tarea_2_process, start_blink_event, NULL);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
