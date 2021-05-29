/**************************************************************************

    This file is part of the C++ library "homog2d", dedicated to
    handle 2D lines and points, see https://github.com/skramm/homog2d

    Author & Copyright 2019 Sebastien Kramm

    Contact: firstname.lastname@univ-rouen.fr

    Licence: MPL v2

	This Source Code Form is subject to the terms of the Mozilla Public
	License, v. 2.0. If a copy of the MPL was not distributed with this
	file, You can obtain one at https://mozilla.org/MPL/2.0/.

**************************************************************************/

/**
\file homog2d.hpp
\brief single header file, implements some 2D homogeneous stuff.
See https://github.com/skramm/homog2d
*/

#ifndef HG_HOMOG2D_HPP
#define HG_HOMOG2D_HPP

#include <cmath>
#include <iostream>
#include <algorithm>
#include <set>
#include <vector>
#include <iomanip>
#include <cassert>
#include <sstream>
#include <type_traits>

#ifdef HOMOG2D_USE_OPENCV
	#include "opencv2/imgproc.hpp"
#endif


#define HOMOG2D_CHECK_ROW_COL \
	if( r > 2 ) \
		throw std::runtime_error( "Error: invalid row value: r=" + std::to_string(r) ); \
	if( c > 2 ) \
		throw std::runtime_error( "Error: invalid col value: r=" + std::to_string(r) )


#define HOMOG2D_CHECK_IS_NUMBER(T) \
	static_assert( std::is_arithmetic<T>::value && !std::is_same<T, bool>::value, "Type must be numerical" )

namespace homog2d {

/// Holds the types needed for policy based design
namespace type {

struct IsLine   {};
struct IsPoint  {};
struct IsHomogr {};
struct IsMatrix {};

} // namespace detail end


namespace detail {

	/// Helper class for Root (Point/Line) type, used as a trick to allow partial specialization of member functions
	template<typename>
	struct RootHelper {};

#if 0
	/// Helper class for Matrix type
	template<typename T1>
	struct HelperMat {};

	template<>
	struct HelperMat<type::IsHomogr>
	{
		using M_OtherType = IsMatrix;
	};

	template<>
	struct HelperMat<type::IsMatrix>
	{
		using M_OtherType = IsHomogr;
	};
#endif

    template<typename>
    struct HelperPL;

    template<>
    struct HelperPL<type::IsPoint>
    {
        using OtherType = type::IsLine;
    };

    template<>
    struct HelperPL<type::IsLine>
    {
        using OtherType = type::IsPoint;
    };

	/// A trick used in static_assert, so it aborts only if function is instanciated
	template<typename T>
	struct AlwaysFalse {
		enum { value = false };
	};

} // namespace detail


// forward declaration
template<typename LP,typename FPT>
class Root;

// forward declaration
template<typename LP,typename FPT>
class Hmatrix_;

// forward declaration
template<typename FPT>
class Segment_;


namespace detail {

// forward declaration of function
template<typename T1,typename T2,typename U,typename FPT1,typename FPT2>
void
product(
	Root<T1,FPT1>&          out,
	const Hmatrix_<U,FPT2>& h,
	const Root<T2,FPT1>&    in
);

} // namespace detail

//------------------------------------------------------------------
/// A 2D homography, defining a planar transformation
/**
To define an affine or rigid transformation, you can use:
- setRotation()
- setTranslation()
- setScale()

To add an affine or rigid transformation to the current one, you can use:
- addRotation()
- addTranslation()
- addScale()

To return to unit transformation, use init()

Implemented as a 3x3 matrix

Templated by Floating-Point Type (FPT) and by type M (type::IsMatrix or type::IsHomogr)
 */
template<typename M,typename FPT>
class Hmatrix_
{
	template<typename T1,typename T2>
	friend class Root;

	template<typename T1,typename T2,typename U,typename FPT1,typename FPT2>
	friend void detail::product( Root<T1,FPT1>&, const Hmatrix_<U,FPT2>&, const Root<T2,FPT1>& );

	public:
	/// Default constructor, initialize to unit transformation
	Hmatrix_()
	{
		init();
	}

	void init()
	{
		impl_mat_init0( detail::RootHelper<M>() );
	}

/// Build homography and set it to a rotation matrix of angle \c val
	template<typename T>
	Hmatrix_( T val )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		init();
		setRotation( val );
	}

/// Build homography and set it to a translation matrix ( see Hmatrix_( T ) )
	template<typename T>
	Hmatrix_( T tx, T ty )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		init();
		setTranslation( tx, ty );
	}

	private:
/// Implementation for matrices: initialize to empty
	void impl_mat_init0( const detail::RootHelper<type::IsMatrix>& )
	{
		for( auto& li: _data )
			for( auto& elem: li )
				elem = 0.;
	}

/// Implementation for homographies: initialize to unit transformation
	void impl_mat_init0( const detail::RootHelper<type::IsHomogr>& )
	{
		impl_mat_init0( detail::RootHelper<type::IsMatrix>() ); // call other overload
/*		for( auto& li: _data )
			for( auto& elem: li )
				elem = 0.; */
		_data[0][0] = 1.;
		_data[1][1] = 1.;
		_data[2][2] = 1.;
		_isNormalized = true;
	}
	public:

/// Constructor, used to fill with a vector of vectors matrix
/** \warning
- Input matrix \b must be 3 x 3, but type can be anything that can be copied to \c double
- no checking is done on validity of matrix as an homography.
Thus some assert can get triggered elsewhere.
*/
	template<typename T>
	Hmatrix_( const std::vector<std::vector<T>>& in )
	{
#ifdef HOMOG2D_SAFE_MODE
		HOMOG2D_CHECK_IS_NUMBER(T);
		if( in.size() != 3 )
			throw std::runtime_error( "Invalid line size for input: " + std::to_string(in.size()) );
		for( auto li: in )
			if( li.size() != 3 )
				throw std::runtime_error( "Invalid column size for input: " + std::to_string(li.size()) );
#endif
		p_fillWith( in );
	}

/// Constructor, used to fill with a std::array
	template<typename T>
	Hmatrix_( const std::array<std::array<T,3>,3>& in )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		p_fillWith( in );
	}

#ifdef HOMOG2D_USE_OPENCV
/// Constructor used to initialise with a cv::Mat, call the assignment operator
	Hmatrix_( const cv::Mat& mat )
	{
		*this = mat;
	}

#endif

/// Setter \warning No normalization is done, as this can be done
/// several times to store values, we therefore must not normalize in between
	template<typename T>
	void set(
		size_t r, ///< row
		size_t c, ///< col
		T      v  ///< value
	)
	{
		#ifdef HOMOG2D_SAFE_MODE
			HOMOG2D_CHECK_ROW_COL;
		#endif
		_data[r][c] = v;
		_isNormalized = false;
	}
/// Getter
	FPT get( size_t r, size_t c ) const
	{
		#ifdef HOMOG2D_SAFE_MODE
			HOMOG2D_CHECK_ROW_COL;
		#endif
		return _data[r][c];
	}
/// Adds a translation \c tx,ty to the matrix
	template<typename T>
	Hmatrix_& addTranslation( T tx, T ty )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		Hmatrix_ out;
		out.setTranslation( tx, ty );
		*this = out * *this;
//		normalize();
		return *this;
	}
/// Sets the matrix as a translation \c tx,ty
	template<typename T>
	Hmatrix_& setTranslation( T tx, T ty )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		init();
		_data[0][2] = tx;
		_data[1][2] = ty;
		_isNormalized = true;
		return *this;
	}
/// Adds a rotation with an angle \c theta (radians) to the matrix
	template<typename T>
	Hmatrix_& addRotation( T theta )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		Hmatrix_ out;
		out.setRotation( theta );
		*this = out * *this;
//		normalize();
		return *this;
	}
/// Sets the matrix as a rotation with an angle \c theta (radians)
	template<typename T>
	Hmatrix_& setRotation( T theta )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		init();
		_data[0][0] = _data[1][1] = std::cos(theta);
		_data[1][0] = std::sin(theta);
		_data[0][1] = -_data[1][0];
		_isNormalized = true;
		return *this;
	}
/// Adds the same scale factor to the matrix
	template<typename T>
	Hmatrix_& addScale( T k )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		return this->addScale( k, k );
	}
/// Adds a scale factor to the matrix
	template<typename T>
	Hmatrix_& addScale( T kx, T ky )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		Hmatrix_ out;
		out.setScale( kx, ky );
		*this = out * *this;
//		normalize();
		return *this;
	}
/// Sets the matrix as a scaling transformation (same on two axis)
	template<typename T>
	Hmatrix_& setScale( T k )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		return setScale( k, k );
	}
/// Sets the matrix as a scaling transformation
	template<typename T>
	Hmatrix_& setScale( T kx, T ky )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		init();
		_data[0][0] = kx;
		_data[1][1] = ky;
		_isNormalized = true;
		return *this;
	}

	template<typename T>
	void
	applyTo( T& ) const;

#ifdef HOMOG2D_USE_OPENCV
	void copyTo( cv::Mat&, int type=CV_64F ) const;
	Hmatrix_& operator = ( const cv::Mat& );
#endif

/// Normalisation
	void normalize() const
	{
		auto eps = std::numeric_limits<FPT>::epsilon();

		if( std::fabs(_data[2][2]) > eps ) // if [2][2] is null, then we use [2][1]
			p_divideBy( 2, 2 );
		else
		{
			if( std::fabs(_data[2][1]) > eps )
				p_divideBy( 2, 1 );
			else
				p_divideBy( 2, 0 );
		}
		if( std::signbit(_data[2][2]) )
			for( auto& li: _data )
				for( auto& e: li )
					e = -e;
		_isNormalized = true;
	}

