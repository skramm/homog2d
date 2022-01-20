// file t_header.cxx
// used in makefile target `test_fig`

#define HOMOG2D_USE_OPENCV
#define HOMOG2D_DEBUGMODE

#include "../../homog2d.hpp"

#define NUMTYPE double

using namespace h2d;
int main( int argc, const char** argv )
{
	img::Image<cv::Mat> img2(350,400);

//#ifdef HOMOG2D_DEBUGMODE
//	std::cout << "file: " << __FILE__ << '\n';
//#endif
