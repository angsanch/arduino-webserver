#pragma once

#include <stddef.h>

class GlobalBuffer {
protected:
	static char *mData;
	static size_t mSize;

	static inline void setBuffer(char *buff, size_t size)
	{
		mData = buff;
		mSize = size;
	}

public:
	inline char *raw() { return (mData); }
	inline size_t size() { return (mSize); }
};

template <size_t buffSize>
class Buffer : public GlobalBuffer {
private:
	static char mBuffer[buffSize];

public:
	void init() {
		GlobalBuffer().setBuffer(mBuffer, buffSize);
	}
};
