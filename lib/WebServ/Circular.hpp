#include <stdlib.h>

template<typename T, size_t arrSize>
class Circular {
private:
	T mArray[arrSize];
	size_t mUsed = 0;
	size_t mIndex = 0;

public:
	Circular() = default;
	~Circular(){
		while (size())
			pop_front();
	}

	size_t size() const { return (mUsed); }

	bool push_back(const T &item)
	{
		if (mUsed >= arrSize)
			return (false);
		(*this)[mUsed] = item;
		mUsed ++;
		return (true);
	}
	template <typename... Args>
	bool emplace_back(Args&& ...args)
	{
		if (mUsed >= arrSize)
			return (false);
		new (&((*this)[mUsed])) T(static_cast<Args&&>(args)...);
		mUsed ++;
		return (true);
	}

	void pop_front()
	{
		if (mUsed <= arrSize)
			return ;
		(*this)[0].~T();
		mUsed --;
		mIndex = (mIndex + 1) % arrSize;
	}

	T &operator [](size_t index)
	{
		return (mArray[(mIndex + index) % arrSize]);
	}
	const T &operator [](size_t index) const
	{
		return (mArray[(mIndex + index) % arrSize]);
	}
};
