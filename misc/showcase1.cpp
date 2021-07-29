/**
\file
\brief Generates general showcase image
*/
#define HOMOG2D_USE_OPENCV
#include "../homog2d.hpp"

#include "opencv2/highgui.hpp"

using namespace h2d;
using namespace h2d::img;

int main( int argc, const char** argv )
{
	int n=20;
	cv::Mat cvmat;
	auto im_w = 400;
	auto im_h = 250;
	cvmat.create( im_h, im_w, CV_8UC3 );
	Image<cv::Mat> myImg( cvmat );

	int offset_h = 100;
	int offset_v = 60;
	Homogr Hr( M_PI/n );
	Homogr HT1(  im_w/2,  100 );
	Homogr HT2( -im_w/2, -100 );
	Homogr H = HT1 * Hr * HT2;

	Circle cir( Point2d( 265,195), 40 );
//	Ellipse ell( Point2d( 85,195), 55, 15, 3 );
	FRect r_fixed( 40,100,130,155 );
	Segment seg( 85,210, 335, 25 );

	FRect_<double> rect( 0,0,200,80 );
	Polyline pl( rect, IsClosed::Yes );
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
		cvmat = cv::Scalar(255,255,255);

//		ell.draw( myImg, color_green );
		lih.draw( myImg );
		liv.draw( myImg );
		li.draw( myImg, color_green );
		pl.draw( myImg, DrawParams().setColor(20,0,250) );

		r_fixed.draw( myImg, color_green );
		seg.draw( myImg, color_green );
		cir.draw( myImg, color_green );

		auto inters = pl.intersects( li );
		if( inters() )
			draw( myImg, inters.get(), color_red );

		auto inters2 = pl.intersects( r_fixed );
		if( inters2() )
			draw( myImg, inters2.get(), color_red );

		auto inters3 = pl.intersects( cir );
		if( inters3() )
			draw( myImg, inters3.get(), color_red );

		auto inters4 = pl.intersects( seg );
		if( inters4() )
			draw( myImg, inters4.get(), color_red );


/* NOT YET IMPLEMENTED !
		auto interse = pl.intersects( ell );
		if( interse() )
			draw( myImg, interse.get(), color_red );
*/
		std::ostringstream oss;
		oss << "BUILD/showcase1_" << std::setfill('0') << std::setw(2) <<i << ".png";
		cv::imwrite( oss.str(), cvmat );

		pl = H * pl;
		lih = H*lih;
		liv = H*liv;
	}
}

