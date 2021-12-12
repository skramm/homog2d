/**
\file
\brief Generates two moving recangles
*/
#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#include "../../homog2d.hpp"

#include "opencv2/highgui.hpp"

using namespace h2d;
using namespace h2d::img;

int main( int argc, const char** argv )
{
	auto nbim = 25; // nb images


	auto Hdraw = Homogr().setScale(30).addTranslation(10,30);


	auto r_w0 = 2;
	auto r_h0 = 1;
	auto x0 = 3.;
	auto y0 = 1.;
	auto k=1.8;
	uint8_t g = 100;
	for( int i=0; i<nbim; i++ )
	{
		auto angle = i*360./nbim;
		auto angle_r = angle * M_PI / 180.;
		auto x1 = std::cos(angle_r)*k;
		auto y1 = std::sin(angle_r)*k;
		auto r_w = r_w0 + std::sin( 2.*M_PI*i/nbim );
//		std::cout << i << ": r_w=" << r_w << "\n";
		FRect rect1( Point2d( x1+x0, y1+x0), r_w, r_h0 );
		FRect rect2( Point2d(-x1+x0,-y1+x0), r_w, r_h0 );
		auto rect1_d = Hdraw * rect1;
		auto rect2_d = Hdraw * rect2;

		img::Image<cv::Mat> im( 250, 200 );
		rect1_d.draw( im, DrawParams().setColor(250,0,0) );
		rect2_d.draw( im, DrawParams().setColor(0,0,250) );
		auto bb = getBB(rect1_d, rect2_d);
		bb.draw( im, DrawParams().setColor(g,g,g) );
		std::ostringstream ossa;
		ossa << "BUILD/showcase4_" << std::setfill('0') << std::setw(2) <<i << ".png";
		cv::imwrite( ossa.str(), im.getReal() );
	}
}

