// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

int main()
{
	std::vector<Point2d> v1 { {20,20}, {250,20}, {230,170}, {60,190} };
	std::vector<Point2d> v2 { {60,60}, {280,50}, {220,150}, {90,130} };

	auto H = buildFrom4Points(v1, v2);
	std::cout << H;

	CPolyline p1(v1);
	CPolyline p2(v2);

	img::Image<cv::Mat> img( 300, 220 );

	p1.draw( img,  DrawParams().showPoints(true).setColor(0,200,200) );
	p2.draw( img,  DrawParams().showPoints(true).setColor(200,200,0) );

	cv::putText(
		img.getReal(),
		"from",
		v1[0].getCvPti(),
		0, 0.6,
		cv::Scalar( 20,20,0 )
	);
	cv::putText(
		img.getReal(),
		"to",
		v2[0].getCvPti(),
		0, 0.6,
		cv::Scalar( 0,20,20 )
	);

	img.write( "homog_from_pts.png" );
}
