/**
\file
\brief Generates two rotating points at different periods and the segments between them
*/

#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#include "../../homog2d.hpp"

using namespace h2d;
using namespace h2d::img;

int main( int argc, const char** argv )
{
	auto im_size = 400;
	auto nbim = 300; // nb images
	auto drawscale = 12.;
	auto Hdraw = Homogr().addScale(drawscale,drawscale).addTranslation(im_size/2.,im_size/2.);
	auto pt0d =  Hdraw * Point2d();
	std::vector<Segment> vecseg;

	auto k1=7; // radius of the orbits
	auto k2=13;

	auto a1=1.5;
	auto a2=1.;

	auto r1=0.7;  // radius of the two points
	auto r2=0.5;
	for( int i=0; i<nbim; i++ )
	{
		auto angle = 21.0*i; // step in degrees between each image
		auto angle_1 = a1 * angle * M_PI / 180.;
		auto angle_2 = a2 * angle * M_PI / 180.;
		auto x1 = std::cos(angle_1)*k1;
		auto y1 = std::sin(angle_1)*k1;
		auto x2 = std::cos(angle_2)*k2;
		auto y2 = std::sin(angle_2)*k2;

		Circle cir1( x1,y1,r1 );
		Circle cir2( x2,y2,r2 );

		auto cir1d = Hdraw * cir1;
		auto cir2d = Hdraw * cir2;

		Image<cv::Mat> im( im_size, im_size );

		pt0d.draw( im, DrawParams().setColor(50,0,50) );

		Circle( pt0d, k1*drawscale ).draw(im);
		Circle( pt0d, k2*drawscale ).draw(im);
		Segment( cir1d.getCenter(), pt0d ).draw( im, DrawParams().setColor(150,0,150) );

		vecseg.push_back( getSegment( cir1d, cir2d ) );
		draw( im, vecseg, DrawParams().setColor(150,200,0) );

		cir1d.draw( im, DrawParams().setColor(250,0,0) );
		cir2d.draw( im, DrawParams().setColor(0,0,250) );

		std::ostringstream ossa;
		ossa << "showcase13_" << std::setfill('0') << std::setw(2) << i << ".png";
		im.write( ossa.str() );
	}
}

