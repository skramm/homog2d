/**
\file
\brief Generates two moving objects and the common bounding box
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
	for( int i=0; i<nbim; i++ )
	{
		auto angle = i * 2. * M_PI / nbim;
		auto x1 = std::cos(angle)*k;
		auto y1 = std::sin(angle)*k;
		auto r_w = r_w0 + std::sin(angle);

		FRect obj1( Point2d( x1+x0, y1+x0), r_w, r_h0 );
		Circle obj2( Point2d(-x1+x0,-y1+x0), 2.-.5*r_w );
		auto bcir = obj1.getBoundingCircle();
		auto cbb = getBB(obj2);
		auto center = obj1.getCenter();

		auto obj1_d = Hdraw * obj1;
		auto obj2_d = Hdraw * obj2;
		auto bcir_d = Hdraw * bcir;
		auto center_d = Hdraw * center;
		auto cbb_d  = Hdraw * cbb;

		Image<cv::Mat> ima( 250, 200 );

		obj1_d.draw( ima, DrawParams().setColor(250,0,0) );
		obj2_d.draw( ima, DrawParams().setColor(0,0,250) );
		bcir_d.draw( ima, DrawParams().setColor(100,250,100)  );
		obj2_d.getBB().draw( ima, DrawParams().setColor(100,50,200)  );

		getBB(obj1_d, obj2_d).draw( ima, DrawParams().setColor(g,g,g) );

		std::ostringstream ossa;
		ossa << "showcase4a_" << std::setfill('0') << std::setw(2) <<i << ".png";
		ima.write( ossa.str() );
	}
}

