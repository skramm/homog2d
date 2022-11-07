/**
\file
\brief demo of full steps rotating of a Polyline (2022-11-06: WIP !!)
*/
#define HOMOG2D_USE_OPENCV
//#define HOMOG2D_DEBUGMODE
#include "../../homog2d.hpp"

using namespace h2d;

img::Image<cv::Mat> im( 300, 400 );

void processRot(
	CPolyline&  pl,      ///< Polyline
	std::string msg,
	Rotate      rot,     ///< What kind of rotation?
	int         nb_img,  ///< How many images
	int         pt_idx   ///< index of Polyline point that will be the center point
)
{
	static int out_idx;
	for( int i=0; i<nb_img; i++ )
	{
		im.clear();
		cv::putText(
			im.getReal(),
			std::to_string(out_idx) + ": " + msg + ": " + std::to_string(i+1) + "/" + std::to_string(nb_img),
			cv::Point2i( 20, 40 ), 0, 0.6, cv::Scalar( 50,0,0 ), 1
		);

		pl.rotate( rot, pl.getPoint(pt_idx) );
		pl.getPoint(0).draw( im, img::DrawParams().setColor(250,120,20).setPointStyle(img::PtStyle::Diam) );
		draw( im, Line2d( LineDir::H, pl.getPoint(0) ), img::DrawParams().setColor(200,200,200) );
		draw( im, Line2d( LineDir::V, pl.getPoint(0) ), img::DrawParams().setColor(200,200,200) );
		pl.draw( im, img::DrawParams().setColor(250,0,20) );
//		pl.getBB().draw( im, img::DrawParams().setColor(150,150,120) );

		std::ostringstream oss;
		oss << "showcase12_" << std::setfill('0') << std::setw(2) << out_idx++ << ".png";
		im.write( oss.str() );
	}
}

int main( int argc, const char** argv )
{
	CPolyline pl;
	std::vector<Point2d> vpts{
		{ 5,5 },
		{ 7,3 },
		{ 6,  2 },
		{ 7.5, 1. },
		{ 4, 0 },
		{ 5.5, 2.5 },
		{ 4., 1.2 },
		{ 3., 2.2 },
		{ 3.6, 4.1 },
		{ 1.6, 5.4 }
	};
	pl.set( vpts );

	auto Hdraw = Homogr().setScale(30).addTranslation(10,30);

	int nbpts = 3;
	auto pl2 = Hdraw * pl;
	processRot( pl2, "VMirror",    Rotate::VMirror, nbpts, 0 );
	processRot( pl2, "HMirror",    Rotate::HMirror, nbpts, 0 );
	processRot( pl2, "ClockWise",  Rotate::CW,      5, 0 );
	processRot( pl2, "CClockWise", Rotate::CCW,     5, 0 );
	processRot( pl2, "Full",       Rotate::Full,    nbpts, 0 );
}

