/**
\file
\brief Generates a rotating polygon with a crossing line, computes the intersection points
*/
#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#include "../../homog2d.hpp"

using namespace h2d;

int main( int argc, const char** argv )
{
	auto n = 25; // nb images
	CPolyline pl;
	std::vector<Point2d> vpts{
		{ 5,5 },
		{ 7,3 },
		{ 6,  2 },
		{ 7.5, 1. },
		{ 4, 0 },
		{ 5.5, 2.5 },
		{ 4., 1.2 },
		{ 3., 2.2 },
		{ 3.6, 4.1 },
		{ 1.6, 5.4 }
	};
	pl.set( vpts );

	auto Hdraw = Homogr().setScale(30).addTranslation(10,30);

	Line2d li(1,1,6,3);
	img::Image<cv::Mat> img2( 300, 250 );

	Homogr H;
	H.addTranslation(-4,-3).addRotation(360./n/180*M_PI ).addTranslation(4,3);

	for( int i=0; i<n; i++ )
	{
		pl = H*pl;
		img2.clear();
		auto pl2 = Hdraw * pl;
		pl2.draw( img2, img::DrawParams().setColor(250,0,20) );
		pl2.getBB().draw( img2, img::DrawParams().setColor(150,150,120) );
		draw( img2, Hdraw * li,  img::DrawParams().setColor(120,250,220) );
		auto pts = pl.intersects(li);
		auto pts2 = Hdraw * pts.get();
		draw( img2, pts2 );
		std::ostringstream ossa;
		ossa << "showcase3_" << std::setfill('0') << std::setw(2) <<i << ".png";
		img2.write( ossa.str() );
	}
}

