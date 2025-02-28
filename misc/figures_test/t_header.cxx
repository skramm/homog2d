// file t_header.cxx
// used in makefile target `test-fig`

//#define HOMOG2D_DEBUGMODE

#include "../../homog2d.hpp"

#define NUMTYPE double

using namespace h2d;

template<typename IMG,typename HOMOG>
void drawGrid( IMG& img, const HOMOG& H )
{
	int g=220, g0=g/2;
	auto colgrid  = img::DrawParams().setColor(g,g,g);
	auto colgrid0 = img::DrawParams().setColor(g0,g0,g0).setThickness(2);
	for (int i=0; i<6; i++ )
	{
		Line2d li( LineDir::H, i );
		auto li2 = H*li;
		if( i==0 )
			li2.draw( img, colgrid0 );
		else
			li2.draw( img, colgrid );
	}
	for (int i=0; i<7; i++ )
	{
		Line2d li( LineDir::V, i );
		auto li2 = H*li;
		if( i==0 )
			li2.draw( img, colgrid0 );
		else
			li2.draw( img, colgrid );
	}
}

int main( int argc, const char** argv )
{
	img::Image<img::SvgImage> img2(350,400);

//#ifdef HOMOG2D_DEBUGMODE
//	std::cout << "file: " << __FILE__ << '\n';
//#endif