/// Transpose matrix
	Hmatrix_& transpose()
	{
		Hmatrix_ out;
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				out._data[i][j] = _data[j][i];
		*this = out;
		return *this;
	}

/// Inverse matrix
	Hmatrix_& inverse()
	{
		Hmatrix_ adjugate = p_adjugate();
		auto det = p_det();

		if( std::abs(det) <= Hmatrix_<M,FPT>::nullDeterValue() )
			throw std::runtime_error( "matrix is not invertible" );

		*this = adjugate / det;

		normalize();
		return *this;
	}

/// Divide all elements by scalar
	template<typename T>
	Hmatrix_& operator / (T v)
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		if( std::abs(v) <= std::numeric_limits<double>::epsilon() )
			throw std::runtime_error( "unable to divide by " + std::to_string(v) );
#if 0
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				_data[i][j] /= v;
		return *this;
#else
		return *this * (1.0/v);
#endif
	}
/// Multiply all elements by scalar
/**
Can't be templated by arg type because it would conflict with operator * for Homogr and line/point
*/
	Hmatrix_& operator * (FPT v)
	{
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				_data[i][j] *= v;
		return *this;
	}

/// Matrix multiplication
	friend Hmatrix_ operator * ( const Hmatrix_& h1, const Hmatrix_& h2 )
	{
		Hmatrix_ out;
		out.p_zero();
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				for( int k=0; k<3; k++ )
					out._data[i][j] += h1._data[i][k] * h2._data[k][j];
		return out;
	}

/// Comparison operator. Does normalization if required
	bool operator == ( const Hmatrix_& h ) const
	{
		if( !_isNormalized )
			normalize();
		if( !h._isNormalized )
			h.normalize();

		auto eps = std::numeric_limits<double>::epsilon();
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				if( std::fabs( _data[i][j] - h._data[i][j] ) >= eps )
					return false;
		return true;
	}
/// Comparison operator. Does normalization if required
	bool operator != ( const Hmatrix_& h ) const
	{
		return !(*this == h);
	}
	static double& nullDeterValue() { return _zeroDeterminantValue; }

//////////////////////////
//   PRIVATE FUNCTIONS  //
//////////////////////////
	private:
	void p_zero()
	{
		for( auto& li: _data )
			for( auto& e: li )
				e = 0.;
	}
	template<typename T>
	void p_fillWith( const T& in )
	{
		for( auto i=0; i<3; i++ )
			for( auto j=0; j<3; j++ )
				_data[i][j] = in[i][j];
		normalize();
	}
	void p_divideBy( size_t r, size_t c ) const
	{
		assert( std::fabs( _data[r][c] ) > std::numeric_limits<double>::epsilon() );
		for( auto& li: _data )
			for( auto& e: li )
				e /= _data[r][c];
	}
/// Return determinant of matrix
/**
See https://en.wikipedia.org/wiki/Determinant
*/
	double p_det()
	{
		double det = _data[0][0] * p_det2x2( {1,1, 1,2, 2,1, 2,2} );
		det       -= _data[0][1] * p_det2x2( {1,0, 1,2, 2,0, 2,2} );
		det       += _data[0][2] * p_det2x2( {1,0, 1,1, 2,0, 2,1} );
		return det;
	}
	double p_det2x2( std::vector<int> v )
	{
		auto det = _data[v[0]][v[1]] * _data[v[6]][v[7]];
		det -= _data[v[2]][v[3]] * _data[v[4]][v[5]];
		return det;
	}
/// Computes adjugate matrix, see https://en.wikipedia.org/wiki/Adjugate_matrix#3_%C3%97_3_generic_matrix
	Hmatrix_ p_adjugate()
	{
		Hmatrix_ out;

		out.set( 0, 0,  p_det2x2( {1,1, 1,2, 2,1, 2,2} ) );
		out.set( 0, 1, -p_det2x2( {0,1, 0,2, 2,1, 2,2} ) );
		out.set( 0, 2,  p_det2x2( {0,1, 0,2, 1,1, 1,2} ) );

		out.set( 1, 0, -p_det2x2( {1,0, 1,2, 2,0, 2,2} ) );
		out.set( 1, 1,  p_det2x2( {0,0, 0,2, 2,0, 2,2} ) );
		out.set( 1, 2, -p_det2x2( {0,0, 0,2, 1,0, 1,2} ) );

		out.set( 2, 0,  p_det2x2( {1,0, 1,1, 2,0, 2,1} ) );
		out.set( 2, 1, -p_det2x2( {0,0, 0,1, 2,0, 2,1} ) );
		out.set( 2, 2,  p_det2x2( {0,0, 0,1, 1,0, 1,1} ) );

		return out;
	}

//////////////////////////
//      DATA SECTION    //
//////////////////////////
	private:
	mutable std::array<std::array<FPT,3>,3> _data;
	mutable bool _isNormalized = false;

	friend std::ostream& operator << ( std::ostream& f, const Hmatrix_& h )
	{
		for( const auto& li: h._data )
		{
			f << "| ";
			for( const auto& e: li )
				f << std::setw(6) << e << ' ';
			f << " |\n";
		}
		return f;
	}
	static double _zeroDeterminantValue; /// Used in matrix inversion
};



#ifdef HOMOG2D_USE_OPENCV
//------------------------------------------------------------------
/// Point drawing style
enum PointStyle
{
	PS_PLUS,   ///< "+" symbol
	PS_TIMES,  ///< "times" symbol
	PS_STAR,   ///< "*" symbol
	PS_DIAM    ///< diamond
};

//------------------------------------------------------------------
/// Draw parameters for Opencv binding, see Root::draw()
struct CvDrawParams
{
/// Inner struct, holds the values. Needed so we can assign a default value as static member
	struct Dp_values
	{
		cv::Scalar _color         = cv::Scalar(128,128,128);
		int        _lineThickness = 1;
		int        _lineType      = cv::LINE_AA; // or cv::LINE_8
		int        _ptDelta       = 8; // pixels, used for drawing points
		PointStyle _ptStyle       = PS_PLUS;
		bool       _enhancePoint  = false;   // to draw selected points
	};
	Dp_values _dpValues;

	private:
	static Dp_values& p_getDefault()
	{
		static Dp_values s_defValue;
		return s_defValue;
	}

	public:
	CvDrawParams()
	{
		_dpValues = p_getDefault();
	}
	void setDefault()
	{
		p_getDefault() = this->_dpValues;
	}
	static void resetDefault()
	{
		p_getDefault() = Dp_values();
	}
	CvDrawParams& setPointStyle( PointStyle ps )
	{
		_dpValues._ptStyle = ps;
		return *this;
	}
	CvDrawParams& setPointSize( int ps )
	{
		assert( ps>1 );
		_dpValues._ptDelta = ps;
		return *this;
	}
	CvDrawParams& setThickness( int t )
	{
		assert( t>0 );
		_dpValues._lineThickness = t;
		return *this;
	}
	CvDrawParams& setColor( uint8_t r, uint8_t g, uint8_t b )
	{
		_dpValues._color = cv::Scalar(b,g,r);
		return *this;
	}
	CvDrawParams& selectPoint()
	{
		_dpValues._enhancePoint = true;
		return *this;
	}
};
#endif // HOMOG2D_USE_OPENCV
//------------------------------------------------------------------

/// Used in Line2d::getValue() and getOrthogonalLine()
enum En_GivenCoord { GC_X, GC_Y };

/// Used in Line2d::addOffset
enum En_OffsetDir{ OD_Vert, OD_Horiz };

// forward declaration of template instanciation
namespace detail {

template<typename T1,typename T2,typename T3>
Root<T1,T3> crossProduct( const Root<T2,T3>&, const Root<T2,T3>& );

}

//------------------------------------------------------------------
/// Base class, will be instanciated as a Point2d or a Line2d
/**
Parameters:
- LP: Line or Point
- FPT: Floating Point Type
*/
template<typename LP,typename FPT>
class Root
{
	template<typename U,typename V>
	friend class Hmatrix_;

//	template<typename T,typename U,typename V,typename W>
//	friend Root<T,V>
//	operator * ( const Hmatrix_<W,U>&, const Root<T,V>& );

	template<typename T>
	friend Root<type::IsPoint,T>
	operator * ( const Root<type::IsLine,T>&, const Root<type::IsLine,T>& );

	template<typename T>
	friend Root<type::IsLine,T>
	operator * ( const Root<type::IsPoint,T>&, const Root<type::IsPoint,T>& );

	template<typename T1,typename T2,typename T3>
	friend Root<T1,T3>
	detail::crossProduct( const Root<T2,T3>&, const Root<T2,T3>& );

	template<typename U,typename V>
	friend std::ostream&
	operator << ( std::ostream& f, const Root<U,V>& r );

	template<typename T1,typename T2,typename U,typename FPT1,typename FPT2>
	friend void detail::product( Root<T1,FPT1>&, const Hmatrix_<U,FPT2>&, const Root<T2,FPT1>& );

/*	private:
		Root( double a, double b, double c )
		{
			_v[0] = a;
			_v[1] = b;
			_v[2] = c;
		}*/

