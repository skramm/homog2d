/**
\file
\brief shows isInside circles vs polyline
*/

#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#include "../../homog2d.hpp"

using namespace h2d;

int main( int argc, const char** argv )
{
	auto n = 25; // nb images
	Circle cir( 1.5, 0.8, 5.2 );
	CPolyline pl;
	std::vector<Point2d> vpts{
		{ 0,0 },
		{ 2.,1. },
		{ 5,-1 },
		{ 6,2 },
		{ 1,3 }
	};
	pl.set( vpts );
	pl = Homogr().addTranslation(-2.5,-1.5)*pl;

	auto Hdraw = Homogr().setScale(10).addTranslation(110,80);

	img::Image<cv::Mat> im( 300, 250 );

	auto col_outside = img::DrawParams().setColor(50,20,200);
	auto col_inside  = img::DrawParams().setColor(200,20,20);
	Homogr H1 = Homogr().addScale(    1.1,    1.2 );
	Homogr H2 = Homogr().addScale( 1./1.1, 1./1.2 );

	for( int i=0; i<n; i++ )
	{
		im.clear();
		auto pl2  = Hdraw * pl;
		auto cir2 = Hdraw * cir;

		auto col_c = col_outside;
		if( pl.isInside( cir ) )
			col_c = col_inside;

		auto col_p = col_outside;
		if( cir.isInside( pl ) )
			col_p = col_inside;

		pl2.draw( im, col_p );
		cir2.draw( im, col_c );

		if( i<n/2 )
			pl = H1*pl;
		else
			pl = H2*pl;

		std::ostringstream oss;
		oss << "showcase7_" << std::setfill('0') << std::setw(2) <<i << ".png";
		im.write( oss.str() );
	}
}

