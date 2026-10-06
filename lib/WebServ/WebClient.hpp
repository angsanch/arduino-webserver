#pragma once

#include <Ethernet.h>
#include <SdFat.h>

#include "definitions.hpp"

class HeaderHandler;

class WebEthernetClient {
protected:
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

	template<typename T>
	void print(const T &value)
	{ mClient.print(value); }

	HeaderHandler header();

	bool valid() { return (mClient); }
};

class HeaderHandler {
private:
	WebEthernetClient &mClient;

public:
	HeaderHandler(WebEthernetClient &client)
	: mClient(client)
	{}
	~HeaderHandler() { mClient.print('\n'); }

	HeaderHandler &send(int code);

	template<typename T>
	HeaderHandler &sendField(const char *name, const T &value)
	{
		mClient.printFlash(name);
		mClient.printFlash(reinterpret_cast<const char *>(F(": ")));
		mClient.print(value);
		mClient.print('\n');
		return (*this);
	}
	HeaderHandler &sendFieldFlash(const char *name, char const *value);
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

	size_t serve();
};