	public:


/// Constructor: build a point from two lines
		Root( const Root<type::IsLine,FPT>& v1, const Root<type::IsLine,FPT>& v2 )
		{
#ifdef HOMOG2D_SAFE_MODE
			if( v1.isParallelTo(v2) )
				std::runtime_error( "unable to build point from these two lines, are parallel" );
#endif
			*this = detail::crossProduct<type::IsPoint>( v1, v2 );
		}

/// Constructor: build a line from two points
		Root( const Root<type::IsPoint,FPT>& v1, const Root<type::IsPoint,FPT>& v2 )
		{
#ifdef HOMOG2D_SAFE_MODE
			if( v1 == v2 )
				std::runtime_error( "unable to build line from these two points, are the same" );
#endif
			*this = detail::crossProduct<type::IsLine>( v1, v2 );
			p_normalizeLine();
		}

/// Constructor: build a line passing through (0,0) and \c pt
/** This will call one of the two overloads of \c impl_init_1_Line(), depending on type of object */
		Root( const Root<type::IsLine,FPT>& li )
		{
			impl_init_1_Line( li, detail::RootHelper<LP>() );
		}

/// Constructor with single arg of type "Point"
/** This will call one of the two overloads of \c impl_init_1_Point(), depending on type of object */
		Root( const Root<type::IsPoint,FPT>& pt )
		{
			impl_init_1_Point( pt, detail::RootHelper<LP>() );
		}

/// Constructor: build from two numerical values, depends on the type
		template<typename T>
		Root( const T& v1, const T& v2 )
		{
			HOMOG2D_CHECK_IS_NUMBER(T);
			impl_init_2( v1, v2, detail::RootHelper<LP>() );
		}

/// Default constructor, depends on the type
		Root()
		{
			impl_init( detail::RootHelper<LP>() );
		}

	private:
		/// Arg is a point, object is a point => copy constructor
		void impl_init_1_Point( const Root<type::IsPoint,FPT>& pt, const detail::RootHelper<type::IsPoint>&  )
		{
			*this = pt;
		}
		/// Arg is a point, object is a line: we build the line passing though (0,0) ant the given point
		void impl_init_1_Point( const Root<type::IsPoint,FPT>& pt, const detail::RootHelper<type::IsLine>&  )
		{
			*this = detail::crossProduct<type::IsLine>( pt, Root<type::IsPoint,FPT>() );
			p_normalizeLine();
		}

		/// Arg is a line, object is a point: ILLEGAL INSTANCIATION
		void impl_init_1_Line( const Root<type::IsLine,FPT>& li, const detail::RootHelper<type::IsPoint>&  )
		{
			static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot build a point from a line" );
		}
		/// Arg is a line, object is a line => copy constructor
		void impl_init_1_Line( const Root<type::IsLine,FPT>& li, const detail::RootHelper<type::IsLine>&  )
		{
			*this = li;
		}

	public:
		FPT
		getCoord( En_GivenCoord gc, FPT other ) const
		{
			return impl_getCoord( gc, other, detail::RootHelper<LP>() );
		}

		Root<type::IsPoint,FPT>
		getPoint( En_GivenCoord gc, FPT other ) const
		{
			return impl_getPoint( gc, other, detail::RootHelper<LP>() );
		}

		/// Returns a pair of points that are lying on line ad distance \c dist from a point defined by one of its coordinates.
		std::pair<Root<type::IsPoint,FPT>,Root<type::IsPoint,FPT>>
		getPoints( En_GivenCoord gc, FPT coord, FPT dist ) const
		{
			return impl_getPoints( gc, coord, dist, detail::RootHelper<LP>() );
		}

		/// Returns an orthogonal line to the one it is called on, at a point defined by one of its coordinates.
		Root<type::IsLine,FPT>
		getOrthogonalLine( En_GivenCoord gc, FPT other ) const
		{
			return impl_getOrthogonalLine( gc, other, detail::RootHelper<LP>() );
		}

		/// Returns an parallel line to the one it is called on, with \c pt lying on it.
		Root<type::IsLine,FPT>
		getParallelLine( const Root<type::IsPoint,FPT>& pt ) const
		{
			return impl_getParallelLine( pt, detail::RootHelper<LP>() );
		}

		template<typename T>
		void
		addOffset( En_OffsetDir dir, T v )
		{
			HOMOG2D_CHECK_IS_NUMBER(T);
			impl_addOffset( dir, v, detail::RootHelper<LP>() );
		}

		FPT getX() const         { return impl_getX( detail::RootHelper<LP>() ); }
		FPT getY() const         { return impl_getY( detail::RootHelper<LP>() ); }
		void set( FPT x, FPT y ) { impl_set( x, y,   detail::RootHelper<LP>() ); }

		FPT distTo( const Root<type::IsPoint,FPT>& pt ) const
		{
			return impl_distToPoint( pt, detail::RootHelper<LP>() );
		}
		FPT distTo( const Root<type::IsLine,FPT>& li ) const
		{
			return impl_distToLine( li, detail::RootHelper<LP>() );
		}

		template<typename T>
		bool isParallelTo( const Root<T,FPT>& li ) const
		{
			return impl_isParallelTo( li, detail::RootHelper<T>() );
		}
		template<typename T>
		bool isParallelTo( const Segment_<T>& seg ) const;

		template<typename T>
		FPT getAngle( const Root<T,FPT>& other ) const
		{
			return impl_getAngle( other, detail::RootHelper<T>() );
		}
		template<typename T>
		FPT getAngle( const Segment_<T>& seg ) const;

	private:
		FPT impl_getX( const detail::RootHelper<type::IsPoint>& /* dummy */ ) const
		{
			return _v[0]/_v[2];
		}
		FPT impl_getY( const detail::RootHelper<type::IsPoint>& /* dummy */ ) const
		{
			return _v[1]/_v[2];
		}
		void impl_set( FPT x, FPT y, const detail::RootHelper<type::IsPoint>& /* dummy */ )
		{
			_v[0] = x;
			_v[1] = y;
			_v[2] = 1.;
		}

		template<typename T>
		void
		impl_addOffset( En_OffsetDir dir, T v, const detail::RootHelper<type::IsLine>& );

		FPT impl_distToPoint( const Root<type::IsPoint,FPT>&, const detail::RootHelper<type::IsPoint>& ) const;
		FPT impl_distToPoint( const Root<type::IsPoint,FPT>&, const detail::RootHelper<type::IsLine>&  ) const;
		FPT impl_distToLine(  const Root<type::IsLine,FPT>&,  const detail::RootHelper<type::IsPoint>& ) const;
		FPT impl_distToLine(  const Root<type::IsLine,FPT>&,  const detail::RootHelper<type::IsLine>&  ) const;

		FPT  impl_getAngle(     const Root<LP,FPT>&, const detail::RootHelper<type::IsLine>&  ) const;
		FPT  impl_getAngle(     const Root<LP,FPT>&, const detail::RootHelper<type::IsPoint>& ) const;
		bool impl_isParallelTo( const Root<LP,FPT>&, const detail::RootHelper<type::IsLine>&  ) const;
		bool impl_isParallelTo( const Root<LP,FPT>&, const detail::RootHelper<type::IsPoint>& ) const;

		FPT impl_getCoord( En_GivenCoord gc, FPT other, const detail::RootHelper<type::IsLine>& ) const;
		Root<type::IsPoint,FPT> impl_getPoint( En_GivenCoord gc, FPT other, const detail::RootHelper<type::IsLine>& ) const;

		std::pair<Root<type::IsPoint,FPT>,Root<type::IsPoint,FPT>> impl_getPoints( En_GivenCoord gc, FPT coord, FPT dist, const detail::RootHelper<type::IsLine>& ) const;
		std::pair<Root<type::IsPoint,FPT>,Root<type::IsPoint,FPT>> impl_getPoints( En_GivenCoord gc, FPT coord, FPT dist, const detail::RootHelper<type::IsPoint>& ) const;

		void impl_op_stream( std::ostream&, const Root<type::IsPoint,FPT>& ) const;
		void impl_op_stream( std::ostream&, const Root<type::IsLine,FPT>& ) const;

	public:
/// Sub-type, holds result of rectangle intersection, see intersectsRectangle().
/// Only defined for Point2d
/// \todo change name (as this will be used also for line-circle intersection)
	struct Intersect
	{
		template<typename U,typename V>
		friend class Root;

		public:
			bool operator()() const
			{
				return _doesIntersect;
			}
			Intersect()
			{}
			Intersect( const Root<type::IsPoint,FPT>& p1, const Root<type::IsPoint,FPT>& p2 ) : ptA(p1), ptB(p2)
			{
				_doesIntersect = true;
			}
			std::pair<Root<type::IsPoint,FPT>,Root<type::IsPoint,FPT>>
			get() const
			{
				return std::make_pair( ptA, ptB );
			}
		private:
			Root<type::IsPoint,FPT> ptA;
			Root<type::IsPoint,FPT> ptB;
			bool _doesIntersect = false;
	};

	public:
		Intersect intersectsRectangle( const Root<type::IsPoint,FPT>& pt1, const Root<type::IsPoint,FPT>& pt2 ) const;
		template<typename T>
		Intersect intersectsCircle( const Root<type::IsPoint,FPT>& pt0, T radius ) const;

