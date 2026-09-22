#include "WebClient.hpp"
#include "WebServer.hpp"

WebClient::~WebClient()
{
	mFile.close();
	mClient.flush();
	mClient.stop();
}


size_t clientDiscardUntil(EthernetClient &client, char c)
{
	size_t count = 0;
	int r;

	while (true) {
		r = client.read();
		if (r == -1)
			break ;
		count ++;
		if (r == c)
			break ;
	}
	return (count);
}

bool getClientHeader(EthernetClient &client, uint8_t *buff, size_t size)
{
	size_t pathLen;

	if (size < 16)
		return (false);
	if (client.readBytesUntil(' ', buff, size) >= size)
		if (client.read() != ' ')
			return (false);
	buff[0] = stringToMethod(reinterpret_cast<char *>(buff));

	pathLen = client.readBytesUntil(' ', &buff[1], size - 2);
	if (pathLen >= size - 2)
		if (client.read() != ' ')
			return (false);
	buff[pathLen + 1] = '\0';
	return (true);
}
