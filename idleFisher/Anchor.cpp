#include "Anchor.h"

vector GetAbsoluteLoc(vector loc, vector size, bool useWorldPos, vector pivot, Anchor xAnchor, Anchor yAnchor) {
	loc = loc.ToPixel();
	 vector pivotLoc = (size * pivot).floor();
	 if (useWorldPos) {
		 return (loc - pivotLoc).ToPixel();
	 } else {
		 vector newLoc;
		 vector pixelScreenSize = stuff::screenSize / stuff::pixelSize;
		 if (xAnchor == ANCHOR_LEFT) { // if anchor left
			 newLoc.x = loc.x;
		 } else if (xAnchor == ANCHOR_CENTER) {
			 newLoc.x = (loc.x + pixelScreenSize.x / 2.f);
		 } else if (xAnchor == ANCHOR_RIGHT) { // if anchor right
			 newLoc.x = (loc.x + pixelScreenSize.x);
		 }

		 if (yAnchor == ANCHOR_BOTTOM) { // if anchor bottom
			 newLoc.y = (loc.y);
		 } else if (yAnchor == ANCHOR_CENTER) { // if anchor bottom
			 newLoc.y = (loc.y + pixelScreenSize.y / 2.f);
		 } else if (yAnchor == ANCHOR_TOP) { // if anchor top
			 newLoc.y = (loc.y + pixelScreenSize.y);
		 }

		 return (newLoc - pivotLoc).floor();
	 }
}