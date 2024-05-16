// cannot use getBB() on a Line... except if we use the variant-based runtime polymorphism

#ifndef HOMOG2D_ENABLE_VRTP
	Line2d li;
	auto bb = getBB( li );
#else
// This is just to make sure the compile step fails in case we do have the variant-based runtime polymorphism enabled
	aaa
#endif
