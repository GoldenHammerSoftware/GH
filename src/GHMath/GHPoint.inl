// Copyright Golden Hammer Software
#include <assert.h>
#include <math.h>
#include <string.h>

template <typename T, size_t COUNT>
inline void GHPoint<T, COUNT>::setCoords(const T* coords) 
{
    /*
    // was getting EXC_ARM_DA_ALIGN errors on ios
    for (size_t i = 0; i < COUNT; ++i) 
    { 
        mCoords[i] = coords[i]; 
    }
    */
    const char* src = reinterpret_cast<const char*>(coords);
    char* dst = reinterpret_cast<char*>(&mCoords[0]);
    memcpy(dst, src, sizeof(T)*COUNT);
}

template <typename T, size_t COUNT>
inline T& GHPoint<T, COUNT>::operator[](size_t i)
{
	assert(i < COUNT);
	return mCoords[i];
}

template <typename T, size_t COUNT>
inline const T& GHPoint<T, COUNT>::operator[](size_t i) const
{
	assert(i < COUNT);
	return mCoords[i];
}

template <typename T, size_t COUNT>
inline bool GHPoint<T, COUNT>::operator==(const GHPoint<T,COUNT>& other) const
{
	const T* otherCoords = other.getCoords();
	for (size_t i = 0; i < COUNT; ++i) {
		if (mCoords[i] != otherCoords[i]) return false;
	}
	return true;
}

template <typename T, size_t COUNT>
inline void GHPoint<T, COUNT>::operator=(const GHPoint<T,COUNT>& other) 
{
	const T* otherCoords = other.getCoords();
	for (size_t i = 0; i < COUNT; ++i) {
		mCoords[i] = otherCoords[i];
	}
}

template <typename T, size_t COUNT>
inline bool GHPoint<T, COUNT>::operator<(const GHPoint<T, COUNT>& other) const
{
	const T* otherCoords = other.getCoords();
	for (size_t i = 0; i < COUNT; ++i) {
		if (mCoords[i] < otherCoords[i]) {
			return true;
		}
		if (mCoords[i] > otherCoords[i]) {
			return false;
		}
	}
	return false;
}

template <typename T, size_t COUNT>
inline GHPoint<T,COUNT>& GHPoint<T, COUNT>::operator+=(const GHPoint<T,COUNT>& other) 
{
	const T* otherCoords = other.getCoords();
	for (size_t i = 0; i < COUNT; ++i) {
		mCoords[i] += otherCoords[i];
	}
    return *this;
}

template <typename T, size_t COUNT>
inline GHPoint<T,COUNT>& GHPoint<T, COUNT>::operator-=(const GHPoint<T,COUNT>& other) 
{
	const T* otherCoords = other.getCoords();
	for (size_t i = 0; i < COUNT; ++i) {
		mCoords[i] -= otherCoords[i];
	}
    return *this;
}

template <typename T, size_t COUNT>
inline GHPoint<T,COUNT>& GHPoint<T, COUNT>::operator*=(const GHPoint<T,COUNT>& other) 
{
	const T* otherCoords = other.getCoords();
	for (size_t i = 0; i < COUNT; ++i) {
		mCoords[i] *= otherCoords[i];
	}
    return *this;
}

template <typename T, size_t COUNT>
inline GHPoint<T, COUNT>& GHPoint<T, COUNT>::operator/=(const GHPoint<T, COUNT>& other)
{
	const T* otherCoords = other.getCoords();
	for (size_t i = 0; i < COUNT; ++i) {
		mCoords[i] /= otherCoords[i];
	}
	return *this;
}

template <typename T, size_t COUNT>
inline GHPoint<T,COUNT>& GHPoint<T, COUNT>::operator*=(const T& other)
{
	for (size_t i = 0; i < COUNT; ++i) {
		mCoords[i] *= other;
	}
    return *this;
}

