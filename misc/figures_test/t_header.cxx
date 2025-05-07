// file t_header.cxx
// used in makefile target `test-fig`

#define HOMOG2D_DEBUGMODE

#include "../../../homog2d.hpp"

#define NUMTYPE double

#include <regex>

using namespace h2d;
using namespace std;


template<typename PTYPE,typename HO>
void
processSource( const PTYPE& poly, const Line2d&, const HO&, std::string fname, std::string OC );

// forward declaration, only for polyline splitting
template<typename POLY,typename VVPTS>
void processAll( const POLY&, std::string fname, std::string OC, const VVPTS&, const Line2d&, const Homogr&, std::ofstream& );

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
		{
			li2.draw( img, colgrid );
			drawText( img, std::to_string(i), Point2d(5,50*i+30) );
		}
	}
	for (int i=0; i<7; i++ )
	{
		Line2d li( LineDir::V, i );
		auto li2 = H*li;
		if( i==0 )
			li2.draw( img, colgrid0 );
		else
		{
			li2.draw( img, colgrid );
			drawText( img, std::to_string(i), Point2d(50*i+32,15) );
		}
	}
}

int main( int argc, const char** argv )
{
	img::Image<img::SvgImage> img2(400,400);

//#ifdef HOMOG2D_DEBUGMODE
//	std::cout << "file: " << __FILE__ << '\n';
//#endif
