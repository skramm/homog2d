
#define HOMOG2D_DEBUGMODE
#include "../../homog2d.hpp"

using namespace h2d;
int main()
{
	OPolyline poly(
		std::vector<Point2d>{
			Point2d( 0,0),
			Point2d( 10,1),
			Point2d( 20,0),
			Point2d( 30, 2),
			Point2d( 40,0),
		}
	);
	poly.minimize();
}
