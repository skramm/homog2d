/**
\file
\brief This is a test file demonstrating the usage of the ttmath library.

See homog2d_manual.md#bignum

Build and run with <code>$ make test_bn</code>
*/

#define HOMOG2D_INUMTYPE ttmath::Big<2,2>
#define HOMOG2D_USE_TTMATH
//#define HOMOG2D_NOCHECKS

#include "../../homog2d.hpp"

using namespace h2d;
using BigM32 = ttmath::Big<3,2>;
int main()
{
	Point2d_<ttmath::Big<3,2>> p1;
	Point2d_<ttmath::Big<1,2>> p2(10,10);
	auto li = p1*p2;    // automatic type conversions (p1,p2)
	std::cout << li << '\n';
//	std::cout << li.dtype();
//	Segment s0( 0,0,45,45);  // this cannot compile, you need to provide an underlying ttmath type
	Segment_<ttmath::Big<1,1>> s1( -8,5,10,-15);  // this is ok;
	Segment_<BigM32>           s2( 1,2,3,4 );     // can be easier to read
	auto it0 = s1.intersects( s2 );
	auto it = s1.intersects( li );
	if( it() )
		std::cout << it.get() << '\n';

	Circle_<BigM32> cir;
	FRect_<BigM32> rect;

	auto it2 = cir.intersects(s1);
	auto it3 = cir.intersects(li);
	auto it4 = cir.intersects(rect);
	auto it5 = li.intersects(rect);
	auto it6 = s1.intersects(rect);
}
