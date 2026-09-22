#pragma once

#include "WebClient.hpp"
#include "GlobalBuffer.hpp"

#include <SdFat.h>
#include <Ethernet.h>

#include <new>

#include <avr/pgmspace.h>

#define FLASHSTRING(name, content) const char name[] PROGMEM = content;

#define WEB_PATH(x) ([]() -> const char * { \
    static FLASHSTRING(str, x); \
    return (str); \
}())

using FlashString = __FlashStringHelper *;

class WebServerHandle {
public:
		virtual WebClient *getClientSpot() = 0;
		virtual void getFile(File32 &file, const char *path) = 0;
};

typedef enum HTTPMethod {
	HTTP_UNRECOGNISED = 0,
	HTTP_GET          = (1u << 0),
	HTTP_HEAD         = (1u << 1),
	HTTP_OPTIONS      = (1u << 2),
	HTTP_TRACE        = (1u << 3),
	HTTP_PUT          = (1u << 4),
	HTTP_DELETE       = (1u << 5),
	HTTP_POST         = (1u << 6),
} t_http_method;
extern const char *const HTTPMethodName[] PROGMEM;

inline t_http_method operator &(t_http_method a, t_http_method b) { return (a & b); }
inline t_http_method operator |(t_http_method a, t_http_method b) { return (a | b); }

t_http_method stringToMethod(const char *str);
size_t methodToString(t_http_method method, char *str, size_t size);

struct ServerEntry {
	const char *path;
	struct {
		bool entrypoint : 1;
		enum HTTPMethod method : 7;
	};
	void (*callBack)(WebServerHandle &, EthernetClient &);
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
		GlobalBuffer buff;

		Serial.println(F("client arrived"));
		if (!getClientHeader(client, reinterpret_cast<uint8_t *>(buff.raw()), buff.size()))
			/*manage bad header, response 400*/return ;

		Serial.print((int)buff.raw()[0]);
		Serial.println((char *)&buff.raw()[1]);
		for (size_t n = 0; n < buff.size(); n++) {
			Serial.print((int)(buff.raw()[n]));
			Serial.print(' ');
		}
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
