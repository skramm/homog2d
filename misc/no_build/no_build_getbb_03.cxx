// cannot use getBB() on a Point... except if we use the variant-based runtime polymorphism

#ifndef HOMOG2D_ENABLE_VRTP
	Point2d pt;
	auto bb = getBB( pt );
#else
// This is just to make sure the compile step fails in case we do have the variant-based runtime polymorphism enabled
	aaa
#endif
