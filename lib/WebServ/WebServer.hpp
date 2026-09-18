#pragma once

#include "WebClient.hpp"

#include <SdFat.h>
#include <Ethernet.h>

#include <new>

#include <avr/pgmspace.h>

#define FLASHSTRING(name, content) const char name[] PROGNAME = content;

using FlashString = __FlashStringHelper *;

class WebServerHandle {
public:
		virtual WebClient *getClientSpot() = 0;
		virtual void getFile(File32 &file, const char *path) = 0;
};

enum HTTPMethod {
	UNRECOGNISED
};

struct ServerEntry {
	void (*callBack)(WebServerHandle &, EthernetClient &);
	struct {
		bool entrypoint : 1;
		enum HTTPMethod method : 7;
	};
	const char path[] PROGMEM;
};

template<size_t clientCount, typename SD = SdFat, ServerEntry *... entries>
class WebServer : public WebServerHandle {
private:
	ServerEntry *mEntries[sizeof...(entries)] = {entries ...};

	SD &mSd;
	EthernetServer mServer;
	WebClient mClient[clientCount];
	const bool mValid;
	unsigned int mSendCycles = 32;

	void clientLanding(EthernetClient &client)
	{
		//place holder
		mEntries[0]->callBack(*this, client);
	}

public:
	WebServer()
	: mSd(*static_cast<SD *>(nullptr))
	, mServer(0)
	, mValid(false)
	{}
	WebServer(SD &sd, uint16_t port)
	: mSd(sd)
	, mServer(port)
	, mValid(true)
	{
		memset(mClient, 0, sizeof(mClient) / sizeof(mClient[0]));
	}

	template<typename... Args>
	bool init(Args&&... args) {
		if (mValid)
			return (false);
		new (this) WebServer(static_cast<Args&&>(args)...);
		return (true);
	}

	void begin() { mServer.begin(); }
	void setSendCycles(unsigned int sendCycles) { mSendCycles = sendCycles; }


	WebClient *getClientSpot() override
	{
		return (nullptr);
	}
	void getFile(File32 &file, const char *path) override
	{}

	size_t accept()
	{
		size_t count = 0;
		EthernetClient client;

		while (true) {
			client = mServer.accept();
			if (client) {
				count ++;
				clientLanding(client);
			} else
				break ;
		}
		return (count);
	}
};
