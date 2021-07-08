#define HOMOG2D_USE_OPENCV

#include "homog2d.hpp"
#include "opencv2/highgui.hpp"

#define NUMTYPE double

using namespace homog2d;
int main()
{
	cv::Mat img;
	img.create( 350, 450, CV_8UC3 );
	std::cout << "homog2d: version " << HOMOG2D_VERSION << '\n';
