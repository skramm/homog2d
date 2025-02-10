/**
\file
\brief Three circles, one fixed and two whose center rotates. Fixed radius.
It shows the line formed by intersections points. The three lines intersect at same point (black).
*/
#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#include "../../homog2d.hpp"

using namespace h2d;
using namespace h2d::img;

int main( int, const char** )
{
	auto nbim = 25; // nb images

	uint8_t m = 240;

	CPolyline pol( 50, 3 );
	pol.translate( 150, 160 );
	auto pts = pol.getPts();
	auto pt0 = pts[0];
	auto pt1 = pts[1];
	auto pt2 = pts[2];

	auto k1=6;
	auto k2=12;

	auto rad0 = 65;
	auto rad1 = 70;
	auto rad2 = 83;
	for( int i=0; i<nbim; i++ )
	{
		Image<img::SvgImage> im( 320, 320 );

		auto angle = 2. * M_PI * i / nbim;
		auto xr1 = std::cos(angle)*k1;
		auto yr1 = std::sin(angle)*k1;
		auto xr2 = std::cos(angle)*k2;
		auto yr2 = std::sin(angle)*k2;

		Point2d p2( pt2 );
		Point2d p1( pt1 );
		p1.translate( xr1, -yr1 );
		p2.translate( xr2, yr2 );

		Circle cir1( p1, rad1 );
		Circle cir2( p2, rad2 );
		Circle cir0( pt0, rad0 );

		std::array<Circle,3> arr;
		arr[0] = cir0;
		arr[1] = cir1;
		arr[2] = cir2;
		draw( im, getBB( arr ), DrawParams().setColor(220,220,220) );

#if 0
		p1.draw( im, DrawParams().setColor(250,0,0) );
		p2.draw( im, DrawParams().setColor(0,0,250) );
		pt0.draw( im, DrawParams().setColor(0,250,0) );
#endif
		auto int01 = cir0.intersects(cir1).get();
		auto int02 = cir0.intersects(cir2).get();
		auto int12 = cir2.intersects(cir1).get();

		auto li01 = int01[0] * int01[1];
		auto li02 = int02[0] * int02[1];
		auto li12 = int12[0] * int12[1];

		li01.draw( im, DrawParams().setColor(m,m,0) );
		li02.draw( im, DrawParams().setColor(0,m,m) );
		li12.draw( im, DrawParams().setColor(m,0,m) );

		draw( im, getTanSegs( cir0, cir1 ), DrawParams().setColor(m,m,m/3) );
		draw( im, getTanSegs( cir0, cir2 ), DrawParams().setColor(m/3,m,m) );
		draw( im, getTanSegs( cir1, cir2 ), DrawParams().setColor(m,m/3,m) );

		cir0.draw( im, DrawParams().setColor(0,250,0) );
		cir1.draw( im, DrawParams().setColor(250,0,0) );
		cir2.draw( im, DrawParams().setColor(0,0,250) );

		auto centerpt = li01 * li02;
		centerpt.draw( im, DrawParams().setColor(0,0,0).setPointStyle(PtStyle::Dot) );

		std::ostringstream oss;
		oss << "showcase20_" << std::setfill('0') << std::setw(2) << i << ".svg";
		im.write( oss.str() );
	}
}

