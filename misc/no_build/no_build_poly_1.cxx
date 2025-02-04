// Polyline: cannot create an Open polyline from a closed one
// (because we can't know where to open it)

	CPolyline cp;
	OPolyline op(cp);
