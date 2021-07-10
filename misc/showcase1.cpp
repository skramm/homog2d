/**
\file showcase1.cpp
\brief Generates showcase images
*/
#define HOMOG2D_USE_OPENCV
#include "../homog2d.hpp"

#include "opencv2/highgui.hpp"

using namespace homog2d;

int main( int argc, const char** argv )
{
	int n=20;
	cv::Mat img;
	auto im_w = 400;
	auto im_h = 300;
	img.create( im_h, im_w, CV_8UC3 );

	int offset_h = 100;
	int offset_v = 50;
	Homogr Hr( M_PI/n );
	Homogr HT1(  offset_h,  offset_v );
	Homogr HT2( -offset_h, -offset_v );
	Homogr H = HT1 * Hr * HT2;
	FRect_<double> rect( 0,0,200,100 );

	Polyline pl( rect, IsClosed::Yes );

//	pl = Homogr( im_w/2, im_h/2) * pl;
	pl = Homogr().addTranslation(im_w/2, im_h/2) * pl;

	Line2d li( Point2d(0,0), Point2d(200,80) );
	for( int i=0; i<n; i++ )
	{
		img = cv::Scalar(255,255,255);
		li.draw( img, CvDrawParams().setColor(20,250,20) );
		pl.draw( img, CvDrawParams().setColor(20,0,250) );
		auto inters = li.intersects( pl );
		if( inters() )
			draw( img, inters.get() );

		std::ostringstream oss;
		oss << "BUILD/showcase1_" << i << ".png";
		cv::imwrite( oss.str(), img );

		pl = H * pl;
	}

}


