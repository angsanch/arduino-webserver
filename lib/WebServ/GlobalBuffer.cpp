#include "GlobalBuffer.hpp"

char *GlobalBuffer::mData = nullptr;
size_t GlobalBuffer::mSize = 0;
char GlobalBuffer::mFallback = '\0';
