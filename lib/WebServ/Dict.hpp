#pragma once

#include <stdlib.h>
#include <avr/pgmspace.h>
#include <string.h>

class Dict {
private:
	size_t mKeySize;
	size_t mValueSize;
	size_t mLen;
	PGM_P mData;

	PGM_P search(const char *key, size_t start, size_t len, int (*strcmp_p)(const char *, const char *));

public:
	Dict(size_t keySize, size_t valueSize, size_t len, PGM_P data)
	: mKeySize(keySize)
	, mValueSize(valueSize)
	, mLen(len)
	, mData(data)
	{}
	~Dict() = default;

	size_t size() const { return (mLen); }

	PGM_P getFlash(const char *key);
	PGM_P get(const char *key);
};
