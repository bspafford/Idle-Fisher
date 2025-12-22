#pragma once

#include "vector.h"
#include "stuff.h"

// where the image anchors relative to the screen
enum Anchor {
	ANCHOR_LEFT,
	ANCHOR_RIGHT,
	ANCHOR_TOP,
	ANCHOR_BOTTOM,
	ANCHOR_CENTER
};

vector GetAbsoluteLoc(vector loc, vector size, bool useWorldPos, vector pivot, Anchor xAnchor, Anchor yAnchor);