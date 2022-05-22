/**
\file
\brief Generates area of intersection of two rectangles
*/
#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#include "../../homog2d.hpp"

using namespace h2d;
using namespace h2d::img;

int main( int argc, const char** argv )
{
	int n=30;
	auto im_w = 350;
	auto im_h = 180;
	img::Image<cv::Mat> imga( im_w, im_h );

	FRect r1(40,30, 130,90 );
	FRect r2(160,45, 210,150 );

	auto color_red = DrawParams().setColor(200,20,20);
	auto color_green = DrawParams().setColor(20,220,20);
	int mul = 1;
	for( int i=0; i<n; i++ )
	{
		imga.clear();
		getBB(r1,r2).draw( imga, DrawParams().setColor(200,200,200) );

		r1.draw( imga, color_green );
		r2.draw( imga, color_green );

#if 0                                  // either way is the same
		auto a = r1.intersectArea(r2);
#else
		auto a = r1&r2;
#endif

		if( a() )
			a.get().draw( imga, color_red );

		if( (i+1)%10 == 0 )
		{
			mul = mul*-1;
			r1.translate(0,30);
		}
		std::ostringstream ossa;
		ossa << "showcase2a_" << std::setfill('0') << std::setw(2) <<i << ".png";
		imga.write( ossa.str() );


		r1.translate( mul*20,0);
	}

}


