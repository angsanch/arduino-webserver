#pragma once

#include <Ethernet.h>
#include <SdFat.h>

#include "definitions.hpp"

class WebEthernetClient {
private:
	EthernetClient mClient;


public:
	WebEthernetClient() = default;
	WebEthernetClient(EthernetClient &&client)
	: mClient(static_cast<EthernetClient &&>(client))
	{
		mClient.setTimeout(4096);
	}
	WebEthernetClient(const WebEthernetClient &) = delete;
	WebEthernetClient &operator=(const WebEthernetClient &) = delete;
	WebEthernetClient(WebEthernetClient &&) = default;
	WebEthernetClient &operator=(WebEthernetClient &&) = default;
	~WebEthernetClient();

	size_t discardUntil(char c);
	bool getHeader(uint8_t *buff, size_t size);

	inline void printFlash(const char *flash) { return (writeFlash(flash, strlen_P(flash))); }
	void writeFlash(const char *flash, size_t len);

	bool sendHeader(int code);

	bool valid() { return (mClient); }
};

class WebClient : public WebEthernetClient {
private:
	File32 mFile;

public:
	WebClient() = default;
	WebClient(EthernetClient &&client)
	: WebEthernetClient(static_cast<EthernetClient &&>(client))
	{}
	WebClient(const WebClient &) = delete;
	WebClient &operator=(const WebClient &) = delete;
	WebClient(WebClient &&) = default;
	WebClient &operator=(WebClient &&) = default;
	~WebClient();

	inline WebEthernetClient &client() { return (*this); }
};
