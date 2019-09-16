#include "channel_simulator.h"

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/socket.h>

void init_channel_simulator(channel_simulator *c,
    uint32_t samplerate, int n_samples)
{
  c->timestamp = 1024;         /* timestamps [0..1023] used as commands */
  c->samplerate = samplerate;
  c->n_samples = n_samples;
  c->channels = NULL;
  c->channels_count = 0;
  c->connections = NULL;
  c->connections_count = 0;
}

void cleanup_connections(channel_simulator *c)
{
  connection *con;
  int i;

  /* remove dead connections */
  i = 0;
  while (i < c->connections_count) {
    if (c->connections[i].socket != -1) { i++; continue; }
    con = &c->connections[i];
    c->channels[con->rx_channel_index].connection_count--;
    c->channels[con->tx_channel_index].connection_count--;
    free(con->tx_buffer);
    if (c->connections_count == 1) {
      free(c->connections);
      c->connections = NULL;
      c->connections_count = 0;
      break;
    }
    c->connections_count--;
    memmove(&c->connections[i], &c->connections[i+1],
            (c->connections_count-i) * sizeof(connection));
    c->connections = realloc(c->connections,
                             c->connections_count * sizeof(connection));
    if (c->connections == NULL) goto oom;
  }

  /* remove channels with no more connections */
  i = 0;
  while (i < c->channels_count) {
    if (c->channels[i].connection_count) { i++; continue; }
    free(c->channels[i].data);
    if (c->channels_count == 1) {
      free(c->channels);
      c->channels = NULL;
      c->channels_count = 0;
      break;
    }
    c->channels_count--;
    memmove(&c->channels[i], &c->channels[i+1],
            (c->channels_count-i) * sizeof(channel));
    c->channels = realloc(c->channels, c->channels_count * sizeof(channel));
    if (c->channels == NULL) goto oom;
  }

  return;

oom:
  printf("ERROR: cleanup_connections: out of memory\n");
  exit(1);
}

static int find_or_create_channel(channel_simulator *c, uint64_t freq)
{
  channel *chan;
  int i;
  for (i = 0; i < c->channels_count; i++)
    if ( c->channels[i].frequency == freq) return i;

  c->channels_count++;
  c->channels = realloc(c->channels, c->channels_count * sizeof(channel));
  if (c->channels == NULL) goto oom;

  chan = &c->channels[i];
  chan->frequency = freq;
  chan->data = calloc(1, c->n_samples * 4);
  if (chan->data == NULL) goto oom;
  chan->connection_count = 0;

  return i;

oom:
  printf("ERROR: find_or_create_channel: out of memory\n");
  exit(1);
}

void channel_simulator_add_connection(channel_simulator *c,
            int socket, uint64_t rx_frequency, uint64_t tx_frequency)
{
  connection *con;

  printf("INFO: new connection RX %"PRIu64" TX %"PRIu64"\n",
         rx_frequency, tx_frequency);

  c->connections_count++;
  c->connections = realloc(c->connections,
                           c->connections_count * sizeof(connection));
  if (c->connections == NULL) goto oom;
  con = &c->connections[c->connections_count-1];

  con->socket = socket;
  con->tx_buffer = calloc(1, c->n_samples * 4);
  if (con->tx_buffer == NULL) goto oom;
  con->rx_frequency = rx_frequency;
  con->tx_frequency = tx_frequency;
  con->rx_channel_index = find_or_create_channel(c, rx_frequency);
  con->tx_channel_index = find_or_create_channel(c, tx_frequency);

  c->channels[con->rx_channel_index].connection_count++;
  c->channels[con->tx_channel_index].connection_count++;

  return;

oom:
  printf("ERROR: channel_simulator_add_connection: out of memory\n");
  exit(1);
}

void connection_send_rx(connection *c, uint64_t timestamp,
    uint32_t *data, int n_samples)
{
  unsigned char b[8+4];

  if (c->socket == -1) return;

  pu64(b, timestamp);
  pu32(b+8, n_samples);
  if (fullwrite(c->socket, b, 8+4) != 8+4 ||
      fullwrite(c->socket, data, n_samples * 4) != n_samples * 4) {
    printf("ERROR: connection_send_rx failed, dropping\n");
    shutdown(c->socket, SHUT_RDWR);
    close(c->socket);
    c->socket = -1;
  }
}

