#include "webserv.h"

void null_terminate(void *buf, size_t len)
{
  ((char *)buf)[len] = '\0';
}

void end_client(client *c)
{
  report_exit(c);
  c->client.flush();
  c->client.stop();
  c->file.close();
  c->active = false;
}

int send_file(EthernetClient *client, char *path)
{
  File32 file = env.sd.open(path, FILE_READ);
  size_t readed;

  if (!file)
    return (0);
  while ((readed = file.readBytes(env.buff, BUFF_SIZE)) == BUFF_SIZE)
    client->write(env.buff, BUFF_SIZE);
  client->write(env.buff, readed);
  return (1);
}