template <typename T, size_t COUNT>
inline GHPoint<T,COUNT>& GHPoint<T, COUNT>::operator/=(const T& other)
{
	for (size_t i = 0; i < COUNT; ++i) {
		mCoords[i] /= other;
	}
    return *this;
}

template <typename T, size_t COUNT>
inline GHPoint<T,COUNT>& GHPoint<T, COUNT>::operator+=(const T& other)
{
	for (size_t i = 0; i < COUNT; ++i) {
		mCoords[i] += other;
	}
    return *this;
}

template <typename T, size_t COUNT>
inline GHPoint<T,COUNT>& GHPoint<T, COUNT>::operator-=(const T& other)
{
	for (size_t i = 0; i < COUNT; ++i) {
		mCoords[i] -= other;
	}
    return *this;
}

template <typename T, size_t COUNT>
inline float GHPoint<T, COUNT>::operator*(const GHPoint<T,COUNT>& other) const
{
	const T* otherCoords = other.getCoords();
	float ret = 0;
	for (size_t i = 0; i < COUNT; ++i) {
		ret += mCoords[i] * otherCoords[i];
	}
	return ret;
}

template <typename T, size_t COUNT>
inline void GHPoint<T, COUNT>::plus(const GHPoint<T,COUNT>& second, GHPoint<T,COUNT>& ret) const
{
	const T* otherCoords = second.getCoords();
	for (size_t i = 0; i < COUNT; ++i) {
		ret[i] = mCoords[i] + otherCoords[i];
	}
}

template <typename T, size_t COUNT>
inline void GHPoint<T, COUNT>::minus(const GHPoint<T,COUNT>& second, GHPoint<T,COUNT>& ret) const
{
	const T* otherCoords = second.getCoords();
	for (size_t i = 0; i < COUNT; ++i) {
		ret[i] = mCoords[i] - otherCoords[i];
	}
}

template <typename T, size_t COUNT>
inline void GHPoint<T, COUNT>::mult(const T& other, GHPoint<T,COUNT>& ret) const
{
	for (size_t i = 0; i < COUNT; ++i) {
		ret[i] = mCoords[i] * other;
	}
}

template <typename T, size_t COUNT>
inline void GHPoint<T, COUNT>::div(const T& other, GHPoint<T,COUNT>& ret) const
{
	for (size_t i = 0; i < COUNT; ++i) {
		ret[i] = mCoords[i] / other;
	}
}

template <typename T, size_t COUNT>
inline float GHPoint<T, COUNT>::normalize(void)
{
	float len = length();
	// avoid div by 0.
	// todo: figure out the right thing to do.
	if (len < 0.00001) return len; 
	for (size_t i = 0; i < COUNT; ++i) {
        mCoords[i] /= len;
	} 
	return len;
}

template <typename T, size_t COUNT>
inline float GHPoint<T, COUNT>::length(void) const
{
	return (float)::sqrt(lenSqr());
}

template <typename T, size_t COUNT>
inline float GHPoint<T, COUNT>::lenSqr(void) const
{
	float val = 0;
	for (size_t i = 0; i < COUNT; ++i) {
		val += mCoords[i]*mCoords[i];
	}
	return val;
}

template <typename T, size_t COUNT>
inline void GHPoint<T, COUNT>::linearInterpolate(const GHPoint<T, COUNT>& other, float tVal, GHPoint<T, COUNT>& ret) const
{
	ret = other;
	ret -= *this;
	ret *= tVal;
	ret += *this;
}

template <typename T, size_t COUNT>
inline void GHPoint<T, COUNT>::clamp(const GHPoint<T, COUNT>& min, const GHPoint<T, COUNT>& max)
{
	for (size_t i = 0; i < COUNT; ++i)
	{
		if (mCoords[i] < min[i]) mCoords[i] = min[i];
		if (mCoords[i] > max[i]) mCoords[i] = max[i];
	}
}

template <typename T, size_t COUNT>
inline void GHPoint<T, COUNT>::fillBuffer(T* buffer) const
{
	::memcpy(buffer, mCoords, COUNT*sizeof(T));
}