		bool isInsideRectangle( const Root<type::IsPoint,FPT>& pt1, const Root<type::IsPoint,FPT>& pt2 ) const
		{
			return impl_isInsideRectangle( pt1, pt2, detail::RootHelper<LP>() );
		}
		template<typename T>
		bool isInsideCircle( const Root<type::IsPoint,FPT>& center, T radius ) const
		{
			static_assert( std::is_arithmetic<T>::value && !std::is_same<T,bool>::value, "Radius type needs to be arithmetic" );
			return impl_isInsideCircle( center, radius, detail::RootHelper<LP>() );
		}

//////////////////////////
//       OPERATORS      //
//////////////////////////
		bool operator == ( const Root<LP,FPT>& other ) const
		{
			return impl_op_equal( other, detail::RootHelper<LP>() );
		}
		bool operator != ( const Root<LP,FPT>& other ) const
		{
			return !(*this == other);
		}

// optional stuff
#ifdef HOMOG2D_USE_OPENCV

/// Constructor: build line from two OpenCv points
/*		template<typename T>
		Root( const cv::Point2d& p1, const cv::Point2d& p2 )
		{
			impl_init_2( v1, v2, detail::RootHelper<LP>() );
		}*/

		bool draw( cv::Mat& mat, CvDrawParams dp=CvDrawParams() )  const
		{
			return impl_draw( mat, dp, detail::RootHelper<LP>() );
		}
		cv::Point2i getCvPti() const { return impl_getCvPt( detail::RootHelper<LP>(), cv::Point2i() ); }
		cv::Point2d getCvPtd() const { return impl_getCvPt( detail::RootHelper<LP>(), cv::Point2d() ); }
		cv::Point2f getCvPtf() const { return impl_getCvPt( detail::RootHelper<LP>(), cv::Point2f() ); }

		template<typename T>
		Root( cv::Point_<T> pt )
		{
			impl_init_opencv( pt, detail::RootHelper<LP>() );
		}
#endif

	static double& nullAngleValue() { return _zeroAngleValue; }
	static double& nullDistance()   { return _zeroDistance; }

//////////////////////////
//      DATA SECTION    //
//////////////////////////

	private:
		FPT _v[3]; ///< data, uses the template parameter FPT (for "Floating Point Type")

		static double _zeroAngleValue;       /// Used in isParallel();
		static double _zeroDistance;         /// Used to define points as identical

//////////////////////////
//   PRIVATE FUNCTIONS  //
//////////////////////////
	private:
		void p_normalizeLine() const { impl_normalizeLine( detail::RootHelper<LP>() ); }


		Root<LP,FPT>::Intersect
		impl_intersectsRectangle( const Root<type::IsPoint,FPT>& p0, const Root<type::IsPoint,FPT>& p1, const detail::RootHelper<type::IsLine>& ) const;
		Root<LP,FPT>::Intersect
		impl_intersectsRectangle( const Root<type::IsPoint,FPT>& p0, const Root<type::IsPoint,FPT>& p1, const detail::RootHelper<type::IsPoint>& ) const;

		template<typename T>
		Root<LP,FPT>::Intersect
		impl_intersectsCircle( const Root<type::IsPoint,FPT>& pt, T radius, const detail::RootHelper<type::IsLine>& ) const;
		template<typename T>
		Root<LP,FPT>::Intersect
		impl_intersectsCircle( const Root<type::IsPoint,FPT>& pt, T radius, const detail::RootHelper<type::IsPoint>& ) const;


		bool impl_isInsideRectangle( const Root<type::IsPoint,FPT>&, const Root<type::IsPoint,FPT>&, const detail::RootHelper<type::IsPoint>& ) const;
		bool impl_isInsideRectangle( const Root<type::IsPoint,FPT>&, const Root<type::IsPoint,FPT>&, const detail::RootHelper<type::IsLine>&  ) const;

		template<typename T>
		bool impl_isInsideCircle( const Root<type::IsPoint,FPT>&, T radius, const detail::RootHelper<type::IsLine>&  ) const;
		template<typename T>
		bool impl_isInsideCircle( const Root<type::IsPoint,FPT>&, T radius, const detail::RootHelper<type::IsPoint>& ) const;

		void impl_normalizeLine( const detail::RootHelper<type::IsLine>& ) const;

		Root<type::IsLine,FPT> impl_getOrthogonalLine( En_GivenCoord gc, FPT val, const detail::RootHelper<type::IsLine>& ) const;
		Root<type::IsLine,FPT> impl_getOrthogonalLine( En_GivenCoord gc, FPT val, const detail::RootHelper<type::IsPoint>& ) const;
		Root<type::IsLine,FPT> impl_getParallelLine( const Root<type::IsPoint,FPT>&,    const detail::RootHelper<type::IsLine>& ) const;
		Root<type::IsLine,FPT> impl_getParallelLine( const Root<type::IsPoint,FPT>&,    const detail::RootHelper<type::IsPoint>& ) const;

		bool impl_op_equal( const Root<LP,FPT>&, const detail::RootHelper<type::IsLine>& ) const;
		bool impl_op_equal( const Root<LP,FPT>&, const detail::RootHelper<type::IsPoint>& ) const;

		Root<type::IsPoint,FPT> impl_op_product( const Root<type::IsLine,FPT>& , const Root<type::IsLine,FPT>& , const detail::RootHelper<type::IsPoint>& ) const;
		Root<type::IsLine,FPT>  impl_op_product( const Root<type::IsPoint,FPT>&, const Root<type::IsPoint,FPT>&, const detail::RootHelper<type::IsLine>&  ) const;

#ifdef HOMOG2D_USE_OPENCV
		template<typename OPENCVT>
		OPENCVT impl_getCvPt( const detail::RootHelper<type::IsPoint>&, const OPENCVT& ) const;

		template<typename T>
		void impl_init_opencv( T pt, const detail::RootHelper<type::IsPoint>& )
		{
			impl_init_2( pt.x, pt.y, detail::RootHelper<type::IsPoint>() );
		}
		template<typename T>
		void impl_init_opencv( T pt, const detail::RootHelper<type::IsLine>& )
		{
			static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot build a line using an OpenCv point" );
		}

		bool impl_draw( cv::Mat&, const CvDrawParams&, const detail::RootHelper<type::IsPoint>& ) const;
		bool impl_draw( cv::Mat&, const CvDrawParams&, const detail::RootHelper<type::IsLine>& )  const;
#endif

		/// Called by default constructor, overload for lines
		void impl_init( const detail::RootHelper<type::IsLine>& )
		{
			_v[0] = 1.;
			_v[1] = 0.;
			_v[2] = 0.;
		}
		/// Called by default constructor, overload for points
		void impl_init( const detail::RootHelper<type::IsPoint>& )
		{
			_v[0] = 0.;
			_v[1] = 0.;
			_v[2] = 1.;
		}
		template<typename T>
		void impl_init_2( const T& v1, const T& v2, const detail::RootHelper<type::IsPoint>& );
		template<typename T>
		void impl_init_2( const T& v1, const T& v2, const detail::RootHelper<type::IsLine>& );
};


/// Instanciation of static variable
template<typename LP,typename FPT>
double Root<LP,FPT>::_zeroAngleValue = 0.001; // 1 thousand of a radian (tan = 0.001 too)

/// Instanciation of static variable
template<typename LP,typename FPT>
double Hmatrix_<LP,FPT>::_zeroDeterminantValue = 1E-20;

/// Instanciation of static variable
template<typename LP,typename FPT>
double Root<LP,FPT>::_zeroDistance = 1E-15;

#ifdef HOMOG2D_USE_OPENCV
/// Free function to return an OpenCv point (double)
template<typename FPT>
cv::Point2d
getCvPtd( const Root<type::IsPoint,FPT>& pt )
{
	return pt.getCvPtd();
}
/// Free function to return an OpenCv point (float)
template<typename FPT>
cv::Point2f
getCvPtf( const Root<type::IsPoint,FPT>& pt )
{
	return pt.getCvPtf();
}
/// Free function to return an OpenCv point (integer)
template<typename FPT>
cv::Point2i
getCvPti( const Root<type::IsPoint,FPT>& pt )
{
	return pt.getCvPti();
}
#endif


//------------------------------------------------------------------
/// This namespace holds some private stuff
namespace detail {

/// Private free function, get top-left and bottom-right points from two arbitrary points
/// \todo Fix floating point issue (comparison), add some threshold
template<typename FPT>
std::pair<Root<type::IsPoint,FPT>,Root<type::IsPoint,FPT>>
getCorrectPoints( const Root<type::IsPoint,FPT>& p0, const Root<type::IsPoint,FPT>& p1 )
{
	if( p0.getX() == p1.getX() || p0.getY() == p1.getY() )
		throw std::runtime_error( "error: a coordinate of the 2 points are identical, does not define a rectangle" );

	Root<type::IsPoint,FPT> p00( std::min(p0.getX(), p1.getX()), std::min(p0.getY(), p1.getY()) );
	Root<type::IsPoint,FPT> p11( std::max(p0.getX(), p1.getX()), std::max(p0.getY(), p1.getY()) );
	return std::make_pair( p00, p11 );
}

//------------------------------------------------------------------
/// Private free function, returns true if point \c pt is inside the rectangle defined by (\c p00 , \c p11)
template<typename FPT>
bool
ptIsInside( const Root<type::IsPoint,FPT>& pt, const Root<type::IsPoint,FPT>& p00, const Root<type::IsPoint,FPT>& p11 )
{
	if( pt.getX() >= p00.getX() && pt.getX() <= p11.getX() )
		if( pt.getY() >= p00.getY() && pt.getY() <= p11.getY() )
			return true;
	return false;
}

/// Private free function, swap the points so that \c ptA.x <= \c ptB.x, and if equal, sorts on y
template<typename FPT>
void
fix_order( Root<type::IsPoint,FPT>& ptA, Root<type::IsPoint,FPT>& ptB )
{
	if( ptA.getX() > ptB.getX() )
		std::swap( ptA, ptB );
	else
		if( ptA.getX() == ptB.getX() )
			if( ptA.getY() > ptB.getY() )
				std::swap( ptA, ptB );
}

} // namespace detail end


