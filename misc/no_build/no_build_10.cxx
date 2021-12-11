// homography * FRect does NOT produce a FRect !

	FRect rect;
	Homogr H;
	FRect rect2 = H * rect;
