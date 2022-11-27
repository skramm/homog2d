/**
\file
\brief Generates two moving circles and the tangential segments
*/
#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#include "../../homog2d.hpp"

using namespace h2d;
using namespace h2d::img;

int main( int argc, const char** argv )
{
	auto nbim = 25; // nb images
	auto rad_0 = 20;
	auto x0 = 110;
	auto y0 = 90;
	auto k1=70;
	auto k2=15;
	uint8_t g = 180;
	Circle cir1( 120, 80, 25 );

	for( int i=0; i<nbim; i++ )
	{
		auto angle = i * 2. * M_PI / nbim;
		auto x1 = std::cos(angle)*k1;
		auto y1 = std::sin(angle)*k1;
		auto rad = rad_0 + std::sin(angle)*k2;

		Circle cir2( Point2d(x0+x1, y0+y1), rad );

		Image<cv::Mat> im( 250, 220 );
		auto seg = getSegment( cir1, cir2 ); //.draw( im, DrawParams().setColor(150,150,0) );
		seg.draw( im, DrawParams().setColor(150,150,0) );

		cir1.draw( im, DrawParams().setColor(250,0,0) );
		cir2.draw( im, DrawParams().setColor(0,0,250) );

		getBB(cir1, cir2).draw( im, DrawParams().setColor(g,g,g) );

		auto lightgray = DrawParams().setColor(200,200,200);
		seg.getLine().getOrthogonalLine( seg.getPts().first ).draw( im,  lightgray );
		seg.getLine().getOrthogonalLine( seg.getPts().second ).draw( im, lightgray );

		auto segs = getTanSegs(cir1,cir2);
		draw( im, segs.first,  DrawParams().setColor(250,250,0) );
		draw( im, segs.second, DrawParams().setColor(0,250,250) );
		std::ostringstream oss;
		oss << "showcase9_" << std::setfill('0') << std::setw(2) <<i << ".png";
		im.write( oss.str() );
	}
}

