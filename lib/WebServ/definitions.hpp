#pragma once

#include "Dict.hpp"

extern Dict content_type;
extern Dict http_code;

extern const char http_response_header[] PROGMEM;

extern const char http_response_static_400[] PROGMEM;
extern const char http_response_static_404[] PROGMEM;
extern const char http_response_static_405[] PROGMEM;
extern const char http_response_static_429[] PROGMEM;
extern const char http_response_static_500[] PROGMEM;
