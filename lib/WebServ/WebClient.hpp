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