//------------------------------------------------------------------
/// A line segment, define by two points
/**
- Storage: "smallest" point is always stored as first element (see constructor)
*/
template<typename FPT>
class Segment_
{
	private:
		Root<type::IsPoint,FPT> _ptS1, _ptS2;

	public:
/// sub type, holds intersection between two segments
	struct SIntersect
	{
		template<typename U>
		friend class Segment_;

		public:
			bool operator()() const
			{
				return _doesIntersect;
			}
			SIntersect()
			{}
/*			explicit SIntersect( const Root<type::IsPoint,FPT>& pti ) : _ptIntersect(pti)
			{
				_doesIntersect = true;
			}*/
			Root<type::IsPoint,FPT>
			get() const
			{
				return _ptIntersect;
			}
		private:
			Root<type::IsPoint,FPT> _ptIntersect;
			bool _doesIntersect = false;
	};

	public:
/// Default constructor: initializes segment to (0,0)--(1,1)
		Segment_(): _ptS2(1.,1.)
		{}
/// Contructor 2: build segment from two points
		Segment_( Root<type::IsPoint,FPT> p1, Root<type::IsPoint,FPT> p2 ):
			_ptS1(p1), _ptS2(p2)
		{
			detail::fix_order( _ptS1, _ptS2 );
		}
/// Setter
		void set( const Root<type::IsPoint,FPT>& p1, const Root<type::IsPoint,FPT>& p2 )
		{
			_ptS1 = p1;
			_ptS2 = p2;
			detail::fix_order( _ptS1, _ptS2 );
		}

/// Get length
		FPT length() const
		{
			return _ptS1.distTo( _ptS2 );
		}
		template<typename U>
		FPT getAngle( const U& other ) const
		{
			return other.getAngle( this->getLine() );
		}
/// Comparison operator
		bool operator == ( const Segment_& s2 ) const
		{
			if( _ptS1 != s2._ptS1 )
				return false;
			if( _ptS2 != s2._ptS2 )
				return false;
			return true;
		}
		bool operator != ( const Segment_& s2 ) const
		{
			return !(*this == s2);
		}
		template<typename U>
		friend std::ostream& operator << ( std::ostream& f, const Segment_<U>& seg )
		{
			f << seg._ptS1 << "-" << seg._ptS2;
			return f;
		}
/// Returns the points as a std::pair
/** The one with smallest x coordinate will be returned as "first". If x-coordinate are equal, then
the one with smallest y-coordinate will be returned first */
		std::pair<Root<type::IsPoint,FPT>,Root<type::IsPoint,FPT>>
		get() const
		{
			return std::make_pair( _ptS1, _ptS2 );
		}

/// Returns supporting line
		Root<type::IsLine,FPT> getLine() const
		{
			return _ptS1 * _ptS2;
		}
		SIntersect intersects( const Segment_<FPT>& ) const;

		template<typename T>
		bool isParallelTo( const T& other ) const
		{
			static_assert(
				std::is_same<T,Segment_<FPT>>::value ||
				std::is_same<T,Root<type::IsLine,FPT>>::value,
				"type needs to be a segment or a line" );
			return getLine().isParallelTo( other );
		}

#ifdef HOMOG2D_USE_OPENCV
	void draw( cv::Mat& mat, CvDrawParams dp=CvDrawParams() )
	{
		cv::line( mat, _ptS1.getCvPtd(), _ptS2.getCvPtd(), dp._dpValues._color, dp._dpValues._lineThickness, dp._dpValues._lineType );
	}
#endif
};

//------------------------------------------------------------------
/// Implementation of line::isParallelTo( Segment )
template<typename LP,typename FPT>
template<typename T>
bool
Root<LP,FPT>::isParallelTo( const Segment_<T>& seg ) const
{
	return impl_isParallelTo( seg.getLine(), detail::RootHelper<LP>() );
}

/// Implementation of line::getAngle()
template<typename LP,typename FPT>
template<typename T>
FPT
Root<LP,FPT>::getAngle( const Segment_<T>& seg ) const
{
	return impl_getAngle( seg.getLine(), detail::RootHelper<LP>() );
}


//------------------------------------------------------------------
namespace detail {

/// Helper function
template<typename T>
bool
isBetween( T v, T v1, T v2 )
{
	if( v >= std::min( v1, v2 ) )
		if( v <= std::max( v1, v2 ) )
			return true;
	return false;
}
} // namespace detail

//------------------------------------------------------------------
/// Computes intersection between 2 segments
/**
Algorithm:<br>
We check if the intersection point lies in between the range of both segments, both on x and on y
*/
template<typename FPT>
typename Segment_<FPT>::SIntersect
Segment_<FPT>::intersects( const Segment_<FPT>& s2 ) const
{
	typename Segment_<FPT>::SIntersect out;
	auto l1 = getLine();
	auto l2 = s2.getLine();
	if( l1.isParallelTo( l2 ) )
		return out;
	out._ptIntersect = l1 * l2;   // intersection point

	const auto& pi   = out._ptIntersect;
	const auto& ptA1 = this->get().first;
	const auto& ptA2 = this->get().second;
	const auto& ptB1 = s2.get().first;
	const auto& ptB2 = s2.get().second;
//	std::cout << "s1=" << *this << " s2=" << s2 << " pi=" << pi << '\n';

	if( detail::isBetween( pi.getX(), ptA1.getX(), ptA2.getX() ) )
		if( detail::isBetween( pi.getY(), ptA1.getY(), ptA2.getY() ) )
			if( detail::isBetween( pi.getX(), ptB1.getX(), ptB2.getX() ) )
				if( detail::isBetween( pi.getY(), ptB1.getY(), ptB2.getY() ) )
					out._doesIntersect = true;
	return out;
}

//------------------------------------------------------------------
/// Overload for points
template<typename LP,typename FPT>
void
Root<LP,FPT>::impl_op_stream( std::ostream& f, const Root<type::IsPoint,FPT>& r ) const
{
	f << '[' << r.getX() << ',' << r.getY() << "] ";
}

/// Overload for lines
template<typename LP,typename FPT>
void
Root<LP,FPT>::impl_op_stream( std::ostream& f, const Root<type::IsLine,FPT>& r ) const
{
	f << '[' << r._v[0] << ',' << r._v[1] << ',' << r._v[2] << "] ";
}

/// Stream operator, free function, call member function pseudo operator impl_op_stream()
template<typename LP,typename FPT>
std::ostream&
operator << ( std::ostream& f, const Root<LP,FPT>& r )
{
	r.impl_op_stream( f, r );
	return f;
}

//------------------------------------------------------------------
/// Normalize to unit length, and make sure \c a is always >0
template<typename LP,typename FPT>
void
Root<LP,FPT>::impl_normalizeLine( const detail::RootHelper<type::IsLine>& ) const
{
	auto sq = std::hypot( _v[0], _v[1] );
	if( sq <= std::numeric_limits<double>::epsilon() )
		throw std::runtime_error( "unable to normalize line, sq=" + std::to_string(sq) );

	for( int i=0; i<3; i++ )
		const_cast<Root<LP,FPT>*>(this)->_v[i] /= sq; // needed to remove constness

	if( std::signbit(_v[0]) ) // a always >0
		for( int i=0; i<3; i++ )
			const_cast<Root<LP,FPT>*>(this)->_v[i] = -_v[i];

	if( _v[0] == 0. ) // then, change sign so that b>0
		if( std::signbit(_v[1]) )
		{
			const_cast<Root<LP,FPT>*>(this)->_v[1] = - _v[1];
			const_cast<Root<LP,FPT>*>(this)->_v[2] = - _v[2];
		}
}

//------------------------------------------------------------------
template<typename LP,typename FPT>
FPT
Root<LP,FPT>::impl_getCoord( En_GivenCoord gc, FPT other, const detail::RootHelper<type::IsLine>& ) const
{
	if( gc == GC_X )
		return ( -_v[0] * other - _v[2] ) / _v[1];
	else
		return ( -_v[1] * other - _v[2] ) / _v[0];
}

template<typename LP,typename FPT>
Root<type::IsPoint,FPT>
Root<LP,FPT>::impl_getPoint( En_GivenCoord gc, FPT other, const detail::RootHelper<type::IsLine>& dummy ) const
{
	auto coord = impl_getCoord( gc, other, dummy );
	if( gc == GC_X )
		return Root<type::IsPoint,FPT>( other, coord );
	return Root<type::IsPoint,FPT>( coord, other );
}

//------------------------------------------------------------------
/// ILLEGAL INSTANCIATION
template<typename LP,typename FPT>
std::pair<Root<type::IsPoint,FPT>,Root<type::IsPoint,FPT>>
Root<LP,FPT>::impl_getPoints( En_GivenCoord, FPT, FPT, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot call getPoints() on a point" );
}

//------------------------------------------------------------------
/// Returns pair of points on line at distance \c dist from point on line at coord \c coord. Implementation for lines
template<typename LP,typename FPT>
std::pair<Root<type::IsPoint,FPT>,Root<type::IsPoint,FPT>>
Root<LP,FPT>::impl_getPoints( En_GivenCoord gc, FPT coord, FPT dist, const detail::RootHelper<type::IsLine>& ) const
{
	auto pt = impl_getPoint( gc, coord, detail::RootHelper<type::IsLine>() );
	auto coeff = dist / std::sqrt( _v[0] * _v[0] + _v[1] * _v[1] );

	Root<type::IsPoint,FPT> pt1(
        pt.getX() -  _v[1] * coeff,
        pt.getY() +  _v[0] * coeff
	);
	Root<type::IsPoint,FPT> pt2(
        pt.getX() +  _v[1] * coeff,
        pt.getY() -  _v[0] * coeff
	);
	detail::fix_order( pt1, pt2 );
	return std::make_pair( pt1, pt2 );
}

