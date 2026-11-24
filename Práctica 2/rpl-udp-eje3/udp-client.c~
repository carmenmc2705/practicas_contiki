#include "contiki.h"
#include "net/routing/routing.h"
#include "random.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678

#define SEND_INTERVAL		  (60 * CLOCK_SECOND)

static struct simple_udp_connection udp_conn;

/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client");
AUTOSTART_PROCESSES(&udp_client_process);
/*---------------------------------------------------------------------------*/
static void
udp_rx_callback(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  /* Al recibir la respuesta en Fahrenheit desde el servidor */
  LOG_INFO("Temperatura en Fahrenheit calculada por el servidor '%.*s' grados.\n",
           datalen, (char *) data);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data)
{
  static struct etimer periodic_timer;
  static unsigned count = 0;
  static char str[32];
  uip_ipaddr_t dest_ipaddr;
  int temp_c;

  PROCESS_BEGIN();

  /* Inicializar conexión UDP */
  simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL,
                      UDP_SERVER_PORT, udp_rx_callback);

  etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

    if(NETSTACK_ROUTING.node_is_reachable() &&
       NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {

      count++;

      /* Lógica impar (interna = 42) / par (externa = 27) */
      if(count % 2 != 0) {
        temp_c = 42;
        LOG_INFO("Enviando temperatura interna en Celsius = %d\n", temp_c);
      } else {
        temp_c = 27;
        LOG_INFO("Enviando temperatura externa en Celsius = %d\n", temp_c);
      }

      /* Formatear y enviar el valor numérico al servidor */
      snprintf(str, sizeof(str), "%d", temp_c);
      simple_udp_sendto(&udp_conn, str, strlen(str), &dest_ipaddr);

    } else {
      LOG_INFO("Not reachable yet\n");
    }

    /* Siguiente envío con jitter */
    etimer_set(&periodic_timer, SEND_INTERVAL
      - CLOCK_SECOND + (random_rand() % (2 * CLOCK_SECOND)));
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
