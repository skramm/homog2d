// file t_footer_polyline.cxx
// used in makefile target test-fig

	OPolyline psrc_o( src );
	CPolyline psrc_c( src );

	Homogr H;
	H.addScale( 50 );
	H.addTranslation( 40,20 );
//	drawGrid( img2, H );

	auto dp = img::DrawParams().setThickness(3);

	auto poly_o = H*psrc_o;
	auto poly_c = H*psrc_c;

	img::Image<img::SvgImage> im_src_o(400,400);
	img::Image<img::SvgImage> im_src_c(400,400);

	drawGrid( im_src_o, H );
	drawGrid( im_src_c, H );

	poly_o.draw( im_src_o, dp.setColor(250,0,0) );
	poly_c.draw( im_src_c, dp.setColor(250,0,0) );

	auto li2 = H*li;
	li2.draw( im_src_o, dp.setColor(0,250,0).setThickness(1) );
	li2.draw( im_src_c, dp.setColor(0,250,0).setThickness(1) );

	im_src_o.write( std::string(argv[0])+ "_O.svg" );
	im_src_c.write( std::string(argv[0])+ "_C.svg" );

	dp.showPoints(true).setPointSize(7);
	drawSplit( OPolyline(), vv_pts_O, H, dp, std::string(argv[0]), "_O" );
	drawSplit( CPolyline(), vv_pts_O, H, dp, std::string(argv[0]), "_O" );
	drawSplit( OPolyline(), vv_pts_C, H, dp, std::string(argv[0]), "_C" );
	drawSplit( CPolyline(), vv_pts_C, H, dp, std::string(argv[0]), "_C" );
}

template<typename TYPE,typename VVPTS>
void drawSplit( TYPE, const VVPTS& vv_pts, const Homogr& H, img::DrawParams dp, string fname, string t )
{
	for( size_t i=0;i<vv_pts.size(); i++ )
	{
		img::Image<img::SvgImage> im(400,400);
		drawGrid( im, H );
		const auto& v_pts = vv_pts[i];
		auto polsp = TYPE(v_pts);
		std::cout << "******** POLYLINE TYPE=" << polsp.isClosed() << " **********\n";

		if( i%2 )
			dp.setColor(250,0,0).setPointStyle(img::PtStyle::Diam);
		else
			dp.setColor(0,0,250).setPointStyle(img::PtStyle::Times);
		(H*polsp).draw( im, dp );
		char t2 = (polsp.isClosed() ? 'c' : 'o');
		im.write( fname+t+t2+'_'+std::to_string(i)+".svg" );
	}
}
