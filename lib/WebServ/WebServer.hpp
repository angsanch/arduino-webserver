#pragma once

#include "WebClient.hpp"

#include <SdFat.h>
#include <Ethernet.h>

#include <new>

template<size_t clientCount, typename SD = SdFat>
class WebServer {
private:
	SD &mSd;
	EthernetServer mServer;
	WebClient mClient[clientCount];
	const bool mValid;
	unsigned int mSendCycles = 32;

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

	size_t accept()
	{
		// accept incoming clients;
		return (0);
	}
};