//------------------------------------------------------------------
/// Illegal instanciation
template<typename LP,typename FPT>
Root<type::IsLine,FPT>
Root<LP,FPT>::impl_getOrthogonalLine( En_GivenCoord, FPT, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot call getOrthogonalLine() on a point" );
}

/// Returns an orthogonal line, implementation of getOrthogonalLine().
template<typename LP,typename FPT>
Root<type::IsLine,FPT>
Root<LP,FPT>::impl_getOrthogonalLine( En_GivenCoord gc, FPT val, const detail::RootHelper<type::IsLine>& ) const
{
	auto other_val = impl_getCoord( gc, val, detail::RootHelper<type::IsLine>() );

	Root<type::IsPoint,FPT> pt( other_val, val ) ;
	if( gc == GC_X )
		pt.set( val, other_val );

	Root<type::IsLine,FPT> out;
	out._v[0] = -_v[1];
	out._v[1] =  _v[0];
	out._v[2] = _v[1] * pt.getX() - _v[0] * pt.getY();
	out.p_normalizeLine();
	return out;
}

//------------------------------------------------------------------
/// Illegal instanciation
template<typename LP,typename FPT>
Root<type::IsLine,FPT>
Root<LP,FPT>::impl_getParallelLine( const Root<type::IsPoint,FPT>&, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot call getParallelLine() on a point" );
}

/// Returns an parallel line, implementation of getParallelLine().
template<typename LP,typename FPT>
Root<type::IsLine,FPT>
Root<LP,FPT>::impl_getParallelLine( const Root<type::IsPoint,FPT>& pt, const detail::RootHelper<type::IsLine>& ) const
{
	Root<type::IsLine,FPT> out = *this;
	out._v[2] = -_v[0] * pt.getX() - _v[1] * pt.getY();
	out.p_normalizeLine();
	return out;
}

//------------------------------------------------------------------
/// Comparison operator, used for lines
template<typename LP,typename FPT>
bool
Root<LP,FPT>::impl_op_equal( const Root<LP,FPT>& other, const detail::RootHelper<type::IsLine>& ) const
{
	auto eps = std::numeric_limits<double>::epsilon();
	for( int i=0; i<3; i++ )
		if( std::fabs( _v[i] - other._v[i] ) > eps )
			return false;
	return true;
}
/// Comparison operator, used for points
template<typename LP,typename FPT>
bool
Root<LP,FPT>::impl_op_equal( const Root<LP,FPT>& other, const detail::RootHelper<type::IsPoint>& ) const
{
	auto dist = this->distTo( other );
	if( dist < nullDistance() )
		return true;
	return false;
}

//------------------------------------------------------------------
/// Inner implementation details
namespace detail {

	/// Cross product, see https://en.wikipedia.org/wiki/Cross_product#Coordinate_notation
	template<typename Out, typename In,typename FPT>
	Root<Out,FPT> crossProduct( const Root<In,FPT>& r1, const Root<In,FPT>& r2 )
	{
		Root<Out,FPT> res;
		res._v[0] = r1._v[1] * r2._v[2] - r1._v[2] * r2._v[1];
		res._v[1] = r1._v[2] * r2._v[0] - r1._v[0] * r2._v[2];
		res._v[2] = r1._v[0] * r2._v[1] - r1._v[1] * r2._v[0];
//		for ( int i=0; i<3; i++)
//			std::cout << "v[" << i << "]=" << res._v[i] << " ";
//		std::cout  << '\n';
		return res;
	}
} // namespace detail

/// Free function template, product of two lines, returns a point
template<typename FPT>
Root<type::IsPoint,FPT>
operator * ( const Root<type::IsLine,FPT>& lhs, const Root<type::IsLine,FPT>& rhs )
{
#ifdef HOMOG2D_SAFE_MODE
	if( lhs.isParallelTo(rhs) )
		throw std::runtime_error( "lines are parallel, unable to compute product" );
#endif

	auto pt = detail::crossProduct<type::IsPoint,type::IsLine,FPT>(lhs, rhs);
#if 0
	auto eps = std::numeric_limits<double>::epsilon();
	if( std::fabs(pt._v[2]) <= eps )
	{
		std::ostringstream lh,lr;
		lh << lhs;
		lr << rhs;
		throw std::runtime_error( "unable to compute point from two lines: lhs=" + lh.str() + "rhs=" + lr.str() );
	}
#endif
	return pt;
}

/// Free function template, product of two points, returns a line
template<typename FPT>
Root<type::IsLine,FPT>
operator * ( const Root<type::IsPoint,FPT>& lhs, const Root<type::IsPoint,FPT>& rhs )
{
#ifdef HOMOG2D_SAFE_MODE
	if( lhs == rhs )
		throw std::runtime_error( "points are identical, unable to compute product" );
#endif
	Root<type::IsLine, FPT> line = detail::crossProduct<type::IsLine,type::IsPoint,FPT>(lhs, rhs);
	line.p_normalizeLine();
	return line;
}

//------------------------------------------------------------------
///////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////

/// Points overload: generic init from two numeric args
template<typename LP, typename FPT>
template<typename T>
void
Root<LP,FPT>::impl_init_2( const T& v1, const T& v2, const detail::RootHelper<type::IsPoint>& )
{
	_v[0] = v1;
	_v[1] = v2;
	_v[2] = 1.;
}

/// Lines overload: generic init from two numeric args
template<typename LP, typename FPT>
template<typename T>
void
Root<LP,FPT>::impl_init_2( const T& v1, const T& v2, const detail::RootHelper<type::IsLine>& )
{
	Root<type::IsPoint,FPT> pt1;                // 0,0
	Root<type::IsPoint,FPT> pt2(v1,v2);
	*this = detail::crossProduct<type::IsLine>( pt1, pt2 );
	p_normalizeLine();
}

/// overload for point to point distance
template<typename LP, typename FPT>
FPT
Root<LP,FPT>::impl_distToPoint( const Root<type::IsPoint,FPT>& pt, const detail::RootHelper<type::IsPoint>& ) const
{
	return std::hypot( getX() - pt.getX(), getY() - pt.getY() );
}
//------------------------------------------------------------------
/// Returns distance between the line and point \b pt. overload for line to point distance.
/**
http://mathworld.wolfram.com/Point-LineDistance2-Dimensional.html
<pre>
        | a.x0 + b.y0 + c |
  d = -----------------------
         sqrt( a*a + b*b )
</pre>
\todo Do we really require computation of hypot ? (because the line is supposed to be normalized, i.e. h=1 ?)
*/
template<typename LP, typename FPT>
FPT
Root<LP,FPT>::impl_distToPoint( const Root<type::IsPoint,FPT>& pt, const detail::RootHelper<type::IsLine>& ) const
{
	return std::fabs( _v[0] * pt.getX() + _v[1] * pt.getY() + _v[2] ) / std::hypot( _v[0], _v[1] );
}

/// overload for line to point distance
template<typename LP, typename FPT>
FPT
Root<LP,FPT>::impl_distToLine( const Root<type::IsLine,FPT>& li, const detail::RootHelper<type::IsPoint>& ) const
{
//	return li.impl_distToPoint( *this, detail::RootHelper<type::IsLine>() );
	return li.distTo( *this );
}

/// overload for line to line distance. Aborts build if instanciated (distance between two lines makes no sense).
template<typename LP, typename FPT>
FPT
Root<LP,FPT>::impl_distToLine( const Root<type::IsLine,FPT>&, const detail::RootHelper<type::IsLine>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot compute distance between two lines" );
	return 0.;    // to avoid warning message on build
}


/// overload for lines, undefined for points
template<typename LP, typename FPT>
template<typename T>
void
Root<LP,FPT>::impl_addOffset( En_OffsetDir dir, T v, const detail::RootHelper<type::IsLine>& )
{
	if( dir == OD_Vert )
		_v[2] = _v[2] - v*_v[1];
	else
		_v[2] = _v[2] - v*_v[0];
	p_normalizeLine();
}

//------------------------------------------------------------------
/// Free function, returns the angle (in Rad) between two lines.
template<typename FPT>
FPT
getAngle( const Root<type::IsLine,FPT>& li1, const Root<type::IsLine,FPT>& li2 )
{
	return li1.getAngle( li2 );
}

template<typename LP, typename FPT>
bool
Root<LP,FPT>::impl_isParallelTo( const Root<LP,FPT>& li, const detail::RootHelper<type::IsLine>& ) const
{
	if( getAngle(li) < Root::_zeroAngleValue )
		return true;
	return false;
}
template<typename LP, typename FPT>
bool
Root<LP,FPT>::impl_isParallelTo( const Root<LP,FPT>& li, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot use IsParallel() with a point" );
	return false;    // to avoid a warning message on build
}

//------------------------------------------------------------------
/// Returns the angle (in Rad) between the line and another one.
/**
Will return a value in the range [0,M_PI/2]

\todo add implementation of free function with partial specialization trick
*/
template<typename LP, typename FPT>
FPT
Root<LP,FPT>::impl_getAngle( const Root<LP,FPT>& li, const detail::RootHelper<type::IsLine>& ) const
{
	double res = _v[0] * li._v[0] + _v[1] * li._v[1];
	res /= std::sqrt( _v[0]*_v[0] + _v[1]*_v[1] ) * std::sqrt( li._v[0]*li._v[0] + li._v[1]*li._v[1] );
	return std::acos( std::abs(res) );
}

