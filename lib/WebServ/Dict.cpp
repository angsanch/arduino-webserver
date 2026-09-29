#include "Dict.hpp"

PGM_P Dict::search(const char *key, size_t start, size_t len, int (*strcmp_p)(const char *, const char *))
{
	if (len == 0)
		return (nullptr);
	size_t searched = len / 2;
	int result = strcmp_p(key, &mData[(mKeySize + mValueSize) * (searched + start)]);
	if (result == 0)
		return (static_cast<PGM_P>(&mData[(mKeySize + mValueSize) * (searched + start) + mKeySize]));
	if (result < 0)
		return (search(key, start, searched, strcmp_p));
	return (search(key, searched + start + 1, len - searched - 1, strcmp_p));
}

int strcmp_P_P(const char *s1, const char *s2)
{
    while (true) {
        unsigned char c1 = pgm_read_byte(s1++);
        unsigned char c2 = pgm_read_byte(s2++);

        if (c1 != c2)
            return c1 - c2;

        if (c1 == '\0')
            return 0;
    }
}

PGM_P Dict::getFlash(const char *key)
{
	if (strlen_P(key) > mKeySize)
		return (nullptr);
	return (search(key, 0, size(), strcmp_P_P));
}

PGM_P Dict::get(const char *key)
{
	if (strlen(key) > mKeySize)
		return (nullptr);
	return (search(key, 0, size(), strcmp_P));
}

