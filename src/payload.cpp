#include "webserv.h"

static bool send_chunk(client *c)
{
  size_t weight;
  size_t transfered;

  if (!c->active)
    return false;
  weight = c->file.readBytes(env.buff, BUFF_SIZE);
  transfered = c->client.write(env.buff, weight);
  return (weight == transfered && weight == BUFF_SIZE);
}

void send_payload(void)
{
  if (!env.clients[0].active)
    return;
  if (!env.clients[0].used) {
    env.clients[0].used = true;
    report_serve(&env.clients[0]);
  }
  for (unsigned int i = 0; true; i ++){
      if (i == env.send_cycles)
        return;
      if (!env.clients[0].client.availableForWrite())
        break;
      if (!send_chunk(&env.clients[0]))
        break;
  }
  end_client(&env.clients[0]);
  memmove(&env.clients[0], &env.clients[1], sizeof(client[CLIENTS - 1]));
  memset(&env.clients[CLIENTS - 1], 0, sizeof(client));
}