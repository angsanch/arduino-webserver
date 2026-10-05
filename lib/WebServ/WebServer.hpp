#pragma once

#include "WebClient.hpp"
#include "GlobalBuffer.hpp"
#include "Circular.hpp"

#include <SdFat.h>
#include <Ethernet.h>

#include <new>

#include <avr/pgmspace.h>

#define FLASHSTRING(name, content) const char name[] PROGMEM = content;

#define GLOBAL_FSTRING(x) ([]() -> PGM_P { \
    static FLASHSTRING(str, x); \
    return (reinterpret_cast<PGM_P>(str)); \
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

inline t_http_method operator &(t_http_method a, t_http_method b) { return (static_cast<t_http_method>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b))); }
inline t_http_method operator |(t_http_method a, t_http_method b) { return (static_cast<t_http_method>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b))); }

t_http_method stringToMethod(const char *str);
size_t methodToString(t_http_method method, char *str, size_t size);

void staticResponse(WebClient &client, int code);

struct ServerEntry {
	PGM_P path;
	struct {
		bool entrypoint : 1;
		t_http_method method : 7;
	};
	void (*callBack)(WebServerHandle &, WebClient &, t_http_method);
};

template<size_t clientCount, typename SD = SdFat, ServerEntry *... entries>
class WebServer : public WebServerHandle {
private:
	ServerEntry *mEntries[sizeof...(entries)] = {entries ...};

	SD &mSd;
	EthernetServer mServer;
	Circular<WebClient, clientCount> mClient;
	const bool mValid;
	unsigned int mSendCycles = 32;

	void clientLanding(WebClient &client)
	{
		GlobalBuffer buff;

		if (!client.client().getHeader(reinterpret_cast<uint8_t *>(buff.raw()), buff.size()))
			return (staticResponse(client, 400));

		char *path = &buff.raw()[1];
		for (const auto &i : mEntries) {
			if (i->entrypoint) {
				size_t len_entry = strlen_P(i->path);
				size_t len_client = strlen(path);
				if (pgm_read_byte(&i->path[len_entry - 1]) == '/')
					len_entry --;
				if (strncmp_P(path, i->path, len_entry) != 0)
					continue ;
				char next = path[len_entry];
				if (next != '/' && next != '\0')
					continue ;
				memmove(path, &path[len_entry], len_client - len_entry + 1);
			} else {
				if (strcmp_P(path, i->path) != 0)
					continue ;
			}
			if (!(i->method & buff.raw()[0]))
				return (staticResponse(client, 405));
			return (i->callBack(*this, client, static_cast<t_http_method>(buff.raw()[0])));
		}
		return (staticResponse(client, 404));
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
	{}

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
		if (!mClient.emplace_back())
			return (nullptr);
		return (&mClient[mClient.size() - 1]);
	}
	void getFile(File32 &file, const char *path) override
	{
		file = mSd.open(path, FILE_READ);
	}

	size_t accept()
	{
		size_t count = 0;

		while (true) {
			WebClient client(mServer.accept());
			if (client.client().valid()) {
				count ++;
				clientLanding(client);
			} else
				break ;
		}
		return (count);
	}

	size_t serve(size_t bytes)
	{
		size_t served = 0;

		while (served < bytes) {
			WebClient *client;

			if (mClient.size() == 0)
				break;
			client = &mClient[0];
			while (served < bytes) {
				size_t chunk = client->serve();
				if (chunk == 0) {
					mClient.pop_front();
					break;
				}
				served += chunk;
			}
		}
		return (served);
	}
};
