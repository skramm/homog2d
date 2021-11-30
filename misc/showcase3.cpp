/**
\file
\brief Generates
*/
#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#include "../homog2d.hpp"

#include "opencv2/highgui.hpp"

using namespace h2d;
using namespace h2d::img;

int main( int argc, const char** argv )
{
	auto n = 25; // nb images
	Polyline pl(IsClosed::Yes);

	pl.add( 5,5);
	pl.add( 7,3);
	pl.add( 6,  2);
	pl.add( 7.5, 1.);
	pl.add( 4, 0);
	pl.add( 5.5, 2.5);
	pl.add( 4., 1.2);
	pl.add( 3., 2.2);
	pl.add( 3.6, 4.1);
	pl.add( 1.6, 5.4);
	auto Hdraw = Homogr().setScale(30).addTranslation(10,30);

	Line2d li(1,1,6,3);
	cv::Mat cvimg;
	cvimg.create( 250, 300, CV_8UC3 );
	cvimg = cv::Scalar(255,255,255);
	img::Image<cv::Mat> img2( cvimg );

	Homogr H;
	H.addTranslation(-4,-3).addRotation(360./n/180*M_PI ).addTranslation(4,3);

	for( int i=0; i<n; i++ )
	{
		pl = H*pl;
		cvimg = cv::Scalar(255,255,255);
		auto pl2 = Hdraw * pl;
		pl2.draw( img2, DrawParams().setColor(250,0,20) );
		pl2.getBB().draw( img2, DrawParams().setColor(150,150,120) );
		draw( img2, Hdraw * li,  DrawParams().setColor(120,250,220) );
		auto pts = pl.intersects(li);
		auto pts2 = Hdraw * pts.get();
		draw( img2, pts2 );
		std::ostringstream ossa;
		ossa << "BUILD/showcase3_" << std::setfill('0') << std::setw(2) <<i << ".png";
		cv::imwrite( ossa.str(), cvimg );
	}
}

