/**
\file
\brief Generates random points, the Convex Hull, the Bounding Box, the Minimum Enclosing Circle (MEC), and the extremum points
*/
#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#include "../../homog2d.hpp"

using namespace h2d;

int main( int argc, const char** argv )
{
	auto nbim = 20; // nb images

	auto cmax = 250;
	auto cmin = 50;
	auto nbpts_max = 50;
	auto nbpts_min = 5;

	auto extr_col = img::DrawParams().setColor(100,250,0).setPointStyle( img::PtStyle::Dot );
	for( int i=0; i<nbim; i++ )
	{
		img::Image<cv::Mat> im( cmax+30, cmax+30 );
		int nbpts = 1.0*rand() / RAND_MAX * (nbpts_max-nbpts_min) + nbpts_min;
		std::vector<Point2d> vec(nbpts);
		auto it = std::begin(vec);
		for( auto i=0; i<nbpts; i++ )
			*it++ = Point2d(
				1.0*rand() / RAND_MAX * (cmax-cmin) + cmin,
				1.0*rand() / RAND_MAX * (cmax-cmin) + cmin
		);
		auto ch = convexHull( vec );
		draw( im, vec );
		ch.draw( im, img::DrawParams().setColor(250,100,100) );
		getBB(vec).draw( im, img::DrawParams().setColor(100,100,250) );

		Circle cir;
		cir.set( vec ); // Minimum Enclosing Circle (MEC)
		cir.draw( im, img::DrawParams().setColor(0,100,150) );

		getTmPoint(vec).draw( im, extr_col );
		getRmPoint(vec).draw( im, extr_col );
		getLmPoint(vec).draw( im, extr_col );
		getBmPoint(vec).draw( im, extr_col );

		std::ostringstream oss;
		oss << "showcase13_" << std::setfill('0') << std::setw(2) << i << ".png";
		im.write( oss.str() );
	}
}

