/**
\file
\brief random point and a moving vector, show side of point
(WIP !!!)
*/
#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#include "../../homog2d.hpp"

using namespace h2d;

int main( int, const char** )
{
	auto nbim = 25; // nb images
	auto imsize = 300;

	auto kx1 = 20.;
	auto ky1 = 33;
	auto kx2 = 16;
	auto ky2 = 12.1;
	auto rot2 = 2.2;
	auto x0 = imsize/2 + 20;
	auto y0 = imsize/2 - 30;

	Point2d pt0( imsize/2, 80 );
//	Line2d li( 20,20, 100,100);
//	Vector v( 200,20,250,250 );
	img::Image<cv::Mat> im( imsize*2,imsize-100 );

	for( int i=0; i<nbim; i++ )
	{
		im.clear();
		auto angle =  i * 360. /nbim;
		std::cout << "angle=" << angle << '\n';
//		auto angle = i*360./nbim;
		auto angle1 = M_PI * angle / 180;
		auto x1 = std::cos(angle1)*kx1;
		auto y1 = std::sin(angle1)*ky1;

		auto angle2 = M_PI * angle * rot2 / 180;
		auto x2 = std::cos(angle2)*kx2;
		auto y2 = std::sin(angle2)*ky2;

		Point2d ptv1( x0+x1,     y0+y1 );
		Point2d ptv2( x0+x2+100, y0+y2 );
		Vector v( ptv1, ptv2 );
//		std::cout << "x1=" << x0+x1 << " y1=" << y0+y1 << " x2=" << x0+x2 << " y2=" << y0+y2 << '\n';

		v.getLine().draw( im );
		v.draw( im, img::DrawParams().setColor(0,0,250).showPoints() );

		pt0.draw( im, img::DrawParams().setColor(0,0,250) );
//		li.draw( im, img::DrawParams().setThickness(2).setColor(250,0,0) );

		im.drawText( std::string("side=") + getString( v.getPointSide(pt0) ), Point2d(50,50), img::DrawParams() );


		std::ostringstream oss;
		oss << "showcase16b_" << std::setfill('0') << std::setw(2) << i << ".png";
		im.write( oss.str() );
	}
}

