#include "contiki.h"
#include "dev/leds.h"
#include "sys/etimer.h"

/* Declaracion de los tres procesos */
PROCESS(parpadeo_1_process, "Parpadeo LED 1");
PROCESS(parpadeo_2_process, "Parpadeo LED 2");
PROCESS(timer_process, "Temporizador");

/* Inicio automatico de los procesos */
AUTOSTART_PROCESSES(
    &parpadeo_1_process,
    &parpadeo_2_process,
    &timer_process
);

/* Proceso que controla el LED 1 */
PROCESS_THREAD(parpadeo_1_process, ev, data)
{
    static struct etimer timer;

    PROCESS_BEGIN();

    /* Esperar al evento inicial del timer_process */
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_POLL);

    while(1) {
        etimer_set(&timer, 2 * CLOCK_SECOND);

        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));

        /* Conmutar LED 1 */
        leds_toggle(1 << 0);
    }

    PROCESS_END();
}

/* Proceso que controla el LED 2 */
PROCESS_THREAD(parpadeo_2_process, ev, data)
{
    static struct etimer timer;

    PROCESS_BEGIN();

    /* Esperar al evento inicial del timer_process */
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_POLL);

    while(1) {
        etimer_set(&timer, 3 * CLOCK_SECOND);

        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));

        /* Conmutar LED 2 */
        leds_toggle(1 << 1);
    }

    PROCESS_END();
}

/* Proceso que genera los eventos iniciales */
PROCESS_THREAD(timer_process, ev, data)
{
    static struct etimer timer;

    PROCESS_BEGIN();

    /* Esperar 5 segundos */
    etimer_set(&timer, 5 * CLOCK_SECOND);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));

    /* Despertar los dos procesos de parpadeo */
    process_poll(&parpadeo_1_process);
    process_poll(&parpadeo_2_process);

    PROCESS_END();
}
