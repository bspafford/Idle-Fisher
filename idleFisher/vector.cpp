#include "vector.h"
#include "stuff.h"

vector vector::ToPixel() {
	return vector{ floorf(x * stuff::pixelSize) / stuff::pixelSize, floorf(y * stuff::pixelSize) / stuff::pixelSize };
}