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
	const CPolyline& pl_src,      ///< Polyline
	std::string msg,
	Rotate     rot,     ///< What kind of rotation?
	int        nb_img,  ///< How many images
	int        pt_idx   ///< index of Polyline point that will be the center point
)
{
	static int out_idx;
	for( int i=0; i<nb_img; i++ )
	{
		im.clear();
		cv::putText( im.getReal(), msg, cv::Point2i( 20, 40 ), 0, 0.6, cv::Scalar( 50,0,0 ), 1 );

		pl_src.getPoint(0).draw( im, img::DrawParams().setColor(250,120,20).setPointStyle(img::PtStyle::Diam) );
		pl_src.draw( im, img::DrawParams().setColor(20,20,250) );
//		pl.getBB().draw( im, img::DrawParams().setColor(150,150,120) );

		std::ostringstream ossa;
		ossa << "showcase12_" << std::setfill('0') << std::setw(2) << out_idx++ << ".png";
		im.write( ossa.str() );

		CPolyline pl2 = pl_src;
		pl2.rotate( rot, pl2.getPoint(pt_idx) );
		im.clear();
		cv::putText( im.getReal(), msg, cv::Point2i( 20, 40 ), 0, 0.6, cv::Scalar( 50,0,0 ), 1 );
		pl2.getPoint(0).draw( im, img::DrawParams().setColor(250,120,20).setPointStyle(img::PtStyle::Diam) );
		pl2.draw( im, img::DrawParams().setColor(250,0,20) );
//		pl.getBB().draw( im, img::DrawParams().setColor(150,150,120) );

		std::ostringstream ossb;
		ossb << "showcase12_" << std::setfill('0') << std::setw(2) << out_idx++ << ".png";
		im.write( ossb.str() );
	}
}


int main( int argc, const char** argv )
{
//	auto n = 25; // nb images
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
	processRot( pl2, "ClockWise",  Rotate::CW,      nbpts, 0 );
	processRot( pl2, "CClockWise", Rotate::CCW,     nbpts, 0 );
	processRot( pl2, "Full",       Rotate::Full,    nbpts, 0 );
}

