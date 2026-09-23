#include "webserv.h"

#include <WebServer.hpp>


ServerEntry sdcardReader = {WEB_PATH("/"), {true, HTTP_POST | HTTP_HEAD}, [](WebServerHandle &, WebClient &, t_http_method){Serial.println("client"); Serial.println(&GlobalBuffer().raw()[0]);}};

using WebServ = WebServer<CLIENTS, SdFat,
	&sdcardReader
>;

SdFat sd;
WebServ server;
Buffer<BUFF_SIZE> buff;

void setup(){
	buff.init();
	File32 file;

	//Serial and SD initialization
	Serial.begin(9600);
	Serial.println(F("SD"));
	while (!sd.begin(4, SPI_FULL_SPEED)){
		Serial.println(F("KO"));
	}
	Serial.println(F("OK"));


	//Ethernet initialization
	//MAC setup
	byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
	// IP
	file = sd.open(IP_FILE, FILE_READ);
	if (!file) {
		Ethernet.begin(mac);
	} else {
		int ip_list[4];
		for (int i = 0; i < 4; i ++){
			null_terminate(buff, file.readBytesUntil('.', buff, 4));
			ip_list[i] = atoi(buff);
		}
		file.close();
		IPAddress ip (ip_list[0], ip_list[1], ip_list[2], ip_list[3]);
		Ethernet.begin(mac, ip);
	}
	//Get port
	file = sd.open("port", FILE_READ);
	null_terminate(buff, file.readBytes(buff, BUFF_SIZE));
	unsigned int port = atol(buff);
	//Server setup
	server.init(sd, port);
	server.begin();


	//Print info to the serial
	Serial.print (F("Server at http://"));
	Serial.print (Ethernet.localIP ());
	Serial.print (':');
	Serial.println (port);

	//Open static files
	file = sd.open(CYCLES_FILE, FILE_READ);
	server.setSendCycles(file.parseInt());
	file.close();
	// env.header = env.sd.open("header", FILE_READ);

}

void loop(){
/*	static bool was_idle = false;

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
*/
	delay(1000);
	server.accept();
}
