// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

/// Used to check the drawing parameters for points
/// \todo issue here, the size is not used for type PtStyle::Dot
int main()
{
	img::DrawParams dp;
//	dp.setThickness(1).showPoints().showIndex();
	dp.setThickness(1).showPoints();
	dp.setDefault();

	img::Image<SvgImage> im1( 400,300 );
	img::Image<cv::Mat>  im2( 400,300 );


	dp.setColor( 250,0,0);
	img::PtStyle newPointStyle = img::PtStyle::Plus;

	int x0=30;
	int y0=40;
	int deltax=25;
	int deltay=30;
	Point2d pt( x0, 30 );

	for( int j=0;j<5; j++ )
	{
		pt.set( x0, y0 );
		int s = 3 + j*2;
		im2.drawText( std::to_string(s), pt, DrawParams() ); /// \todo check why default arg not implemented !!
		pt.translate(deltax,0 );
		for( int i=0;i<=(int)img::PtStyle::Dot; i++ )
		{
//			std::cout << i << " color=" << dp.color() << " pt=" << pt << '\n';
			pt.draw( im1, dp.setPointStyle( newPointStyle ).setPointSize(s) );
			pt.draw( im2, dp.setPointStyle( newPointStyle ).setPointSize(s) );
			newPointStyle = dp._dpValues.nextPointStyle();
			pt.translate(0,deltay);
		}
		pt.translate( deltax, 0 );

	}
	im1.write( "drawparams_1.svg" );
	im2.write( "drawparams_2.png" );
}
