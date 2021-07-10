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
	img.create( 500, 400, CV_8UC3 );

	int offset_h = 200;
	int offset_v = 50;
	Homogr Hr( M_PI/n );
	Homogr HT1(  offset_h,  offset_v );
	Homogr HT2( -offset_h, -offset_v );
	FRect_<double> rect( 0,0,200,100 );
	Polyline pl( rect, IsClosed::Yes );
	Line2d li( Point2d(0,0), Point2d(200,80) );
	for( int i=0; i<n; i++ )
	{
		img = cv::Scalar(255,255,255);
		li.draw( img, CvDrawParams().setColor(20,250,20) );
		pl.draw( img, CvDrawParams().setColor(20,0,250) );
		auto inters = li.intersects( pl );
		if( inters() )
		{
			draw( img, inters.get() );

		}

		std::ostringstream oss;
		oss << "showcase1_" << i << ".png";
		cv::imwrite( oss.str(), img );
		std::cout << "pl=" << pl << "\n";

		Homogr H = HT1 * Hr * HT2;
		pl = H * pl;
	}

}


