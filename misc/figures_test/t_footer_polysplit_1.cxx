// file t_footer_polyline.cxx
// used in makefile target test-fig

	OPolyline psrc_o( src );
	CPolyline psrc_c( src );

	std::ofstream fhtml_O{ "polysplit_O.html", std::ios::app };
	std::ofstream fhtml_C{ "polysplit_C.html", std::ios::app };

	Homogr H;
	H.addScale( 50 );
	H.addTranslation( 40,20 );

	auto dp = img::DrawParams().setThickness(3);

	auto poly_o = H*psrc_o;
	auto poly_c = H*psrc_c;

	img::Image<img::SvgImage> im_src_o(400,300);
	img::Image<img::SvgImage> im_src_c(400,300);

	drawGrid( im_src_o, H );
	drawGrid( im_src_c, H );

	poly_o.draw( im_src_o, dp.setColor(250,0,0) );
	poly_c.draw( im_src_c, dp.setColor(250,0,0) );


	auto li2 = H*li;
	li2.draw( im_src_o, dp.setColor(0,250,0).setThickness(1) );
	li2.draw( im_src_c, dp.setColor(0,250,0).setThickness(1) );

	dp.setColor(250,0,0).showPoints(true).setPointSize(7).setThickness(2);

	auto fname = std::regex_replace( std::string(argv[0]), std::regex("/bin/"), "/out/" );

	im_src_o.write( fname+"_O.svg" );
	im_src_c.write( fname+"_C.svg" );

	processAll( poly_c, fname, "C", vv_pts_C, li2, H, dp, fhtml_C );
	processAll( poly_o, fname, "O", vv_pts_O, li2, H, dp, fhtml_O );
}


template<typename TYPE,typename VVPTS>
void drawExpectedSplit( TYPE, const VVPTS& vv_pts, const Homogr& H, img::DrawParams dp, string fname, string t, std::ofstream& fhtml )
{
	for( size_t i=0;i<vv_pts.size(); i++ )
	{
		img::Image<img::SvgImage> im(400,300);
		drawGrid( im, H );
		const auto& v_pts = vv_pts[i];
		auto polsp = TYPE(v_pts);
		(H*polsp).draw( im, dp );
		char t2 = (polsp.isClosed() ? 'c' : 'o');

		auto fn1 = fname+t+t2+'_'+std::to_string(i)+".svg";
		im.write( fn1 );
		fhtml << "<img src='"<< fn1 << "'>\n";
	}
}


template<typename POLYSET,typename DP>
void
drawReal( const POLYSET& pset, const Homogr& H, DP dp, std::string fname, std::ofstream& fhtml )
{
	for( size_t i=0;i<pset.size(); i++ )
	{
		img::Image<img::SvgImage> im(400,300);
		drawGrid( im, H );
		const auto& poly = pset[i];
		poly.draw( im, dp );
		auto fn = fname+std::to_string(i)+".svg";
		std::cout << "drawSplit(): name=" << fn << '\n';
		im.write( fn );
		fhtml << "<img src='"<< fn << "'>\n";
	}
}

template<typename TYPE,typename POLY,typename DP>
void drawRealSplit( TYPE, const POLY& poly, const Line2d& li, const Homogr& H, DP dp, std::string fname, std::string suffix, std::ofstream& fhtml )
{
	auto dummy = TYPE();
	if( dummy.isClosed() )
		drawReal( poly.splitC( li ), H, dp, fname + suffix, fhtml );
	else
		drawReal( poly.splitO( li ), H, dp, fname + suffix, fhtml );
}

template<typename POLY,typename VVPTS,typename DP>
void
processAll(
	const POLY&    poly,   ///< the polyline we want to split (open or closed)
	std::string    name,   ///< source filename
	std::string    OC,     ///< "O" or "C" (source polyline type)
	const VVPTS&   vv_pts, ///< expected set of points
	const Line2d&  li,     ///< line used for the split
	const Homogr&  H,      ///< Homography, needed to draw the grid
	DP             dp,     ///< Drawing parameters
	std::ofstream& fhtml   ///< output HTML file
)
{
	auto fn = name + "_" + OC;
	fhtml<< "<h2>Source polyline: " << fn << "</h2>\n"
		<< "<img src='" << fn+".svg" << "'><br>\n"
		<< "<h3>1/2 - Expected Split</h3>\n";

	fhtml << "<h4>Output: Open</h4>\n";
	drawExpectedSplit( OPolyline(), vv_pts, H, dp, name, "_exp_"+OC, fhtml );
	fhtml << "<h4>Output: Closed</h4>\n";
	drawExpectedSplit( CPolyline(), vv_pts, H, dp, name, "_exp_"+OC, fhtml );
	fhtml << "<hr>\n";

	fhtml << "<h3>2/2 - Generated Split</h3>\n";
	fhtml << "<h4>Output: Open</h4>\n";
	drawRealSplit( OPolyline(), poly, li, H, dp, name, "_real_"+OC+"_o", fhtml );
	fhtml << "<h4>Output: Closed</h4>\n";
	drawRealSplit( CPolyline(), poly, li, H, dp, name, "_real_"+OC+"_c", fhtml );
	fhtml << "<hr>\n";
}


