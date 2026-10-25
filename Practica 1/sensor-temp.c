#include "contiki.h"
#include <stdio.h>
/* Cabecera genérica para el manejo de sensores */
#include "lib/sensors.h" 

/* Declaración del sensor de temperatura externo para la plataforma nRF52840 */
extern struct sensors_sensor temperature_sensor;

/* Declaración de los procesos */
PROCESS(timer_process, "Proceso Temporizador");
PROCESS(temperature_process, "Proceso Temperatura");

/* Iniciar ambos procesos automáticamente */
AUTOSTART_PROCESSES(&timer_process, &temperature_process);

/*---------------------------------------------------------------------------*/
/* PROCESO 1: Temporizador de 3 segundos */
PROCESS_THREAD(timer_process, ev, data)
{
  static struct etimer timer;

  PROCESS_BEGIN();

  /* Configurar el temporizador periódico a 3 segundos */
  etimer_set(&timer, CLOCK_SECOND * 3);

  while(1) {
    /* Esperar a que expire el temporizador */
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    
    /* Reiniciar el temporizador para el siguiente ciclo */
    etimer_reset(&timer);
    
    /* Enviar un evento asíncrono al proceso de temperatura para que despierte */
    process_post(&temperature_process, PROCESS_EVENT_CONTINUE, NULL);
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
/* PROCESO 2: Lectura de la temperatura */
PROCESS_THREAD(temperature_process, ev, data)
{
  PROCESS_BEGIN();

  /* Activar el sensor antes de poder leerlo */
  SENSORS_ACTIVATE(temperature_sensor);

  while(1) {
    /* Quedar a la espera de cualquier evento */
    PROCESS_WAIT_EVENT();

    /* Comprobar si el evento recibido es el enviado por timer_process */
    if(ev == PROCESS_EVENT_CONTINUE) {
      /* Leer el valor del sensor. Se le pasa un 0 por defecto */
      int temp_val = temperature_sensor.value(0);
      int temp_ent = temp_val/4;
      int temp_dec = (temp_ent%4)*25;
      
      /* Imprimir la lectura en grados centígrados */
      printf("Temperatura: %d.%02d ºC\n", temp_ent, temp_dec);
    }
  }

  /* Desactivar el sensor al terminar (aunque en este bucle infinito no se llegue) */
  SENSORS_DEACTIVATE(temperature_sensor);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
