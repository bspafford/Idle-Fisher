#include "vector.h"
#include "stuff.h"

vector vector::ToPixel() {
	return vector{ roundf(x * stuff::pixelSize) / stuff::pixelSize, roundf(y * stuff::pixelSize) / stuff::pixelSize };
}