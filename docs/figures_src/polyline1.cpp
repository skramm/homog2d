// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

int main()
{
//	std::srand(time(0));
	std::vector<Point2d> vpts{
		{5,5},
		{7,3},
		{6,2},
		{7.5, 1.},
		{4, 0},
		{5.5, 2.5},
		{4., 1.2},
		{3., 2.2},
		{3.6, 4.1},
		{1.6, 5.4}
	};
	OPolyline opl(vpts);

	opl = Homogr().setScale(30).addTranslation(10,30) * opl;

	img::Image<cv::Mat> img2( 300, 450 );

	opl.draw( img2, DrawParams().setColor(250,0,20) );
	opl.getBB().draw( img2, DrawParams().setColor(150,150,120) );
	cv::imwrite( "polyline1a.png", img2.getReal()  );

	CPolyline cpl(opl);

	img2.clear();
	cpl.draw( img2, DrawParams().setColor(250,0,20) );
	cpl.getBB().draw( img2, DrawParams().setColor(150,150,120) );
	cv::imwrite( "polyline1b.png", img2.getReal() );
}
