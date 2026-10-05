#include "WebClient.hpp"
#include "WebServer.hpp"

WebEthernetClient::~WebEthernetClient()
{
	mClient.flush();
	mClient.stop();
}

WebClient::~WebClient()
{
	mFile.close();
}


size_t WebEthernetClient::discardUntil(char c)
{
	size_t count = 0;
	int r;

	while (true) {
		r = mClient.read();
		if (r == -1)
			break ;
		count ++;
		if (r == c)
			break ;
	}
	return (count);
}

bool WebEthernetClient::getHeader(uint8_t *buff, size_t size)
{
	size_t methodLen;
	size_t pathLen;

	if (size < 16)
		return (false);
	methodLen = mClient.readBytesUntil(' ', buff, size - 1);
	buff[methodLen] = '\0';
	buff[0] = stringToMethod(reinterpret_cast<char *>(buff));

	pathLen = mClient.readBytesUntil(' ', &buff[1], size - 2);
	buff[pathLen + 1] = '\0';
	return (true);
}

void WebEthernetClient::writeFlash(const char *flash, size_t len)
{
	GlobalBuffer buff;
	size_t offset = 0;

	while (len - offset > 0) {
		size_t block = min(len - offset, buff.size());

		memcpy_P(buff.raw(), &flash[offset], block);
		mClient.write(buff.raw(), block);
		offset += block;
	}
}

static size_t sendUntil(WebEthernetClient &client, char const *flash, int chr)
{
	char const *end = strchr_P(flash, chr);
	size_t len = (end) ? end - flash : strlen_P(flash);

	client.writeFlash(flash, len);
	return (len);
}

WebEthernetClient &WebEthernetClient::sendHeader(int code)
{
	GlobalBuffer buff;
	size_t offset;

	offset = sendUntil(*this, http_response_header, ';');
	offset ++;
	snprintf(buff.raw(), buff.size(), "%d", code);
	mClient.print(buff.raw());
	mClient.write(' ');
	printFlash(http_code.get(buff.raw()));
	printFlash(&http_response_header[offset]);
	return (*this);
}

WebEthernetClient &WebEthernetClient::sendHeaderFieldFlash(const char *name, char const *value)
{
	printFlash(name);
	printFlash(reinterpret_cast<const char *>(F(": ")));
	printFlash(value);
	mClient.write('\n');
	return (*this);
}

size_t WebClient::serve()
{
	GlobalBuffer buff;
	size_t read = mFile.read(buff.raw(), buff.size());

	mClient.write(buff.raw(), read);
	return (read);
}
