// file t_footer_polyline.cxx
// used in makefile target test-fig

	Homogr H;
	H.addScale( 50 );
	H.addTranslation( 20,20 );
	drawGrid( img2, H );

	auto poly1 = H*pol;
	auto dp = img::DrawParams().setColor(250,0,20).showPoints(true).setPointStyle(img::PtStyle::Times).showIndex(true);
	poly1.draw( img2, dp );

	auto li2 = H*li;
	li2.draw( img2, dp.setColor( 0,0,250) );

	img2.write( std::string(argv[0])+ ".svg" );
}

