
#include <ttmath/ttmath.h>

#define HOMOG2D_INUMTYPE ttmath::Big<1,2>
#define HOMOG2D_USE_TTMATH
#define HOMOG2D_NOCHECKS
//#define HOMOG2D_DEBUGMODE

#include "../../../homog2d.hpp"

using namespace h2d;
//using namespace h2d::img;
int main()
{
	Point2d_<ttmath::Big<1,2>> p1;
	Point2d_<ttmath::Big<1,2>> p2(10,10);
	auto li = p1*p2;
	std::cout << li;
}
