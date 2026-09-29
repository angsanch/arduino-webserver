#pragma once

#include <stdlib.h>
#include <avr/pgmspace.h>
#include <Arduino.h>

class Dict {
private:
	size_t mKeySize;
	size_t mValueSize;
	size_t mLen;
	PGM_P mData;

public:
	Dict(size_t keySize, size_t valueSize, size_t len, PGM_P data)
	: mKeySize(keySize)
	, mValueSize(valueSize)
	, mLen(len)
	, mData(data)
	{}
	~Dict() = default;

	size_t size() const { return (mLen); }

	PGM_P getFlash(const __FlashStringHelper *key);
	PGM_P get(const char *key);
};
