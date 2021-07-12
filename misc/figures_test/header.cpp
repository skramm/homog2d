#define HOMOG2D_USE_OPENCV

#include "../../homog2d.hpp"
#include "opencv2/highgui.hpp"

#define NUMTYPE double

using namespace h2d;
int main( int argc, const char** argv )
{
	cv::Mat img;
	img.create( 200, 250, CV_8UC3 );
//	std::cout << "homog2d: version " << HOMOG2D_VERSION << '\n';
//	std::cout << "file: " << argv[0] << '\n';