template<typename LP, typename FPT>
FPT
Root<LP,FPT>::impl_getAngle( const Root<LP,FPT>& li, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "cannot get angle of a point" );
	return 0.; // to avoid a warning
}


//------------------------------------------------------------------
/// Returns true if point is inside (or on the edge) of a flat rectangle defined by (p0,p1)
template<typename LP, typename FPT>
bool
Root<LP,FPT>::impl_isInsideRectangle( const Root<type::IsPoint,FPT>& p0, const Root<type::IsPoint,FPT>& p1, const detail::RootHelper<type::IsPoint>& ) const
{
	auto pair_pts = detail::getCorrectPoints( p0, p1 );
	const auto& p00 = pair_pts.first;
	const auto& p11 = pair_pts.second;
	return detail::ptIsInside( *this, p00, p11 );
}
template<typename LP, typename FPT>
bool
Root<LP,FPT>::impl_isInsideRectangle( const Root<type::IsPoint,FPT>& p0, const Root<type::IsPoint,FPT>& p1, const detail::RootHelper<type::IsLine>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "cannot use isInsideRectangle() with a line" );
	return false; // to avoid a warning
}

template<typename LP, typename FPT>
template<typename T>
bool
Root<LP,FPT>::impl_isInsideCircle( const Root<type::IsPoint,FPT>& center, T radius, const detail::RootHelper<type::IsPoint>& ) const
{
	if( distTo( center ) <= radius )
		return true;
	return false;
}

template<typename LP, typename FPT>
template<typename T>
bool
Root<LP,FPT>::impl_isInsideCircle( const Root<type::IsPoint,FPT>& center, T radius, const detail::RootHelper<type::IsLine>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "cannot use isInsideCircle() with a line" );
	return false; // to avoid a warning
}

//------------------------------------------------------------------
/// Intersection of line and circle
template<typename LP, typename FPT>
template<typename T>
typename Root<LP,FPT>::Intersect
Root<LP,FPT>::intersectsCircle( const Root<type::IsPoint,FPT>& pt, T radius ) const
{
	return impl_intersectsCircle( pt, radius, detail::RootHelper<type::IsLine>() );
}

//------------------------------------------------------------------
/// Intersection of line and circle: implementation for points
template<typename LP, typename FPT>
template<typename T>
typename Root<LP,FPT>::Intersect
Root<LP,FPT>::impl_intersectsCircle(
	const Root<type::IsPoint,FPT>& pt,       ///< circle origin
	T                              r,        ///< radius
	const detail::RootHelper<type::IsPoint>&  ///< dummy arg
) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "cannot use intersectsCircle() with a point" );
}
//------------------------------------------------------------------
/// Intersection of line and circle: implementation
/// For computation details, checkout http://skramm.lautre.net/files/misc/intersect_circle_line.pdf
template<typename LP, typename FPT>
template<typename T>
typename Root<LP,FPT>::Intersect
Root<LP,FPT>::impl_intersectsCircle(
	const Root<type::IsPoint,FPT>& pt,       ///< circle origin
	T                              r,        ///< radius
	const detail::RootHelper<type::IsLine>&  ///< dummy arg, needed so that this overload is only called for lines
) const
{
	Intersect out;
	HOMOG2D_CHECK_IS_NUMBER(T);
	const FPT& a = _v[0]; // just to lighten a bit...
	const FPT& b = _v[1];
	const FPT& c = _v[2];

// step 1: translate to origin
	double cp = pt.getX() * a + pt.getY() * b + c;

// step 2: compute distance	between center (origin) and middle point
	double a2b2 = a * a + b * b;
	double d0 = std::abs(cp) / std::sqrt( a2b2 );
	if( r < d0 )                            // if less than radius,
		return out;                         // no intersection

	double d2 = r*r - d0*d0;

// step 3: compute coordinates of middle point B
	double xb = - a * cp / a2b2;
	double yb = - b * cp / a2b2;

// step 4: compute coordinates of intersection points, with center at (0,0)
	double m  = std::sqrt( d2 / a2b2 );
	double x1 = xb + m*b;
	double y1 = yb - m*a;

	double x2 = xb - m*b;
	double y2 = yb + m*a;

// last step: translate back
	out.ptA.set( x1 + pt.getX(), y1 + pt.getY() );
	out.ptB.set( x2 + pt.getX(), y2 + pt.getY() );
	out._doesIntersect = true;

	detail::fix_order( out.ptA, out.ptB );
	return out;
}
//------------------------------------------------------------------
/// Checks for intersection with flat rectangle defined by the two points \c p0 and \c p1
/**
Pre-conditions: points are different (throws if not)
*/
template<typename LP, typename FPT>
typename Root<LP,FPT>::Intersect
Root<LP,FPT>::intersectsRectangle( const Root<type::IsPoint,FPT>& p0, const Root<type::IsPoint,FPT>& p1 ) const
{
	return impl_intersectsRectangle( p0, p1, detail::RootHelper<LP>() );
}

/// Overload used when attempting to use that on a point
template<typename LP, typename FPT>
typename Root<LP,FPT>::Intersect
Root<LP,FPT>::impl_intersectsRectangle( const Root<type::IsPoint,FPT>& p0, const Root<type::IsPoint,FPT>& p1, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot call intersectsRectangle() on a point" );
}

/// Checks for intersection with flat rectangle defined by the two points p00 and p11: implementation
template<typename LP, typename FPT>
typename Root<LP,FPT>::Intersect
Root<LP,FPT>::impl_intersectsRectangle( const Root<type::IsPoint,FPT>& p0, const Root<type::IsPoint,FPT>& p1, const detail::RootHelper<type::IsLine>& ) const
{
	auto pair_pts = detail::getCorrectPoints( p0, p1 );
	const auto& p00 = pair_pts.first;
	const auto& p11 = pair_pts.second;

	Root<type::IsPoint,FPT> p01( p11.getX(), p00.getY() );
	Root<type::IsPoint,FPT> p10( p00.getX(), p11.getY() );

	Root<type::IsLine,FPT> l[4];
	l[0] = Root<type::IsLine,FPT>( p00, p01 );
	l[1] = Root<type::IsLine,FPT>(      p01, p11 );
	l[2] = Root<type::IsLine,FPT>(           p11, p10 );
	l[3] = Root<type::IsLine,FPT>(                p10, p00 );

	std::vector<Root<type::IsPoint,FPT>> vec;
	for( int i=0; i<4; i++ )         // compare with each of the 4 lines
	{
		if( *this == l[i] )          // if same line, then we are done: return the two points
			return typename Root<type::IsLine,FPT>::Intersect( p00, p11 );
		else
		{
			Root<type::IsPoint,FPT> pt;
			bool okFlag(true);
			try{
				pt = *this * l[i];
			}
			catch( const std::exception& )
			{
				okFlag = false; // lines are parallel
			}
			if( okFlag )
			{
				if( detail::ptIsInside( pt, p00, p11 ) )
					vec.push_back( pt );
			}
		}
	}

	typename Root<LP,FPT>::Intersect out;
	if( vec.size() > 1 )                                // if more than one point was found, then
	{
		std::vector<Root<type::IsPoint,FPT>> vec2( 1, vec[0] );   // build a second vector, holding the first found point as first element
		for( size_t i=1; i<vec.size(); i++ )            // and add the other points, only once
		{
			if(
				std::find( std::begin( vec2 ), std::end( vec2 ), vec[i] ) // if not already stored,
				== std::end( vec2 )
			)
				vec2.push_back( vec[i] );                                 // then, add the point
		}

		if( vec2.size() > 1 )
		{
			out._doesIntersect = true;
			out.ptA = vec2[0];
			out.ptB = vec2[1];
		}
		detail::fix_order( out.ptA, out.ptB );
	}
	return out;
}

//------------------------------------------------------------------
namespace detail {

/// Implementation of product 3x3 by 3x1
template<typename T1,typename T2,typename U,typename FPT1,typename FPT2>
void
product(
	Root<T1,FPT1>&          out,
	const Hmatrix_<U,FPT2>& h,
	const Root<T2,FPT1>&    in
)
{
	for( int i=0; i<3; i++ )
	{
		out._v[i]  = h._data[i][0] * in._v[0];
		out._v[i] += h._data[i][1] * in._v[1];
		out._v[i] += h._data[i][2] * in._v[2];
	}
}

} // namespace detail

/// Apply Hmatrix to a point or line. Free function, templated by point or line
template<typename T,typename U,typename V>
Root<typename detail::HelperPL<T>::OtherType,V>
operator * ( const Hmatrix_<type::IsMatrix,U>& h, const Root<T,V>& in )
{
	Root<typename detail::HelperPL<T>::OtherType,V> out;
	detail::product( out, h, in );
	return out;
}

/// Apply homography to a point or line. Free function, templated by point or line
template<typename T,typename U,typename V>
Root<T,V>
operator * ( const Hmatrix_<type::IsHomogr,U>& h, const Root<T,V>& in )
{
	Root<T,V> out;
	detail::product( out, h, in );
	return out;
}

