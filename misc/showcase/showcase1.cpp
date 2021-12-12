/**
\file
\brief Generates general showcase image
*/
#define HOMOG2D_USE_OPENCV
#include "../../homog2d.hpp"

#include "opencv2/highgui.hpp"

using namespace h2d;
using namespace h2d::img;

int main( int argc, const char** argv )
{
	int n=20;
	auto im_w = 400;
	auto im_h = 250;
	Image<cv::Mat> myImg( im_w, im_h );

	int offset_h = 100;
	int offset_v = 60;
	Homogr Hr( M_PI/n );
	Homogr HT1(  im_w/2,  100 );
	Homogr HT2( -im_w/2, -100 );
	Homogr H = HT1 * Hr * HT2;

	Circle cir_fixed( Point2d( 265,195), 40 );
	Circle cir( Point2d( 32,82), 22 );

//	Ellipse ell( Point2d( 85,195), 55, 15, 3 );
	FRect r_fixed( 40,85,130,155 );
	Segment seg( 85,210, 335, 25 );

	FRect_<double> rect( 0,0,200,80 );
	CPolyline pl;
	auto color_red = DrawParams().setColor(200,20,20);
	auto color_green = DrawParams().setColor(20,220,20);
	auto color_blue = DrawParams().setColor(20,20,220);

// move rectangle dx=100, dy=60
	pl = Homogr().addTranslation(100, 60) * pl;
	Line2d lih( Point2d(0,100), Point2d(200,100) );
	Line2d liv( Point2d(200,0), Point2d(200,400) );

	Line2d li( Point2d(0,0), Point2d(200,80) );
	for( int i=0; i<n; i++ )
	{
		myImg.clear();
//		cvmat = cv::Scalar(255,255,255);

//		ell.draw( myImg, color_green );
		lih.draw( myImg );
		liv.draw( myImg );
		li.draw( myImg, color_green );
		pl.draw( myImg, color_blue );

		if( cir.isInside( cir_fixed ) || cir.isInside( r_fixed ) )
			cir.draw( myImg, DrawParams().setColor(220,0,0) );
		else
			cir.draw( myImg, color_blue );

		r_fixed.draw( myImg, color_green );
		seg.draw( myImg, color_green );
		cir_fixed.draw( myImg, color_green );

		auto inters1 = pl.intersects( li );
		if( inters1() )
			draw( myImg, inters1.get(), color_red );

		auto inters2 = pl.intersects( r_fixed );
		if( inters2() )
			draw( myImg, inters2.get(), color_red );

		auto inters3 = pl.intersects( cir_fixed );
		if( inters3() )
			draw( myImg, inters3.get(), color_red );

		auto inters4 = pl.intersects( seg );
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


/* NOT YET IMPLEMENTED !
		auto interse = pl.intersects( ell );
		if( interse() )
			draw( myImg, interse.get(), color_red );
*/
		std::ostringstream oss;
		oss << "BUILD/showcase1_" << std::setfill('0') << std::setw(2) <<i << ".png";
		cv::imwrite( oss.str(), myImg.getReal() );

		pl = H * pl;
		lih = H*lih;
		liv = H*liv;

		cir.translate( 12,6);
	}
}

