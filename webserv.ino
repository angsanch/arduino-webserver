#include "webserv.h"

enviroment env;
EthernetServer server(0);

void setup(){
  File32 file;

  //Serial and SD initialization
  Serial.begin(9600);
  Serial.println("SD");
  while (!env.sd.begin(4, SPI_FULL_SPEED)){
    Serial.println("KO");
  }
  Serial.println("OK");


  //Ethernet initialization
  // IP
  file = env.sd.open("ip", FILE_READ);
  int ip_list[4];
  for (int i = 0; i < 4; i ++){
    null_terminate(env.buff, file.readBytesUntil('.', env.buff, 4));
    ip_list[i] = atoi(env.buff);
  }
  file.close();
  IPAddress ip (ip_list[0], ip_list[1], ip_list[2], ip_list[3]);
  //Get port
  file = env.sd.open("port", FILE_READ);
  null_terminate(env.buff, file.readBytes(env.buff, BUFF_SIZE));
  unsigned int port = atol(env.buff);
  //MAC setup
  byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
  //Server setup
  Ethernet.begin(mac, ip);
  server = EthernetServer(port);
  server.begin ();


  //Print info to the serial
  Serial.print ("Server at http://");
  Serial.print (Ethernet.localIP ());
  Serial.print (':');
  Serial.println (port);

  //Open static files
  file = env.sd.open(CYCLES_FILE, FILE_READ);
  env.send_cycles = file.parseInt();
  file.close();
  env.header = env.sd.open("header", FILE_READ);

  //Empty client memory
  memset(&env.clients, 0, sizeof(client) * CLIENTS);
}

void loop(){
  static bool was_idle = false;

  while (true) {
    env.lobby.client = server.accept();
    if (env.lobby.client) {
      if (recieve_client(&env.lobby))
        handshake(&env.lobby);
    } else
      break;
  }
  send_payload();
  if (active_clients() == 0) {
    if (!was_idle)
      Serial.println("Idle");
    was_idle = true;
    delay(1000);
  } else
    was_idle = false;
}