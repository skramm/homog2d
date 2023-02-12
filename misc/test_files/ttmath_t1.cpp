
#include <ttmath/ttmath.h>

#define HOMOG2D_INUMTYPE ttmath::Big<2,2>
#define HOMOG2D_USE_TTMATH
//#define HOMOG2D_NOCHECKS

#include "../../homog2d.hpp"

using namespace h2d;

int main()
{
	Point2d_<ttmath::Big<3,2>> p1;
	Point2d_<ttmath::Big<1,2>> p2(10,10);
	auto li = p1*p2;
	std::cout << li << '\n';

//	Segment s1( 0,0,45,45);  // this cannot compile
	Segment_<ttmath::Big<1,1>> s1( -8,5,10,-15);  // this is ok;
	auto it = s1.intersects( li );
	if( it() )
		std::cout << it.get() << '\n';
}
