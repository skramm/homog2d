/**
\file
\brief Generates a circle from 2/3 points
*/

#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#include "../../homog2d.hpp"

using namespace h2d;
using namespace h2d::img;

int main( int argc, const char** argv )
{
	auto nbim = 25; // nb images
	auto Hdraw = Homogr().setScale(30).addTranslation(10,10);

	auto r_w0 = 2;
	auto r_h0 = 1;
	auto x0 = 3.;
	auto y0 = 1.;
	auto k=1.8;
	uint8_t g = 100;

	Point2d p1( 2, 4 );
	Point2d p2( 6, 5 );
	Segment seg( 1., 1., 4.5, 2.8 );

	for( int i=0; i<nbim; i++ )
	{
		auto angle = i*360./nbim;
		auto angle_r = angle * M_PI / 180.;
		auto x1 = std::cos(angle_r)*k;
		auto y1 = std::sin(angle_r)*k;
		auto r_w = r_w0 + std::sin( 2.*M_PI*i/nbim );

		Point2d p0( x1+x0, y1+x0);
		std::vector<Point2d> v1{ p1, p2, p0 };
		std::vector<Point2d> v2{ p1, p0     };
		Circle cir1( p0,p1,p2);
		Circle cir2( p0,p1 );

		auto v1d = Hdraw * v1;
		auto v2d = Hdraw * v2;

		auto cir1d = Hdraw * cir1;
		auto cir2d = Hdraw * cir2;
		auto segd  = Hdraw * seg;

		Image<cv::Mat> im1( 250, 200 );

		auto dpinter= DrawParams().setPointStyle( PtStyle::Diam ).setColor( 20,250,20 );
		auto int1 = cir1.intersects( seg );
		auto int2 = cir2.intersects( seg );
		if( int1() )
			for( const auto pt: int1.get() )
				draw( im1, Hdraw * pt, dpinter );
		if( int2() )
			for( const auto pt: int2.get() )
				draw( im1, Hdraw * pt, dpinter );

		draw( im1, v1d );
		cir2d.getBB().draw( im1, DrawParams().setColor(150,200,200) );

		segd.draw( im1, DrawParams().setColor(50,50,150) );
		cir1d.draw( im1, DrawParams().setColor(250,0,0) );
		cir2d.draw( im1, DrawParams().setColor(0,0,250) );

		std::ostringstream ossa;
		ossa << "showcase6_" << std::setfill('0') << std::setw(2) << i << ".png";
		im1.write( ossa.str() );
	}
}

