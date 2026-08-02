#include "webserv.h"

static int set_method(client *c)
{
  char buff[5];

  if (!c->client.readBytesUntil(' ', buff, 5))
    return (0);
  switch (buff[0]) {
    case 'G':
      c->method = GET;
      break;
    case 'H':
      c->method = HEAD;
      break;
    case 'P':
      c->method = (buff[1] == 'O') ? POST : NOT;
      break;
    default:
      c->method = NOT;
  }
  return (1);
}

static int get_path(client *c)
{
  size_t len = strlen(BASE_PATH);
  size_t readed;

  strcpy(env.buff, BASE_PATH);
  readed = c->client.readBytesUntil(' ', env.buff + len, BUFF_SIZE - len);
  if (!readed)
    return (0);
  len += readed;
  env.buff[len] = '\0';
  if (strchr(env.buff, '.') == NULL && env.buff[len - 1] != '/') {
    strcat(env.buff, "/");
    len ++;
  }
  if (env.buff[len - 1] == '/')
    strcat(env.buff, DEFAULT_FILE);
  return (1);
}

unsigned int active_clients(void)
{
  unsigned int i = 0;

  while (i < CLIENTS && env.clients[i].active)
    i ++;
  return (i);
}

static void prepare_file(client *c)
{
  char *format;

  c->file = env.sd.open(env.buff);
  format = strrchr(env.buff, '.');
  if (format == NULL)
    format = env.buff;
  else
    format ++;
  strlcpy(c->format, format, FORMAT_SIZE);
}

static int get_status(client *c)
{
  if (active_clients() >= CLIENTS) {
    c->code = 429;
    return (1);
  }
  if (!set_method(c))
    return (0);
  if (c->method == NOT) {
    c->code = 405;
    return (1);
  }
  c->code = 200;
  return (1);
}

static int set_path(client *c)
{
  if (!get_path(c))
    return (0);
  if (c->code != 429)
    if (!env.sd.exists(env.buff))
      c->code = 404;
  if (c->code != 200)
    itoa(c->code, env.buff + 1, 10);
  c->external_header = c->code != 429;
  return (1);
}

int recieve_client(client *c)
{
  static unsigned int id = 0;

  if (!get_status(c))
    return (0);
  if (!set_path(c))
    return (0);
  prepare_file(c);
  if (id == 0)
    id ++;
  c->id = id;
  id ++;
  report_entry(c);
  if (c->method == POST)
    report_post(c);
  return (1);
}