/// Apply homography to a segment
template<typename FPT1,typename FPT2>
Segment_<FPT1>
operator * ( const Hmatrix_<type::IsHomogr,FPT2>& h, const Segment_<FPT1>& seg )
{
	const auto& pts = seg.get();
	Root<type::IsPoint,FPT1> pt1 = h * pts.first;
	Root<type::IsPoint,FPT1> pt2 = h * pts.second;
	return Segment_<FPT1>( pt1, pt2 );

}
//------------------------------------------------------------------
/// Apply homography to a vector/array/list (type T) of points or lines.
template<typename W,typename FPT>
template<typename T>
void
Hmatrix_<W,FPT>::applyTo( T& vin ) const
{
	for( auto& elem: vin )
		elem = *this * elem;
}

//------------------------------------------------------------------
#ifdef HOMOG2D_USE_OPENCV
/// Return Opencv 2D point
template<typename LP, typename FPT>
template<typename OPENCVT>
OPENCVT
Root<LP,FPT>::impl_getCvPt( const detail::RootHelper<type::IsPoint>&, const OPENCVT& ) const
{
	return OPENCVT( getX(),getY() );
}


//------------------------------------------------------------------
/// Copy matrix to Opencv \c cv::Mat
/**
The output matrix is passed by reference to avoid issues with Opencv copy operator, and is allocated here.

User can pass a type as second argument: CV_32F for \c float, CV_64F for \c double (default)
*/
template<typename W,typename FPT>
void
Hmatrix_<W,FPT>::copyTo( cv::Mat& mat, int type ) const
{
#ifdef HOMOG2D_SAFE_MODE
	if( type != CV_64F && type != CV_32F )
		throw std::runtime_error( "invalid OpenCv matrix type" );
#endif
	mat.create( 3, 3, type ); // default:CV_64F
	size_t i=0;
	switch( type )
	{
		case CV_64F:
			for( auto it = mat.begin<double>(); it != mat.end<double>(); it++, i++ )
				*it = _data[i/3][i%3];
			break;
		case CV_32F:
			for( auto it = mat.begin<float>(); it != mat.end<float>(); it++, i++ )
				*it = _data[i/3][i%3];
			break;
		default: assert(0);
	}
}
//------------------------------------------------------------------
/// Get homography from Opencv \c cv::Mat
template<typename W,typename FPT>
Hmatrix_<W,FPT>&
Hmatrix_<W,FPT>::operator = ( const cv::Mat& mat )
{
#ifdef HOMOG2D_SAFE_MODE
	if( mat.rows != 3 || mat.cols != 3 )
		throw std::runtime_error( "invalid matrix size, rows=" + std::to_string(mat.rows) + " cols=" + std::to_string(mat.cols) );
	if( mat.channels() != 1 )
		throw std::runtime_error( "invalid matrix nb channels: " + std::to_string(mat.channels() ) );
#endif
	auto type = mat.type();
#ifdef HOMOG2D_SAFE_MODE
	if( type != CV_64F && type != CV_32F )
		throw std::runtime_error( "invalid matrix type" );
#endif
	size_t i=0;
	switch( type )
	{
		case CV_64F:
			for( auto it = mat.begin<double>(); it != mat.end<double>(); it++, i++ )
				_data[i/3][i%3] = *it;
			break;
		case CV_32F:
			for( auto it = mat.begin<float>(); it != mat.end<float>(); it++, i++ )
				_data[i/3][i%3] = *it;
			break;
		default: assert(0);
	}
	return *this;
}

//------------------------------------------------------------------
namespace detail {
/// Private helper function, used by Root<IsPoint>::draw()
void
drawPt( cv::Mat& mat, PointStyle ps, std::vector<cv::Point2d> vpt, const CvDrawParams& dp, bool drawDiag=false )
{
	auto delta  = dp._dpValues._ptDelta;
	auto delta2 = std::round( 0.85 * delta);
	switch( ps )
	{
		case PS_TIMES:
			vpt[0].x -= delta2;
			vpt[0].y += delta2;
			vpt[1].x += delta2;
			vpt[1].y -= delta2;

			vpt[2].x += delta2;
			vpt[2].y += delta2;
			vpt[3].x -= delta2;
			vpt[3].y -= delta2;
		break;

		case PS_PLUS:
		case PS_DIAM:
			vpt[0].x -= delta;
			vpt[1].x += delta;
			vpt[2].y -= delta;
			vpt[3].y += delta;
		break;
		default: assert(0);
	}
	if( !drawDiag )
	{
		cv::line( mat, vpt[0], vpt[1], dp._dpValues._color, dp._dpValues._enhancePoint?2:1 );
		cv::line( mat, vpt[2], vpt[3], dp._dpValues._color, dp._dpValues._enhancePoint?2:1 );
	}
	else // draw 4 diagonal lines
	{
		cv::line( mat, vpt[0], vpt[2], dp._dpValues._color, dp._dpValues._enhancePoint?2:1 );
		cv::line( mat, vpt[2], vpt[1], dp._dpValues._color, dp._dpValues._enhancePoint?2:1 );
		cv::line( mat, vpt[1], vpt[3], dp._dpValues._color, dp._dpValues._enhancePoint?2:1 );
		cv::line( mat, vpt[0], vpt[3], dp._dpValues._color, dp._dpValues._enhancePoint?2:1 );
	}
}
} // namespace detail
//------------------------------------------------------------------
/// Draw Point on Cv::Mat. Overload for points.
/// Returns false if point not in image
template<typename LP, typename FPT>
bool
Root<LP,FPT>::impl_draw( cv::Mat& mat, const CvDrawParams& dp, const detail::RootHelper<type::IsPoint>& /* dummy */ )  const
{
	if( getX()<0 || getX()>=mat.cols )
		return false;
	if( getY()<0 || getY()>=mat.rows )
		return false;

	std::vector<cv::Point2d> vpt( 4, getCvPtd() );
	switch( dp._dpValues._ptStyle )
	{
		case PS_PLUS:   // "+" symbol
			detail::drawPt( mat, PS_PLUS,  vpt, dp );
		break;

		case PS_STAR:
			detail::drawPt( mat, PS_PLUS,  vpt, dp );
			detail::drawPt( mat, PS_TIMES, vpt, dp );
		break;

		case PS_DIAM:
			detail::drawPt( mat, PS_PLUS,  vpt, dp, true );
		break;

		case PS_TIMES:      // "times" symbol
			detail::drawPt( mat, PS_TIMES, vpt, dp );
		break;

		default: assert(0);
	}
	return true;
}
//------------------------------------------------------------------
/// Draw line on Cv::Mat. Overload for lines.
/**
Returns false if line is not in image.

Steps:
 -# builds the 4 corner points of the image
 -# build the 4 corresponding lines (borders of the image)
 -# find the intersection points between the line and these 4 lines. Should find 2
 -# draw a line between these 2 points
*/
template<typename LP, typename FPT>
bool
Root<LP,FPT>::impl_draw( cv::Mat& mat, const CvDrawParams& dp, const detail::RootHelper<type::IsLine>& /* dummy */ ) const
{
	assert( mat.rows > 2 );
	assert( mat.cols > 2 );

	Root<type::IsPoint,FPT> pt1; // 0,0
	Root<type::IsPoint,FPT> pt2( mat.cols-1, mat.rows-1 );
    Intersect ri = this->intersectsRectangle( pt1,  pt2 );
    if( ri() )
    {
		cv::Point2d ptcv1 = ri.ptA.getCvPtd();
		cv::Point2d ptcv2 = ri.ptB.getCvPtd();
		cv::line( mat, ptcv1, ptcv2, dp._dpValues._color, dp._dpValues._lineThickness, dp._dpValues._lineType );
		return true;
	}
	return false;
}

//------------------------------------------------------------------
/// Free function, draws a set of points or lines
/**
Template type can be std::array<type> or std::vector<type>, with \c type being Point2d or \c Line2d
*/
template<typename T>
void draw( cv::Mat& mat, const T& cont, const CvDrawParams& dp=CvDrawParams() )
{
	for( const auto& elem: cont )
		elem.draw( mat, dp );
}

//------------------------------------------------------------------
#endif // HOMOG2D_USE_OPENCV

/// Default line type, uses \c double as numerical type
using Line2d = Root<type::IsLine,double>;

/// Default point type, uses \c double as numerical type
using Point2d = Root<type::IsPoint,double>;

/// Default homography (3x3 matrix) type, uses \c double as numerical type
using Homogr = Hmatrix_<type::IsHomogr,double>;

/// Default homogeneous matrix, uses \c double as numerical type
using Hmatrix = Hmatrix_<type::IsMatrix,double>;

/// Default segment type
using Segment = Segment_<double>;

// float types
using Line2dF  = Root<type::IsLine,float>;
using Point2dF = Root<type::IsPoint,float>;
using HomogrF  = Hmatrix_<type::IsHomogr,float>;
using SegmentF = Segment_<float>;

// double types
using Line2dD  = Root<type::IsLine,double>;
using Point2dD = Root<type::IsPoint,double>;
using HomogrD  = Hmatrix_<type::IsHomogr,double>;
using SegmentD = Segment_<double>;

// long double types
using Line2dL  = Root<type::IsLine,long double>;
using Point2dL = Root<type::IsPoint,long double>;
using HomogrL  = Hmatrix_<type::IsHomogr,long double>;
using SegmentL = Segment_<long double>;

template<typename T>
using Point2d_ = Root<type::IsPoint,T>;
template<typename T>
using Line2d_  = Root<type::IsLine,T>;

template<typename T>
using Intersect_ = typename Root<type::IsLine,T>::Intersect;

using Intersect = Intersect_<double>;

} // namespace homog2d end


#endif // HG_HOMOG2D_HPP

