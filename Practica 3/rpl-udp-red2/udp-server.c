
#include "contiki.h"
#include "net/routing/routing.h"
#include "net/netstack.h"
#include "net/ipv6/simple-udp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define UDP_CLIENT_PORT	8765
#define UDP_SERVER_PORT	5678

static struct simple_udp_connection udp_conn;

PROCESS(udp_server_process, "UDP server");
AUTOSTART_PROCESSES(&udp_server_process);
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
  char buf[16];
  char reply[16];
  int temp_c, temp_f;

  /* Los datos recibidos NO terminan en '\0': copiarlos a un buffer */
  if(datalen >= sizeof(buf)) {
    datalen = sizeof(buf) - 1;
  }
  memcpy(buf, data, datalen);
  buf[datalen] = '\0';

  temp_c = atoi(buf);
  temp_f = temp_c * 9 / 5 + 32;   /* aritmética entera: sin %f en MSP430 */

  LOG_INFO("Recibida temperatura %d C desde ", temp_c);
  LOG_INFO_6ADDR(sender_addr);
  LOG_INFO_("\n");

  snprintf(reply, sizeof(reply), "%d", temp_f);
  LOG_INFO("Respondiendo con %d F\n", temp_f);
  simple_udp_sendto(&udp_conn, reply, strlen(reply), sender_addr);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_server_process, ev, data)
{
  PROCESS_BEGIN();

  /* El servidor es la raíz de la red RPL */
  NETSTACK_ROUTING.root_start();

  /* Escucha en el puerto del servidor; los clientes usan UDP_CLIENT_PORT */
  simple_udp_register(&udp_conn, UDP_SERVER_PORT, NULL,
                      UDP_CLIENT_PORT, udp_rx_callback);

  PROCESS_END();
}
