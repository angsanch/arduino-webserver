#pragma once

#include <Ethernet.h>
#include <SdFat.h>

class WebClient {
private:
	EthernetClient mClient;
	File32 mFile;

public:
	WebClient() = default;
	~WebClient();
};

size_t clientDiscardUntil(EthernetClient &client, char c);
bool getClientHeader(EthernetClient &client, uint8_t *buff, size_t size);
