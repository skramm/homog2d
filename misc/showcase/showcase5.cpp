/**
\file
\brief Generates moving ellipse and bounding boxes
*/
#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
//#define HOMOG2D_NOCHECKS
#include "../../homog2d.hpp"

using namespace h2d;

int main( int argc, const char** argv )
{
	auto nbim = 30; // nb images
	auto Hdraw = Homogr().setScale(20).addTranslation(10,20);

	uint8_t g = 100;
	auto kx=2.5;
	auto ky=0.9;
	auto x0 = 4.8;
	auto y0 = 3.5;

	for( int i=0; i<nbim; i++ )
	{
		auto angle = i*360./nbim;
		auto angle_r = angle * M_PI / 180.;


		auto x1 = x0+std::cos(angle_r)*kx;
		auto y1 = y0+std::sin(angle_r)*ky;
		auto ell = Ellipse( x1, y1, 2.5, 1., angle_r ) ;

//		std::cout << i << ": angle=" << angle << " ell=" << ell << "\n";

		auto ell2 = Hdraw * ell;

		img::Image<cv::Mat> im( 250, 200 );
		ell2.getCenter().draw( im, img::DrawParams().setColor(0,250,0) );
		auto lines = ell2.getAxisLines();
		lines.first.draw( im );
		lines.second.draw( im );

		getBB(ell2).draw(  im, img::DrawParams().setColor(0,250,0) );
		getOBB(ell2).draw( im, img::DrawParams().setColor(0,0,250) );
		ell2.draw( im, img::DrawParams().setColor(250,0,0) );
		std::ostringstream ossa;
		ossa << "showcase5_" << std::setfill('0') << std::setw(2) <<i << ".png";
		im.write( ossa.str() );
	}
}
