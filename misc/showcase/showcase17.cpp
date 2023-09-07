/**
\file
\brief Generates a moving segment and its orthogonal segments and points
*/
#define HOMOG2D_USE_OPENCV
#include "../../homog2d.hpp"

using namespace h2d;

int main( int argc, const char** argv )
{
	auto nbim = 30; // nb images
	auto k = 60;
	auto x1 = 120.;
	auto y1 = 120.;
	auto org_x2 = 130;
	auto org_y2 = 130;

	std::array<img::Color,4> vcol;
	vcol[0]=img::Color(0,250,125);
	vcol[1]=img::Color(0,125,250);
	vcol[2]=img::Color(125,0,250);
	vcol[3]=img::Color(250,0,125);

	auto fl = [&](int i)   // lambda
	{
		return img::DrawParams().setPointStyle(img::PtStyle::Dot).setColor( vcol[i] );
	};
	std::function<img::DrawParams(int)> style(fl);

	for( int i=0; i<nbim; i++ )
	{
		auto angle = i*360./nbim;
		auto angle_r2 = 1.0*angle * M_PI / 180.;

		auto x2 = org_x2+std::cos(angle_r2)*k;
		auto y2 = org_y2+std::sin(angle_r2)*k;
		Segment s( x1, y1, x2, y2 );

		img::Image<cv::Mat> im( 280, 250 );
		s.draw( im, img::DrawParams().setColor(250,25,0).setThickness(2) );
		auto osegs = s.getOrthogSegs();
		auto opts = s.getOrthogPts();
		if( i<nbim/2 )
			draw( im, osegs, style );
		else
			draw( im, opts, style );

		std::ostringstream ossa;
		ossa << "showcase17_" << std::setfill('0') << std::setw(2) <<i << ".png";
		im.write( ossa.str() );
	}
}
