// file t_footer_polyline.cxx
// used in makefile target test-fig

	OPolyline opsrc( src );

	Homogr H;
	H.addScale( 50 );
	H.addTranslation( 40,20 );
	drawGrid( img2, H );

	auto dp = img::DrawParams().setThickness(3);

	auto li2 = H*li;
	li2.draw( img2, dp.setColor(0,250,0) );

	auto poly1 = H*opsrc;
	poly1.draw( img2, dp.setColor(250,0,0) );
	img2.write( std::string(argv[0])+ ".svg" );

	img2.clear();
	drawGrid( img2, H );
	dp.showPoints(true).setPointSize(7);
	drawSplit( img2, CPolyline(), vv_pts_C, H, dp, std::string(argv[0]) + "_C.svg"  );
	drawSplit( img2, OPolyline(), vv_pts_O, H, dp, std::string(argv[0]) + "_O.svg"  );
}

template<typename IMG,typename TYPE,typename VVPTS>
void drawSplit( IMG& img, TYPE, const VVPTS& vv_pts, const Homogr& H, img::DrawParams dp, string fname )
{
	for( size_t i=0;i<vv_pts.size(); i++ )
	{
		const auto& v_pts = vv_pts[i];
		auto polsp = TYPE(v_pts);
		std::cout << "******** POLYLINE TYPE=" << polsp.isClosed() << " **********\n";

		if( i%2 )
			dp.setColor(250,0,0).setPointStyle(img::PtStyle::Diam);
		else
			dp.setColor(0,0,250).setPointStyle(img::PtStyle::Times);
		(H*polsp).draw( img, dp );
	}
	img.write( fname );
}
