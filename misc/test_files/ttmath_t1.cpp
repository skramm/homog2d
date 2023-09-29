/**
\file
\brief This is a test file demonstrating the usage of the ttmath library.

See homog2d_manual.md#bignum

Build and run with <code>$ make test_bn</code>
*/

//#define HOMOG2D_INUMTYPE ttmath::Big<2,2>
#define HOMOG2D_USE_TTMATH
//#define HOMOG2D_NOCHECKS

#include "../../homog2d.hpp"

using namespace h2d;
using BigM32 = ttmath::Big<3,2>;
int main()
{
	Point2d_<ttmath::Big<3,2>> p1;
	Point2d_<ttmath::Big<1,2>> p2(10,10);
	std::cout << "FP size of p1: mantissa=" << p1.dsize().first << ", exponent=" << p1.dsize().second << " bits\n";
	std::cout << "FP size of p2: mantissa=" << p2.dsize().first << ", exponent=" << p2.dsize().second << " bits\n";

	auto li = p1*p2;    // automatic type conversions between two types with different underlying numerical types
	std::cout << li << '\n';
	std::cout << "FP type of line=" << getString(li.dtype()) << '\n';
	std::cout << "FP size of line: mantissa=" << li.dsize().first << ", exponent=" << li.dsize().second << " bits\n";

	Segment                    s0( 0,0,45,45);    // uses the default ttmath type
	Segment_<ttmath::Big<3,1>> s1( -8,5,10,-15);  // specific type
	Segment_<BigM32>           s2( 1,2,3,4 );     // this can be easier to read
	auto it0 = s1.intersects( s2 );
	auto it = s1.intersects( li );
	if( it() )
		std::cout << "Intersection of segment and line: " << it.get() << '\n';

	std::cout << "FP type of s1=" << getString(s1.dtype()) << '\n';
	std::cout << "FP size of s0: mantissa=" << s0.dsize( ).first << ", exponent=" << s0.dsize().second << " bits\n";
	std::cout << "FP size of s1: mantissa=" << s1.dsize( ).first << ", exponent=" << s1.dsize().second << " bits\n";

	Circle_<BigM32> cir;
	FRect_<BigM32> rect;

	auto it2 = cir.intersects(s1);
	auto it3 = cir.intersects(li);
	auto it4 = cir.intersects(rect);
	auto it5 = li.intersects(rect);
	auto it6 = s1.intersects(rect);
}