void command_set_frequency(channel_simulator *cs, connection *con, int n)
{
  unsigned char b[8*2];
  uint64_t rx_frequency;
  uint64_t tx_frequency;

  if (n != 8*2) goto err;
  if (fullread(con->socket, b, 8*2) != 8*2) goto err;
  rx_frequency = gu64(b);
  tx_frequency = gu64(b+8);

  printf("INFO: setting new frequencies RX %"PRIu64" and TX %"PRIu64"\n",
         rx_frequency, tx_frequency);

  cs->channels[con->rx_channel_index].connection_count--;
  cs->channels[con->tx_channel_index].connection_count--;

  con->rx_frequency = rx_frequency;
  con->tx_frequency = tx_frequency;
  con->rx_channel_index = find_or_create_channel(cs, rx_frequency);
  con->tx_channel_index = find_or_create_channel(cs, tx_frequency);
  cs->channels[con->rx_channel_index].connection_count++;
  cs->channels[con->tx_channel_index].connection_count++;

  return;

err:
  printf("ERROR: command_set_frequency failed, dropping\n");
  shutdown(con->socket, SHUT_RDWR);
  close(con->socket);
  con->socket = -1;
}

void do_command(channel_simulator *cs, connection *c, uint64_t command, int n)
{
  switch (command) {
  case 0: return command_set_frequency(cs, c, n);
  default:
    printf("ERROR: bad command %"PRIu64", dropping\n", command);
    shutdown(c->socket, SHUT_RDWR);
    close(c->socket);
    c->socket = -1;
  }
}

void connection_receive_tx(channel_simulator *cs,
    connection *c, uint64_t timestamp, int n_samples)
{
  unsigned char b[8+4];
  uint64_t recv_timestamp;
  uint32_t recv_n_samples;

again:
  if (c->socket == -1) return;

  if (fullread(c->socket, b, 8+4) != 8+4) goto err;
  recv_timestamp = gu64(b);
  recv_n_samples = gu32(b+8);

  if (recv_timestamp < 1024) {
    do_command(cs, c, recv_timestamp, recv_n_samples);
    goto again;
  }

  if (timestamp != recv_timestamp) {
    printf("ERROR: bad timestamp, got %"PRIu64" expected %"PRIu64"\n",
           recv_timestamp, timestamp);
    goto err;
  }
  if (n_samples != recv_n_samples) {
    printf("ERROR, bad n_samples, got %d expected %d\n",
           recv_n_samples, n_samples);
    goto err;
  }
  if (fullread(c->socket, c->tx_buffer, n_samples * 4) != n_samples * 4)
    goto err;

  return;

err:
  printf("ERROR: connection_receive_tx failed, dropping\n");
  shutdown(c->socket, SHUT_RDWR);
  close(c->socket);
  c->socket = -1;
}

void channel_simulate(channel_simulator *c)
{
  int i;
  int k;
  connection *con;
  channel *chan;
  int16_t *to;
  int16_t *from;
  int32_t mix[c->channels_count][c->n_samples*2];

  memset(mix, 0, c->channels_count * c->n_samples*2 * 4);

  /* clear channels */
  for (i = 0; i < c->channels_count; i++)
    memset(c->channels[i].data, 0, c->n_samples * 4);

  /* basic mixing */
  for (i = 0; i < c->connections_count; i++) {
    con = &c->connections[i];
    from = (int16_t *)con->tx_buffer;

    for (k = 0; k < c->n_samples * 2; k++)
      mix[con->tx_channel_index][k] += from[k];
  }

  for (i = 0; i < c->channels_count; i++) {
    chan = &c->channels[i];
    to = (int16_t *)chan->data;

    for (k = 0; k < c->n_samples * 2; k++) {
      int v = mix[i][k];
      if (v > 32767) v = 32767;
      if (v < -32768) v = -32768;
      to[k] = v;
    }
  }
}
