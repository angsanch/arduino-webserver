#include "webserv.h"

void report_entry(client *c)
{
  //c->file.getName(env.buff, BUFF_SIZE);

  Serial.print(c->id);
  Serial.print('\t');
  Serial.print(c->client.remoteIP());
  Serial.print(':');
  Serial.print(c->client.remotePort());
  Serial.print('\t');
  Serial.print(c->method);
  Serial.print('\t');
  Serial.print(c->code);
  Serial.print('\t');
  Serial.print(env.buff);
  Serial.println();
}

void report_serve(client *c)
{
  Serial.print(c->id);
  Serial.print('\t');
  Serial.print("Serving");
  Serial.println();
}

void report_exit(client *c)
{
  Serial.print(c->id);
  Serial.print('\t');
  Serial.print("Exiting");
  Serial.println();
}

void report_post(client *c)
{
  bool open = false;
  size_t loaded;

  while (c->client.available()) {
    loaded = c->client.readBytesUntil('\n', env.buff, BUFF_SIZE);
    if (loaded < BUFF_SIZE) {
      if (open && loaded <= 1)
        break;
      open = true;
    } else
      open = false;
  }
  open = false;
  if (c->client.available()){
    Serial.print(c->id);
    Serial.print('\t');
    Serial.print("Data");
    Serial.println();
  }
  while (c->client.available()) {
    loaded = c->client.readBytesUntil('&', env.buff, BUFF_SIZE);
    if (!open) {
      Serial.print(c->id);
      Serial.print('\t');
      open = true;
    }
    Serial.write(env.buff, loaded);
    if (loaded < BUFF_SIZE)
      Serial.println();
    open = false;
  }
}
