/**
\file
\brief "Spirograph demo": Generates two rotating points at different periods and the segments between them
*/

#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#include "../../homog2d.hpp"

using namespace h2d;
using namespace h2d::img;

/// Arguments:
/**
-# angle step in degrees for outer point
-# scale factor for inner radius
-# inner period coeff:
  - >1 => inner point will be faster than outer point
  - <1 => inner point will be slower than outer point
*/
int main( int argc, const char** argv )
{
	auto angle_step = 15.0;
	if( argc > 1 )
		angle_step = std::stod( argv[1] );
	std::cout << argv[0] << ": \n - angle_step=" << angle_step << '\n';

	auto outer_radius = 17.;
	auto inner_radius_scale = 7. / outer_radius;
	if( argc > 2 )
		inner_radius_scale = std::stod( argv[2] );
	auto inner_radius = outer_radius * inner_radius_scale;
	std::cout << " - inner_radius=" << inner_radius
		<< ", outer_radius=" << outer_radius << '\n';

	auto inner_period_coeff = 1.4;
	if( argc > 3 )
		inner_period_coeff = std::stod( argv[3] );
	std::cout << " - inner_period_coeff=" << inner_period_coeff << '\n';

	auto im_size = 500;
	auto nbim = 300; // nb images
	auto drawscale = 12.;
	auto Hdraw = Homogr().addScale(drawscale,drawscale).addTranslation(im_size/2.,im_size/2.);
	auto pt0d =  Hdraw * Point2d();
	std::vector<Segment> vecseg;

	auto r1=0.7;  // radius of the two points
	auto r2=0.5;
	for( int i=0; i<nbim; i++ )
	{
		auto angle = angle_step*i; // step in degrees between each image
		auto angle_1 = inner_period_coeff * angle * M_PI / 180.;
		auto angle_2 = angle * M_PI / 180.;
		auto x1 = std::cos(angle_1) * inner_radius;
		auto y1 = std::sin(angle_1) * inner_radius;
		auto x2 = std::cos(angle_2) * outer_radius;
		auto y2 = std::sin(angle_2) * outer_radius;

		Circle cir1( x1,y1,r1 );
		Circle cir2( x2,y2,r2 );

		auto cir1d = Hdraw * cir1;
		auto cir2d = Hdraw * cir2;

		Image<cv::Mat> im( im_size, im_size );

		pt0d.draw( im, DrawParams().setColor(50,0,50) );

		Circle( pt0d, inner_radius*drawscale ).draw(im);
		Circle( pt0d, outer_radius*drawscale ).draw(im);
		auto seg_style = DrawParams().setColor(50,20,0).setThickness(2);

		auto current = getSegment( cir1d, cir2d );
		vecseg.push_back( current );
		draw( im, vecseg, DrawParams().setColor(150,200,0) );

		current.draw( im, seg_style );
		Segment( cir1d.getCenter(), pt0d ).draw( im, seg_style );

		cir1d.draw( im, DrawParams().setColor(250,0,0) );
		cir2d.draw( im, DrawParams().setColor(0,0,250) );

		std::ostringstream ossa;
		ossa << "showcase_v_01_" << std::setfill('0') << std::setw(3) << i << ".png";
		im.write( ossa.str() );
	}
}

