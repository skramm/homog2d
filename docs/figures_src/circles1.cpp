#define HOMOG2D_USE_OPENCV

#include "homog2d.hpp"
#include "opencv2/highgui.hpp"

using namespace homog2d;
int main()
{
	Circle c1( Point2d(120,100), 80 );
	Circle c2( Point2d(300,200), 60 );
	auto seg = getSegment( c1, c2 );
	auto pseg = getTanSegs( c1, c2 );

	cv::Mat img;
	img.create( 300, 450, CV_8UC3 );
	img = cv::Scalar(255,255,255);
	c1.draw( img, CvDrawParams().setColor(0,0,100) );
	c2.draw( img, CvDrawParams().setColor(0,0,100) );
	seg.draw( img, CvDrawParams().setColor(250,0,0) );
	pseg.first.draw( img,  CvDrawParams().setColor(0,200,100) );
	pseg.second.draw( img, CvDrawParams().setColor(0,100,200) );
	cv::imwrite( "circles1.png", img );
}
