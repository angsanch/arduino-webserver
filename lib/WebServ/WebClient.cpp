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
	size_t pathLen;

	if (size < 16)
		return (false);
	if (mClient.readBytesUntil(' ', buff, size) >= size)
		if (mClient.read() != ' ')
			return (false);
	buff[0] = stringToMethod(reinterpret_cast<char *>(buff));

	pathLen = mClient.readBytesUntil(' ', &buff[1], size - 2);
	if (pathLen >= size - 2)
		if (mClient.read() != ' ')
			return (false);
	buff[pathLen + 1] = '\0';
	return (true);
}
