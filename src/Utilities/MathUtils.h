#pragma once

template<typename T>
T Clamp(T a, T min, T max)
{
	T toReturn = a > max ? max : a;
	return toReturn < min ? min : toReturn;
}

template<typename T>
T Lerp(T a, T b, float value)
{
	return ((1 - value) * a) + (value * b);
}
