// file t_header.cxx
// used in makefile target `test_fig`

//#define HOMOG2D_DEBUGMODE

// source files get build in BUILD/figures_test/src/, thus this path
#include "../../../homog2d.hpp"

#define NUMTYPE double

using namespace h2d;
int main( int, const char** argv )
{
	img::Image<img::SvgImage> img2(350,400);

#ifdef HOMOG2D_DEBUGMODE
	std::cout << "file: " << __FILE__ << '\n';
#endif
