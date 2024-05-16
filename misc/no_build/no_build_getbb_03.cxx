// cannot use getBB() on a Point... except if we use the variant-based runtime polymorphism

#ifndef HOMOG2D_ENABLE_VRTP
	Point2d pt;
	auto bb = getBB( pt );
#endif
