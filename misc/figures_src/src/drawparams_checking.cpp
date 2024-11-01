// this file is part of homog2d
// used to build a figure that is included in manual
// see makefile target doc_fig

#include "fig_src.header"

template<typename T>
void process( img::Image<T>& im, std::string fn, int width )
{
	img::DrawParams dp;
	dp.setColor(220,220,220);
	dp.setDefault();

	int x0=30;
	int y0=40;
	int deltax=50;
	int deltay=42;
	Point2d pt( x0, y0 );

	int nbSize = 5;
	int nbTypes = (int)img::PtStyle::Dot + 1;

// grid: vertical lines
	Line2d lv(pt, Point2d(x0,nbSize*deltay));
	auto textColor = DrawParams().setColor(0,0,0);
	for( int j=0;j<nbSize; j++ )
	{
		lv.draw( im );
		lv=Homogr(deltax,0)*lv;
		int s = 3 + j*2;
		im.drawText( std::to_string(s), Point2d( pt.getX()-5, pt.getY()), textColor );
		pt.translate( deltax, 0 );
	}

// grid: horizontal lines
	pt.translate(0,deltay);
	Line2d lh(pt, Point2d(nbTypes*deltax,y0+deltay) );
	for( int i=0;i<nbTypes; i++ )
	{
		lh.draw( im );
		lh=Homogr(0,deltay)*lh;
	}

	auto color = DrawParams().setColor(0,0,250).setThickness(width);
	pt.set( x0, y0+deltay );
	for( int j=0;j<nbSize; j++ )
	{
		auto newPointStyle = img::PtStyle::Plus;
		int s = 3 + j*2;
		for( int i=0;i<nbTypes; i++ )
		{
			pt.draw( im, color.setPointStyle( newPointStyle ).setPointSize(s) );
			newPointStyle = color._dpValues.nextPointStyle();
			pt.translate(0,deltay);
		}
		pt.set( getX(pt)+deltax, y0+deltay );
	}

	pt.set( x0+nbSize*deltax, y0+deltay );
	auto newPointStyle = img::PtStyle::Plus;
	for( int i=0;i<nbTypes; i++ )
	{
		auto npts = static_cast<int>(newPointStyle)+1;
		drawText( im, getString(newPointStyle), pt, textColor );

		newPointStyle = static_cast<img::PtStyle>(npts);
		if( npts == static_cast<int>( img::PtStyle::Dot )+1 )
			newPointStyle = img::PtStyle::Plus;
		pt.translate(0,deltay);
	}

	im.write( fn );
}

/// Used to check the drawing parameters for points
int main()
{
	img::Image<SvgImage> im1( 400,350 );
	process( im1, "drawparams_1.svg", 1 );
	process( im1, "drawparams_2.svg", 2 );

	img::Image<cv::Mat> im2( 400,350 );
	process( im2, "drawparams_1.png", 1 );
	process( im2, "drawparams_2.png", 2 );
}

