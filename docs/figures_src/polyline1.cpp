#define HOMOG2D_USE_OPENCV

#include "homog2d.hpp"
#include "opencv2/highgui.hpp"

using namespace h2d;
int main()
{
	std::srand(time(0));
	Polyline pl(IsClosed::Yes);
	for( int i=0; i<7; i++ )
		pl.add(
			400.*rand()/RAND_MAX+20,
			300.*rand()/RAND_MAX+20
		);

	cv::Mat img;
	img.create( 350, 450, CV_8UC3 );
	img = cv::Scalar(255,255,255);
	pl.draw( img, CvDrawParams().setColor(250,0,20) );

	cv::imwrite( "polyline1.png", img );
}
