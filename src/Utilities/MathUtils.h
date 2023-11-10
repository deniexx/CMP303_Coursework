#pragma once

template<typename T>
T Clamp(T a, T min, T max)
{
	T toReturn = a > max ? max : a;
	return toReturn < min ? min : a;
}