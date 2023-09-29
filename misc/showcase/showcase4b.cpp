/**
\file
\brief Generates rotating rectangle and its bounding circle
*/
#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#include "../../homog2d.hpp"

using namespace h2d;

int main( int argc, const char** argv )
{
	auto nbim = 25; // nb images
	auto Hdraw = Homogr().setScale(30).addTranslation(10,10);

	auto r_w0 = 2;
	auto r_h0 = 1;
	auto x0 = 3.;
	auto y0 = 1.;
	auto k=1.8;

	for( int i=0; i<nbim; i++ )
	{
		auto angle = i*360./nbim;
		auto angle_r = angle * M_PI / 180.;
		auto x1 = std::cos(angle_r)*k;
		auto y1 = std::sin(angle_r)*k;
		auto r_w = r_w0 + std::sin( 2.*M_PI*i/nbim );

		FRect rect( Point2d( x1+x0, y1+x0), r_w, r_h0 );

		auto cir_b = rect.getBoundingCircle();
		auto cir_i = rect.getInscribedCircle();

		img::Image<cv::Mat> imb( 250, 200 );

		(Hdraw * cir_b).draw( imb, img::DrawParams().setColor(50,250,50)  );
		(Hdraw * cir_i).draw( imb, img::DrawParams().setColor(50,25,250)  );
		(Hdraw * rect).draw( imb,  img::DrawParams().setColor(250,25,25)  );

		std::ostringstream ossb;
		ossb << "showcase4b_" << std::setfill('0') << std::setw(2) <<i << ".png";
		imb.write( ossb.str() );
	}
}

