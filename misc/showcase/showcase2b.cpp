/**
\file
\brief Generates area of union of two rectangles
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
	img::Image<cv::Mat> imgb( im_w, im_h );

	FRect r1(40,30, 130,90 );
	FRect r2(160,45, 210,150 );

	auto color_red = DrawParams().setColor(200,20,20);
	auto color_green = DrawParams().setColor(20,220,20);
	int mul = 1;
	for( int i=0; i<n; i++ )
	{
		imgb.clear();
		getBB(r1,r2).draw( imgb, DrawParams().setColor(200,200,200) );

		r1.draw( imgb, color_green );
		r2.draw( imgb, color_green );

#if 0                                  // either way is the same
		auto b = r1.unionArea(r2);
#else
		auto b = r1|r2;
#endif

		b.draw( imgb, color_red );
		if( (i+1)%10 == 0 )
		{
			mul = mul*-1;
			r1.translate(0,30);
		}

		std::ostringstream ossb;
		ossb << "showcase2b_" << std::setfill('0') << std::setw(2) <<i << ".png";
		imgb.write( ossb.str() );

		r1.translate( mul*20,0);
	}

}


