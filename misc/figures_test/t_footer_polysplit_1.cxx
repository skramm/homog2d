// file t_footer_polyline.cxx
// used in makefile target test-fig

	OPolyline psrc_o( src );
	CPolyline psrc_c( src );

	std::ofstream fhtml_O{ "polysplit_O.html", std::ios::app };
	std::ofstream fhtml_C{ "polysplit_C.html", std::ios::app };

	Homogr H;
	H.addScale( 50 );
	H.addTranslation( 40,40 );

	auto fname = std::regex_replace( std::string(argv[0]), std::regex("/bin/"), "/out/" );

	processSource( psrc_o, li, H, fname, "O" );
	processSource( psrc_c, li, H, fname, "C" );

	processAll( psrc_o, fname, "O", vv_pts_O, li, H, fhtml_O );
	processAll( psrc_c, fname, "C", vv_pts_C, li, H, fhtml_C );
}

/// Draw polyline as a set of oriented segments, with their index
template<typename IM,typename POLY,typename DP>
void
drawPoly( IM& im, const POLY& poly, const DP& dp )
{
	for( size_t j=0; j<poly.nbSegs(); j++ )
	{
		auto seg = poly.getOSegment(j);
		seg.draw( im, dp );
		im.drawText( std::to_string(j), Point2d(seg.getCenter() ) );
	}
}

template<typename PTYPE,typename HO>
void
processSource( const PTYPE& poly, const Line2d& li, const HO& H, std::string fname, std::string OC )
{
	auto dp = img::DrawParams().setThickness(3).setColor(250,0,0);

	img::Image<img::SvgImage> im_src(400,300);
	drawGrid( im_src, H );
	drawPoly( im_src, H*poly, dp );
//	(H*poly).draw( im_src, dp.setColor(250,0,0) );

	(H*li).draw( im_src, dp.setColor(0,250,0).setThickness(2) );
	im_src.write( fname+"_"+OC+".svg" );
}

template<typename TYPE,typename VVPTS>
std::vector<TYPE>
drawExpectedSplit( TYPE, const VVPTS& vv_pts, const Homogr& H, img::DrawParams dp, string fname, string t, std::ofstream& fhtml )
{
	std::vector<TYPE> v_exp;

	if( !vv_pts.size() )
	{
		fhtml << "<p>NIL</p>\n";
		return v_exp;
	}

// we need to store them so we can sort them
	for( size_t i=0;i<vv_pts.size(); i++ )
	{
		const auto& v_pts = vv_pts[i];
		v_exp.push_back( TYPE(v_pts) );
	}
	std::sort( v_exp.begin(), v_exp.end() );

	fhtml << "<table border='1'><tr>\n";
	for( size_t i=0;i<vv_pts.size(); i++ )
		fhtml << "<td>" << v_exp[i] << "</td>\n";
	fhtml << "</tr><tr>\n";

	for( size_t i=0;i<vv_pts.size(); i++ )
	{
		img::Image<img::SvgImage> im(400,300);
		drawGrid( im, H );
		drawPoly( im, H*v_exp[i], dp );

		char t2 = (v_exp[i].isClosed() ? 'c' : 'o');

		auto fn1 = fname+t+t2+'_'+std::to_string(i)+".svg";
		im.write( fn1 );
		fhtml << "<td><img src='"<< fn1 << "'></td>\n";
	}
	fhtml << "</tr></table>\n";

	return v_exp;
}



template<typename POLYSET,typename DP>
void
drawReal( POLYSET pset, const Homogr& H, DP dp, std::string fname, std::ofstream& fhtml )
{
	fhtml << "<table border='1'><tr>\n";
	for( size_t i=0;i<pset.size(); i++ )
		fhtml << "<td>" << pset[i] << "</td>\n";
	fhtml << "</tr><tr>\n";

	for( size_t i=0;i<pset.size(); i++ )
	{
		img::Image<img::SvgImage> im(400,300);
		drawGrid( im, H );
		drawPoly( im, H*pset[i], dp );
		auto fn = fname+std::to_string(i)+".svg";
		im.write( fn );
		fhtml << "<td><img src='"<< fn << "'></td>\n";
	}
	fhtml << "</tr></table>\n";

}
/// TYPE is the output split type
template<typename TYPE,typename POLY,typename DP>
std::vector<TYPE>
drawRealSplit( TYPE, const POLY& poly, const Line2d& li, const Homogr& H, DP dp, std::string fname, std::string suffix, std::ofstream& fhtml )
{
	auto dummy = TYPE();
	std::vector<TYPE> out;
	if constexpr( dummy.isClosed() )
	{
		out = poly.splitC( li );
	}
	else
	{
		out = poly.splitO( li );
	}
	std::sort( std::begin(out), std::end(out) );

	if( !out.size() )
		fhtml << "<p>NIL</p>\n";
	else
		drawReal( out, H, dp, fname + suffix, fhtml );
	return out;
}

template<typename PS>
void
printResult( std::ofstream& fhtml, const PS& ps_e, const PS& ps_r )
{
	if( ps_e == ps_r )
		fhtml << "<p style='color:green;'>Success !!</p>\n";
	else
		fhtml << "<p style='color:red;'>Failure !!</p>\n";
}

template<typename POLY,typename VVPTS>
void
processAll(
	const POLY&    poly,   ///< the polyline we want to split (open or closed)
	std::string    name,   ///< source filename
	std::string    OC,     ///< "O" or "C" (source polyline type)
	const VVPTS&   vv_pts, ///< expected set of points
	const Line2d&  li,     ///< line used for the split
	const Homogr&  H,      ///< Homography, needed to draw the grid
	std::ofstream& fhtml   ///< output HTML file
)
{
	auto fn = name + "_" + OC;
	auto dp = img::DrawParams().setColor(250,0,0).showPoints(true).setPointSize(7).setThickness(2);

	fhtml << "<h2>Source polyline: " << fn << "</h2>\n";
	{
		fhtml << "<h3>1 - Open output</h3>\nsource:<br>\n"
			<< "<img src='" << fn+".svg" << "'><br>\n";

		fhtml << "<h4>1.1 - Expected</h4>\n";
		auto ps_e = drawExpectedSplit( OPolyline(), vv_pts, H, dp, name, "_exp_"+OC, fhtml );
		fhtml << "<h4>1.2 - Generated</h4>\n";
		auto ps_r = drawRealSplit( OPolyline(), poly, li, H, dp, name, "_real_"+OC+"_o", fhtml );
		printResult( fhtml, ps_e, ps_r );
	}
	{
		fhtml << "<hr>\n<h3>2 - Closed output</h3>\nsource:<br>\n"
			<< "<img src='" << fn+".svg" << "'><br>\n";

		fhtml << "<h4>2.1 - Expected</h4>\n";
		auto ps_e = drawExpectedSplit( CPolyline(), vv_pts, H, dp, name, "_exp_"+OC, fhtml );
		fhtml << "<h4>2.2 - Generated</h4>\n";
		auto ps_r = drawRealSplit( CPolyline(), poly, li, H, dp, name, "_real_"+OC+"_c", fhtml );
		printResult( fhtml, ps_e, ps_r );
	}
	fhtml << "<hr>\n";
}


