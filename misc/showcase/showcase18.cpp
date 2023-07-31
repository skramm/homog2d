/**
\file
\brief Generates a parallelogram from 2 fixed point and a rotating one.
*/

#define HOMOG2D_USE_OPENCV
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
	auto y0 = 2.;
	auto k=1.8;

	Point2d p1( 1, 4 );
	Point2d p2( 3, 5 );

	for( int i=0; i<nbim; i++ )
	{
		auto angle = i*360./nbim;
		auto angle_r = angle * M_PI / 180.;
		auto x1 = std::cos(angle_r)*k;
		auto y1 = std::sin(angle_r)*k;
//		auto r_w = r_w0 + std::sin( 2.*M_PI*i/nbim );

		Point2d p0( x1+x0, y1+y0);
		CPolyline pol1, pol2, pol3;
		pol1.setParallelogram( p1, p2, p0 );
		pol2.setParallelogram( p1, p0, p2 );
//		pol3.setParallelogram( p2, p1, p0 );

		Image<cv::Mat> im1( 250, 200 );
		(Hdraw * pol1).draw( im1, DrawParams().setPointStyle( PtStyle::Diam ).setColor( 250,25,20 ) );
		(Hdraw * pol2).draw( im1, DrawParams().setPointStyle( PtStyle::Diam ).setColor( 25,250,20 ) );
//		(Hdraw * pol3).draw( im1, DrawParams().setPointStyle( PtStyle::Diam ).setColor( 25,25,250 ) );

		auto drawparams = DrawParams().setPointStyle( PtStyle::Diam ).setColor( 20,0,250 );
		(Hdraw * p0).draw(im1, drawparams );
		(Hdraw * p1).draw(im1, drawparams );
		(Hdraw * p2).draw(im1, drawparams );

		std::ostringstream ossa;
		ossa << "showcase18_" << std::setfill('0') << std::setw(2) << i << ".png";
		im1.write( ossa.str() );
	}
}

