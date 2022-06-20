/**
\file
\brief Generates general showcase image
*/
#define HOMOG2D_USE_OPENCV
#include "../../homog2d.hpp"

using namespace h2d;
using namespace h2d::img;

int main( int argc, const char** argv )
{
	auto nbim = 30;         // number of images in gif
	auto im_w = 400;
	auto im_h = 250;
	Image<cv::Mat> myImg( im_w, im_h );

	int offset_h = 100;
	int offset_v = 60;
	Homogr Hr( 2.*M_PI/nbim );    // set up rotation
	Homogr HT1(  im_w/2,  100 );  // centered on image center
	Homogr HT2( -im_w/2, -100 );
	Homogr H = HT1 * Hr * HT2;

// define some primitives
	Circle  cir_fixed( Point2d( 265,195), 40 );
	Circle  cir( Point2d( 32,82), 22 );
	FRect   r_fixed( 40,85,130,155 );
	Segment seg( 85,210, 335, 25 );
	FRect   rect( 0,0,200,80 );
	CPolyline pol{ std::vector<Point2d>{ {30,20}, {140,45}, {110,110}, {20,65} } };

// define some colors
	auto color_red   = DrawParams().setColor(200,20,20);
	auto color_green = DrawParams().setColor(20,220,20);
	auto color_blue  = DrawParams().setColor(20,20,220);
	auto color_poly  = DrawParams().setColor(180,0,180);

// move rectangle dx=100, dy=60
	pol = Homogr().addTranslation(75, 20) * pol;
	Line2d lih( Point2d(0,100), Point2d(200,100) );
	Line2d liv( Point2d(200,0), Point2d(200,400) );

	Line2d li( Point2d(0,0), Point2d(200,80) );
	for( int i=0; i<nbim; i++ )
	{
		myImg.clear();

		lih.draw( myImg );             // draw all the stuff
		liv.draw( myImg );
		li.draw( myImg, color_green );
		pol.draw( myImg, color_poly );

		if( cir.isInside( cir_fixed ) || cir.isInside( r_fixed ) )
			cir.draw( myImg, DrawParams().setColor(220,0,0) );
		else
			cir.draw( myImg, color_blue );

		r_fixed.draw( myImg, color_green );
		seg.draw( myImg, color_green );
		cir_fixed.draw( myImg, color_green );

		auto inters1 = pol.intersects( li );
		if( inters1() )
			draw( myImg, inters1.get(), color_red );

		auto inters2 = pol.intersects( r_fixed );
		if( inters2() )
			draw( myImg, inters2.get(), color_red );

		auto inters3 = pol.intersects( cir_fixed );
		if( inters3() )
			draw( myImg, inters3.get(), color_red );

		auto inters4 = pol.intersects( seg );
		if( inters4() )
			draw( myImg, inters4.get(), color_red );

		auto intersb1 = cir.intersects( seg );
		if( intersb1() )
			draw( myImg, intersb1.get(), color_red );

		auto intersb2 = cir.intersects( r_fixed );
		if( intersb2() )
			draw( myImg, intersb2.get(), color_red );

		auto intersb3 = cir.intersects( cir_fixed );
		if( intersb3() )
			draw( myImg, intersb3.get(), color_red );

		std::ostringstream oss;
		oss << "showcase1_" << std::setfill('0') << std::setw(2) <<i << ".png";
		myImg.write( oss.str() );

		pol = H * pol;   // rotate lines and polygon
		lih = H * lih;
		liv = H * liv;

		cir.translate( 12, 6 ); // move the circle
	}
}

