// Polyline: cannot set an Open polyline from a rectangle
// (because we can't know where to open it)

	FRect rect;
	OPolyline op;
	op.set(rect);
