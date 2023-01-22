/**
\file
\brief Generates a moving segment, its supporting line, and its two parallel segments
*/
#define HOMOG2D_USE_OPENCV
#include "../../homog2d.hpp"

using namespace h2d;

int main( int argc, const char** argv )
{
	auto nbim = 30; // nb images

	uint8_t g = 100;
	auto kx1 = 78;
	auto ky1 = 30;
	auto kx2 = 43;
	auto ky2 = 49;

	auto org_x1 = 100;
	auto org_y1 = 50;
	auto org_x2 = 120;
	auto org_y2 = 200;
	for( int i=0; i<nbim; i++ )
	{
		auto angle = i*360./nbim;
		auto angle_r1 = angle * M_PI / 180.;
		auto angle_r2 = 1.5*angle * M_PI / 180.;

		auto x1 = org_x1+std::cos(angle_r1)*kx1;
		auto y1 = org_y1+std::sin(angle_r1)*ky1;

		auto x2 = org_x2+std::cos(angle_r2)*kx2;
		auto y2 = org_y2+std::sin(angle_r2)*ky2;
		Segment s( x1, y1, x2, y2 );
		auto bis = s.getBisector();

		img::Image<cv::Mat> im( 280, 250 );
		s.getLine().draw( im, img::DrawParams().setColor(200,200,200) );
		s.draw( im, img::DrawParams().setColor(250,25,0) );
		auto psegs = s.getParallelSegs( 35 );
		draw( im, psegs.first,  img::DrawParams().setColor(150,50,0) );
		draw( im, psegs.second, img::DrawParams().setColor(50,150,0) );
		bis.draw( im, img::DrawParams().setColor(0,50,250) );
		std::ostringstream ossa;
		ossa << "showcase11_" << std::setfill('0') << std::setw(2) <<i << ".png";
		im.write( ossa.str() );
	}
}
