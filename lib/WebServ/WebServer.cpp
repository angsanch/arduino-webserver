#include "WebServer.hpp"

FLASHSTRING (EMPTY, "");
FLASHSTRING (GET, "GET");
FLASHSTRING (HEAD, "HEAD");
FLASHSTRING (OPTIONS, "OPTIONS");
FLASHSTRING (TRACE, "TRACE");
FLASHSTRING (PUT, "PUT");
FLASHSTRING (DELETE, "DELETE");
FLASHSTRING (POST, "POST");

const char *const HTTPMethodName[] PROGMEM = {
	EMPTY,
	GET,
	HEAD,
	OPTIONS,
	TRACE,
	PUT,
	DELETE,
	POST,
};

t_http_method stringToMethod(const char *str)
{
	const size_t count = sizeof(HTTPMethodName) / sizeof(HTTPMethodName[0]);

	for (size_t i = 1; i < count; i++) {
		PGM_P fstr = static_cast<PGM_P>(pgm_read_ptr(&HTTPMethodName[i]));

		if (strcmp_P(str, fstr) == 0)
			return (static_cast<t_http_method>(1u << (i - 1)));
	}
	return (HTTP_UNRECOGNISED);
}

size_t methodToString(t_http_method method, char *str, size_t size);

void staticResponse(WebClient &client, int code)
{
	const char *body = nullptr;

	switch (code) {
		case 400:
			body = http_response_static_400;
			break ;
		case 404:
			body = http_response_static_404;
			break ;
		case 405:
			body = http_response_static_405;
			break ;
		case 429:
			body = http_response_static_429;
			break ;
		case 500:
			body = http_response_static_500;
			break ;
		default:
			break ;
	}

	{
		HeaderHandler header = client.header();

		header.send(code);
		if (code == 429)
			header.sendFieldFlash(GLOBAL_FSTRING("Retry-After"), GLOBAL_FSTRING("30"));
	}
	if (body)
		client.printFlash(body);
}
