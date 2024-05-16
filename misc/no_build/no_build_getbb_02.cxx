// cannot use getBB() on a Segment... except if we use the variant-based runtime polymorphism

#ifndef HOMOG2D_ENABLE_VRTP
	Segment se;
	auto bb = getBB( se );
#endif
