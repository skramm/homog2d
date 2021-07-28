#include "fig_src.header"

int main()
{
	std::srand(time(0));
	Polyline pl(IsClosed::Yes);
/*	for( int i=0; i<7; i++ )
		pl.add(
			400.*rand()/RAND_MAX+20,
			300.*rand()/RAND_MAX+20
		);
*/

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

	cv::Mat img;
	img.create( 250, 300, CV_8UC3 );
	img = cv::Scalar(255,255,255);
	img::Image<cv::Mat> img2( img );

	pl.draw( img2, DrawParams().setColor(250,0,20) );

	cv::imwrite( "polyline1.png", img );
}
