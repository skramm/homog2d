// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

int main()
{
//	std::srand(time(0));
	Polyline pl(IsClosed::No);

	pl.add( 5,5);
	pl.add( 7,3);
	pl.add( 6,  2);
	pl.add( 7.5, 1.);
	pl.add( 4, 0);
	pl.add( 5.5, 2.5);
	pl.add( 4., 1.2);
	pl.add( 3., 2.2);
	pl.add( 3.6, 4.1);
	pl.add( 1.6, 5.4);
	pl = Homogr().setScale(30).addTranslation(10,30) * pl;

	img::Image<cv::Mat> img2( 300, 450 );

	pl.draw( img2, DrawParams().setColor(250,0,20) );
	pl.getBB().draw( img2, DrawParams().setColor(150,150,120) );
	cv::imwrite( "polyline1.png", img2.getReal()  );

	pl.isClosed() = true;
	img2.clear();
	pl.draw( img2, DrawParams().setColor(250,0,20) );
	pl.getBB().draw( img2, DrawParams().setColor(150,150,120) );
	cv::imwrite( "polyline2.png", img2.getReal() );
}
