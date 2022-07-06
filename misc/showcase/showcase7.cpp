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
/*		{ 5.5, 2.5 },
		{ 4., 1.2 },
		{ 3., 2.2 },
		{ 3.6, 4.1 },
		{ 1.6, 5.4 }*/
	};
	pl.set( vpts );
	pl = Homogr().addTranslation(-2.5,-1.5)*pl;

	auto Hdraw = Homogr().setScale(10).addTranslation(110,80);

	img::Image<cv::Mat> img2( 300, 250 );

	auto col_outside = img::DrawParams().setColor(50,20,200);
	auto col_inside  = img::DrawParams().setColor(200,20,20);
	Homogr H;
	H.addScale(1.1,1.2);

	for( int i=0; i<n; i++ )
	{
		img2.clear();
		auto pl2 = Hdraw * pl;
		auto cir2 = Hdraw * cir;

		auto col_c = col_outside;
		if( pl.isInside( cir ))
			col_c = col_inside;

		auto col_p = col_outside;
		if( cir.isInside( pl ))
			col_p = col_inside;

		pl2.draw( img2, col_p );
		cir2.draw( img2, col_c );

		pl = H*pl;
/*

		pl2.getBB().draw( img2, img::DrawParams().setColor(150,150,120) );
		draw( img2, Hdraw * li,  img::DrawParams().setColor(120,250,220) );
		auto pts = pl.intersects(li);
		auto pts2 = Hdraw * pts.get();
		draw( img2, pts2 );
*/
		std::ostringstream ossa;
		ossa << "showcase7_" << std::setfill('0') << std::setw(2) <<i << ".png";
		img2.write( ossa.str() );
	}
}

