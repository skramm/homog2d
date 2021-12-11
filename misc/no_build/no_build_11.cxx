// Polyline: cannot create an Open polyline from a closed one
// (because we can't know where to open it)

	OPolyline cp;
	OPolyline op(cp);
