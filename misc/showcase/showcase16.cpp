/**
\file
\brief random points and a moving point, draw the nearest/farthest point segment
*/
#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#include "../../homog2d.hpp"

using namespace h2d;

int main( int argc, const char** argv )
{
	auto nbim = 25; // nb images
	auto imsize = 300;

	Homogr Hr( 2.*M_PI/nbim );    // set up rotation
	Homogr HT1(  imsize/2,  imsize/2 );  // centered on image center
	Homogr HT2( -imsize/2, -imsize/2 );
	Homogr H = HT1 * Hr * HT2;
//	std::cout << "H=" << H << '\n';

	Point2d pt0( imsize/2, 80 );
	Line2d li( 20,20, 100,100);
	img::Image<cv::Mat> im( imsize,imsize-100 );

	for( int i=0; i<nbim; i++ )
	{
		im.clear();
		pt0.draw( im, img::DrawParams().setColor(0,0,250) );
		li.draw( im, img::DrawParams().setThickness(2).setColor(250,0,0) );
		im.drawText( "side=" + std::to_string( side( pt0, li) ), Point2d(50,50), img::DrawParams() );
		li = H * li;

		std::ostringstream oss;
		oss << "showcase16_" << std::setfill('0') << std::setw(2) << i << ".png";
		im.write( oss.str() );
	}
}

