#include "WebClient.hpp"

WebClient::~WebClient()
{
	mFile.close();
	mClient.flush();
	mClient.stop();
}
