#pragma once

#include <stddef.h>

class GlobalBuffer {
private:
	static char mFallback;

	static inline void setFallback() { setBuffer(&mFallback, 1); }

protected:
	static char *mData;
	static size_t mSize;

	static inline void setBuffer(char *buff, size_t size)
	{
		mData = buff;
		mSize = size;
	}

public:
	inline static char *raw()
	{
		if (!mData) setFallback();
		return (mData);
	}
	inline static size_t size()
	{
		if (!mData) setFallback();
		return (mSize);
	}

	operator char *(){ return (raw()); }
};

template <size_t buffSize>
class Buffer : public GlobalBuffer {
private:
	static char mBuffer[buffSize];

public:
	void init() { setBuffer(mBuffer, buffSize); }
};

template<size_t buffSize>
char Buffer<buffSize>::mBuffer[];
