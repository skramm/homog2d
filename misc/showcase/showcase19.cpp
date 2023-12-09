/**
\file
\brief Generates random segments
*/

#define HOMOG2D_USE_OPENCV
#include "../../homog2d.hpp"

using namespace h2d;
using namespace h2d::img;

auto im_h = 300;
auto im_w = 400;

std::vector<Segment>
generateSegments( int nbSegs )
{
	int delta  = 40;
	int width  = im_w-delta;
	int height = im_h-delta;

	std::vector<Segment> vseg;
	for( auto i=0; i<nbSegs; i++ )
	{
		auto len = 1.0*rand() / RAND_MAX * 40 + 10;
		auto p1x = 1.0*rand() / RAND_MAX * width + 20;
		auto p2x = 1.0*rand() / RAND_MAX * width + 20;;
		auto p1y = 1.0*rand() / RAND_MAX * height + 20;
		auto p2y = 1.0*rand() / RAND_MAX * height + 20;
		auto line = Line2d( p1x, p1y, p2x, p2y );
		auto ppts = line.getPoints( Point2d( p1x, p1y) , len );
		vseg.push_back( Segment( ppts ) );
	}
	return vseg;
}

int main( int argc, const char** argv )
{
	auto nbim  = 5; // nb images
	int nbSegs = 40;
	auto vcol  = img::genRandomColors( nbSegs );

	auto func = [&](int i)   // lambda, needed to fetch color from index
	{
		return img::DrawParams().setColor(vcol[i]).showPoints();
	};
	std::function<img::DrawParams(int)> f(func);

	for( int i=0; i<nbim; i++ )
	{
		auto vseg = generateSegments( nbSegs );
		Image<cv::Mat> im( im_w, im_h );
		draw( im, vseg, f );

		std::ostringstream ossa;
		ossa << "showcase19_" << std::setfill('0') << std::setw(2) << i << ".png";
		im.write( ossa.str() );
	}
}

