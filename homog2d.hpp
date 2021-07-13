/**************************************************************************

    This file is part of the C++ library "homog2d", dedicated to
    handle 2D lines and points, see https://github.com/skramm/homog2d

    Author & Copyright 2019-2021 Sebastien Kramm

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
#include <numeric>
#include <set>
#include <list>
#include <vector>
#include <iomanip>
#include <cassert>
#include <sstream>
#include <type_traits>

#ifdef HOMOG2D_USE_EIGEN
	#include <Eigen/Dense>
#endif

#define HOMOG2D_VERSION 2.5

#ifdef HOMOG2D_USE_OPENCV
	#include "opencv2/imgproc.hpp"
#endif

#if 0
	#define HOMOG2D_START std::cout << "START: " << __PRETTY_FUNCTION__ << "()\n"
	#define HOMOG2D_LOG(a) std::cout << " - line " << __LINE__ << ": " << a << '\n'
#else
	#define HOMOG2D_START
	#define HOMOG2D_LOG(a) {;}
#endif

/// Assert debug macro, used internally if \c HOMOG2D_DEBUGMODE is defined
#define HOMOG2D_DEBUG_ASSERT(a,b) \
	{ \
		if( (a) == false ) \
		{ \
			std::cerr << "Homog2d assert failure, version:" << HOMOG2D_VERSION \
				<< ", line:" << __LINE__ << "\n -details: " << b << '\n'; \
			std::cout << "homog2d: internal failure, please check stderr and report this\n"; \
			std::exit(1); \
		} \
	}

#define HOMOG2D_CHECK_ROW_COL \
	if( r > 2 ) \
		throw std::runtime_error( "Error: invalid row value: r=" + std::to_string(r) ); \
	if( c > 2 ) \
		throw std::runtime_error( "Error: invalid col value: r=" + std::to_string(r) )

#define HOMOG2D_CHECK_IS_NUMBER(T) \
	static_assert( std::is_arithmetic<T>::value && !std::is_same<T, bool>::value, "Type of value must be numerical" )

/// Internal type used for numerical computations, possible	values: \c double, <code>long double</code>
#if !defined(HOMOG2D_INUMTYPE)
	#define HOMOG2D_INUMTYPE double
#endif

/// Error throw wrapper macro
#define HOMOG2D_THROW_ERROR_1( msg ) \
	throw std::runtime_error( std::string("homog2d: line ") + std::to_string( __LINE__ ) + ", " + std::string(__FUNCTION__) + "(): " + msg )

/// Error throw wrapper macro
#define HOMOG2D_THROW_ERROR_2( f, msg ) \
	throw std::runtime_error( std::string("homog2d: line ") + std::to_string( __LINE__ ) + ", " + f + "(): " + msg )

namespace h2d {

/// Holds the types needed for policy based design
namespace type {

struct IsLine   {};
struct IsPoint  {};
struct IsHomogr {};
struct IsEpipmat {};

} // namespace type


namespace detail {

	/// Helper class for Root (Point/Line) type, used as a trick to allow partial specialization of member functions
	template<typename> struct RootHelper {};

	/// Helper class for Root (Point/Line) type, used only to get the underlying floating-point type, see Dtype and Root::dtype()
	template<typename> struct RootDataType {};

#if 0
	/// Helper class for Matrix type
	template<typename T1>
	struct HelperMat {};

	template<>
	struct HelperMat<type::IsHomogr>
	{
		using M_OtherType = IsEpipmat;
	};

	template<>
	struct HelperMat<type::IsEpipmat>
	{
		using M_OtherType = IsHomogr;
	};
#endif

    template<typename> struct HelperPL;

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

//	template<typename> class Matrix_;

	/// A trick used in static_assert, so it aborts only if function is instanciated
	template<typename T>
	struct AlwaysFalse {
		enum { value = false };
	};

} // namespace detail


// forward declarations
template<typename LP,typename FPT>
class Root;

template<typename LP,typename FPT>
class Hmatrix_;

template<typename T>
using Homogr_  =  Hmatrix_<type::IsHomogr,T>;
template<typename T>
using Epipmat_ =  Hmatrix_<type::IsEpipmat,T>;

template<typename FPT> class Segment_;
template<typename FPT> class Polyline_;
template<typename FPT> class Circle_;
template<typename FPT> class FRect_;

template<typename T>
using Point2d_ = Root<type::IsPoint,T>;
template<typename T>
using Line2d_  = Root<type::IsLine,T>;


namespace detail {

template<typename FPT>
using Matrix_ = std::array<std::array<FPT,3>,3> ;

/// forward declaration of function
template<typename T1,typename T2,typename FPT1,typename FPT2>
void
product(
	Root<T1,FPT1>&        out,
	const Matrix_<FPT2>&  h,
	const Root<T2,FPT1>&  in
);

//------------------------------------------------------------------
/// Private free function, get top-left and bottom-right points from two arbitrary points
template<typename FPT>
std::pair<Point2d_<FPT>,Point2d_<FPT>>
getCorrectPoints( const Point2d_<FPT>& p0, const Point2d_<FPT>& p1 )
{
#ifndef HOMOG2D_NOCHECKS
	if(
		   std::fabs( p0.getX() - p1.getX() ) < Point2d_<FPT>::nullOrthogDistance()
		|| std::fabs( p0.getY() - p1.getY() ) < Point2d_<FPT>::nullOrthogDistance()
	)
		throw std::runtime_error(
			std::string("error: a coordinate of the 2 points is identical, does not define a rectangle: \n ")
			+ " p0.getX()=" + std::to_string(p0.getX()) + " p1.getX()=" + std::to_string(p1.getX())
			+ " p0.getY()=" + std::to_string(p0.getY()) + " p1.getY()=" + std::to_string(p1.getY())
		);
#endif
	Point2d_<FPT> p00( std::min(p0.getX(), p1.getX()), std::min(p0.getY(), p1.getY()) );
	Point2d_<FPT> p11( std::max(p0.getX(), p1.getX()), std::max(p0.getY(), p1.getY()) );
	return std::make_pair( p00, p11 );
}

template<typename FPT1, typename FPT2>
void divideAll( detail::Matrix_<FPT1>& mat, FPT2 value )
{
	for( int i=0; i<3; i++ )
		for( int j=0; j<3; j++ )
			mat[i][j] /= value;
}

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

Templated by Floating-Point Type (FPT) and by type M (type::IsEpipmat or type::IsHomogr)
 */
template<typename M,typename FPT>
class Hmatrix_
{
	template<typename T1,typename T2> friend class Root;

	template<typename T1,typename T2,typename FPT1,typename FPT2>
	friend void
	detail::product( Root<T1,FPT1>&, const detail::Matrix_<FPT2>&, const Root<T2,FPT1>& );

	template<typename T,typename U>
	friend Line2d_<T>
	operator * ( const Homogr_<U>&, const Line2d_<T>& );

	template<typename T,typename U>
	friend Point2d_<T>
	operator * ( const Homogr_<U>&, const Point2d_<T>& );

	template<typename T,typename U,typename V>
	friend Root<typename detail::HelperPL<T>::OtherType,V>
	operator * ( const Hmatrix_<type::IsEpipmat,U>& h, const Root<T,V>& in );

public:
	/// Default constructor, initialize to unit transformation
	Hmatrix_()
	{
		init();
	}

/// Constructor, set homography to a rotation matrix of angle \c val
	template<typename T>
	Hmatrix_( T val )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		init();
		setRotation( val );
	}

/// Constructor, set homography to a translation matrix ( see Hmatrix_( T ) )
	template<typename T>
	Hmatrix_( T tx, T ty )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		init();
		setTranslation( tx, ty );
	}

private:
	void init()
	{
		impl_mat_init0( detail::RootHelper<M>() );
	}

/// Implementation for epipolar matrices: initialize to aligned axis
	void impl_mat_init0( const detail::RootHelper<type::IsEpipmat>& )
	{
		p_zero();
		_data[2][1] = 1.;
		_data[1][2] = 1.;
		_isNormalized = true;
	}

/// Implementation for homographies: initialize to unit transformation
	void impl_mat_init0( const detail::RootHelper<type::IsHomogr>& )
	{
		p_zero();
		_data[0][0] = 1.;
		_data[1][1] = 1.;  // "eye" matrix => unit transformation
		_data[2][2] = 1.;
		_isNormalized = true;
	}

public:

/// Constructor, used to fill with a vector of vector matrix
/** \warning
- Input matrix \b must be 3 x 3, but type can be anything that can be copied to \c double
- no checking is done on validity of matrix as an homography.
Thus some assert can get triggered elsewhere.
*/
	template<typename T>
	Hmatrix_( const std::vector<std::vector<T>>& in )
	{
#ifndef HOMOG2D_NOCHECKS
		HOMOG2D_CHECK_IS_NUMBER(T);
		if( in.size() != 3 )
			HOMOG2D_THROW_ERROR_1( "Invalid line size for input: " + std::to_string(in.size()) );
		for( auto li: in )
			if( li.size() != 3 )
				HOMOG2D_THROW_ERROR_1( "Invalid column size for input: " + std::to_string(li.size()) );
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

/// Assignment operator
	Hmatrix_& operator = ( const Hmatrix_<M,FPT>& other )
	{
		_data         = other._data;
		_hasChanged   = true;
		_isNormalized = false;
		_hmt          = nullptr;
		return *this;
	}

/// Copy-constructor
	Hmatrix_( const Hmatrix_<M,FPT>& other )
		:_data          ( other._data )
		, _hasChanged   ( true )
		, _isNormalized ( other._isNormalized )
		, _hmt          (  nullptr )
	{}

/// Setter \warning No normalization is done, as this can be done
/// several times to store values, we therefore must not normalize in between
	template<typename T>
	void set(
		size_t r, ///< row
		size_t c, ///< col
		T      v  ///< value
	)
	{
		#ifndef HOMOG2D_NOCHECKS
			HOMOG2D_CHECK_ROW_COL;
		#endif
		_data[r][c] = v;
		_isNormalized = false;
		_hasChanged = true;
	}
/// Getter
	FPT get( size_t r, size_t c ) const
	{
		#ifndef HOMOG2D_NOCHECKS
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
		return *this;
	}
/// Sets the matrix as a translation \c tx,ty
	template<typename T1,typename T2>
	Hmatrix_& setTranslation( T1 tx, T2 ty )
	{
		HOMOG2D_CHECK_IS_NUMBER(T1);
		HOMOG2D_CHECK_IS_NUMBER(T2);
		init();
		_data[0][2] = tx;
		_data[1][2] = ty;
		_isNormalized = true;
		_hasChanged = true;
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
		_hasChanged = true;
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
	template<typename T1,typename T2>
	Hmatrix_& addScale( T1 kx, T2 ky )
	{
		HOMOG2D_CHECK_IS_NUMBER(T1);
		HOMOG2D_CHECK_IS_NUMBER(T2);
		Hmatrix_ out;
		out.setScale( kx, ky );
		*this = out * *this;
		normalize();
		_hasChanged = true;
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
	template<typename T1,typename T2>
	Hmatrix_& setScale( T1 kx, T2 ky )
	{
		HOMOG2D_CHECK_IS_NUMBER(T1);
		HOMOG2D_CHECK_IS_NUMBER(T2);
		init();
		_data[0][0] = kx;
		_data[1][1] = ky;
		_isNormalized = true;

		_hasChanged = true;
		return *this;
	}

	template<typename T>
	void
	applyTo( T& ) const;

#ifdef HOMOG2D_USE_OPENCV
	void copyTo( cv::Mat&, int type=CV_64F ) const;
	Hmatrix_& operator = ( const cv::Mat& );
#endif

/// Matrix normalisation
	void normalize() const
	{
		auto eps = std::numeric_limits<FPT>::epsilon()*10;

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
		_hasChanged = true;
		_isNormalized = true;
	}

private:
	detail::Matrix_<FPT> p_transpose( const detail::Matrix_<FPT>& m_in ) const
	{
		detail::Matrix_<FPT> m_out;
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				m_out[i][j] = m_in[j][i];
		return m_out;
	}

	detail::Matrix_<FPT> p_inverse() const // const detail::Matrix_<FPT>& m_in ) const
	{
		auto det = p_det();
		if( std::abs(det) <= Hmatrix_<M,FPT>::nullDeterValue() )
			throw std::runtime_error( "matrix is not invertible" );

		auto adjugate = p_adjugate();
		detail::divideAll(adjugate, det);
		return adjugate;
	}

public:
/// Transpose matrix
	Hmatrix_& transpose()
	{
		detail::Matrix_<FPT> out = p_transpose( _data );
		_data = out;
		_hasChanged = true;
		return *this;
	}

/// Inverse matrix
	Hmatrix_& inverse()
	{
		_data = p_inverse();
		normalize();
		_hasChanged = true;
		return *this;
	}

	void buildFrom4Points( const std::vector<Point2d_<FPT>>&, const std::vector<Point2d_<FPT>>&, int method=1 );

/// Matrix multiplication
	friend Hmatrix_ operator * ( const Hmatrix_& h1, const Hmatrix_& h2 )
	{
		Hmatrix_ out;
		out.p_zero();
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				for( int k=0; k<3; k++ )
					out._data[i][j] +=
						static_cast<HOMOG2D_INUMTYPE>(   h1._data[i][k] )
						* static_cast<HOMOG2D_INUMTYPE>( h2._data[k][j] );
		out.normalize();
		out._hasChanged = true;
		return out;
	}

	public:
/// Comparison operator. Does normalization if required
	bool operator == ( const Hmatrix_& h ) const
	{
		if( !_isNormalized )
			normalize();
		if( !h._isNormalized )
			h.normalize();

		auto eps = std::numeric_limits<FPT>::epsilon();
		for( int i=0; i<3; i++ )
			for( int j=0; j<3; j++ )
				if( std::fabs(
					static_cast<HOMOG2D_INUMTYPE>( _data[i][j] ) - static_cast<HOMOG2D_INUMTYPE>( h._data[i][j] ) )
					>= eps
				)
					return false;
		return true;
	}
/// Comparison operator. Does normalization if required
	bool operator != ( const Hmatrix_& h ) const
	{
		return !(*this == h);
	}
	static HOMOG2D_INUMTYPE& nullDeterValue() { return _zeroDeterminantValue; }

//////////////////////////
//   PRIVATE FUNCTIONS  //
//////////////////////////
	private:
	void p_zero()
	{
		for( auto& li: _data )
			for( auto& e: li )
				e = 0.;
		_hasChanged = true;
	}
	template<typename T>
	void p_fillWith( const T& in )
	{
		for( auto i=0; i<3; i++ )
			for( auto j=0; j<3; j++ )
				_data[i][j] = in[i][j];
		normalize();
		_hasChanged = true;
	}
	void p_divideBy( size_t r, size_t c ) const
	{
		assert( std::fabs( _data[r][c] ) > std::numeric_limits<FPT>::epsilon() );
		for( auto& li: _data )
			for( auto& e: li )
				e /= _data[r][c];
		_hasChanged = true;
	}

/// Return determinant of matrix
/**
See https://en.wikipedia.org/wiki/Determinant
*/
	HOMOG2D_INUMTYPE p_det() const
	{
		auto det = _data[0][0] * p_det2x2( {1,1, 1,2, 2,1, 2,2} );
		det     -= _data[0][1] * p_det2x2( {1,0, 1,2, 2,0, 2,2} );
		det     += _data[0][2] * p_det2x2( {1,0, 1,1, 2,0, 2,1} );
		return det;
	}
	HOMOG2D_INUMTYPE p_det2x2( const std::vector<int>& v ) const
	{
		auto det = static_cast<HOMOG2D_INUMTYPE>( _data[v[0]][v[1]] ) * _data[v[6]][v[7]];
		det -=     static_cast<HOMOG2D_INUMTYPE>( _data[v[2]][v[3]] ) * _data[v[4]][v[5]];
		return det;
	}
/// Computes adjugate matrix, see https://en.wikipedia.org/wiki/Adjugate_matrix#3_%C3%97_3_generic_matrix
	detail::Matrix_<FPT> p_adjugate() const
	{
		detail::Matrix_<FPT> out;

		out[ 0 ][ 0 ] =  p_det2x2( {1,1, 1,2, 2,1, 2,2} );
		out[ 0 ][ 1 ] = -p_det2x2( {0,1, 0,2, 2,1, 2,2} );
		out[ 0 ][ 2 ] =  p_det2x2( {0,1, 0,2, 1,1, 1,2} );

		out[ 1 ][ 0 ] = -p_det2x2( {1,0, 1,2, 2,0, 2,2} );
		out[ 1 ][ 1 ] =  p_det2x2( {0,0, 0,2, 2,0, 2,2} );
		out[ 1 ][ 2 ] = -p_det2x2( {0,0, 0,2, 1,0, 1,2} );

		out[ 2 ][ 0 ] =  p_det2x2( {1,0, 1,1, 2,0, 2,1} );
		out[ 2 ][ 1 ] = -p_det2x2( {0,0, 0,1, 2,0, 2,1} );
		out[ 2 ][ 2 ] =  p_det2x2( {0,0, 0,1, 1,0, 1,1} );

		return out;
	}

//////////////////////////
//      DATA SECTION    //
//////////////////////////
	private:
	mutable detail::Matrix_<FPT> _data;
	mutable bool _hasChanged   = true;
	mutable bool _isNormalized = false;
	mutable std::unique_ptr<detail::Matrix_<FPT>> _hmt;

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
	static HOMOG2D_INUMTYPE _zeroDeterminantValue; /// Used in matrix inversion
};

//------------------------------------------------------------------

#ifdef HOMOG2D_USE_OPENCV
//------------------------------------------------------------------
/// Point drawing style
enum class PtStyle
{
	Plus,   ///< "+" symbol
	Times,  ///< "times" symbol
	Star,   ///< "*" symbol
	Diam    ///< diamond
};

//------------------------------------------------------------------
/// Draw parameters for Opencv binding, only available if HOMOG2D_USE_OPENCV defined,
/// see Root::draw()
struct CvDrawParams
{
/// Inner struct, holds the values. Needed so we can assign a default value as static member
	struct Dp_values
	{
		cv::Scalar _color         = cv::Scalar(80,80,80); // gray
		int        _lineThickness = 1;
		int        _lineType      = cv::LINE_AA; ///< or cv::LINE_8
		int        _ptDelta       = 8;           ///< pixels, used for drawing points
		PtStyle    _ptStyle       = PtStyle::Plus;
		bool       _enhancePoint  = false;       ///< to draw selected points
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
	CvDrawParams& setPointStyle( PtStyle ps )
	{
		if( (int)ps > (int)PtStyle::Diam )
			throw std::runtime_error( "Error: invalid value for point style");
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
enum class GivenCoord { X, Y };

/// Used in Line2d::addOffset()
enum class LineOffset { vert, horiz };

namespace detail {

// forward declaration of template instanciation
template<typename T1,typename T2,typename FPT1,typename FPT2>
Root<T1,FPT1> crossProduct( const Root<T2,FPT1>&, const Root<T2,FPT2>& );

struct Inters_1 {};
struct Inters_2 {};

/// Common stuff for intersection code
struct IntersectCommon
{
	protected:
		bool _doesIntersect = false;
	public:
		bool operator()() const
		{
			return _doesIntersect;
		}
};

/// Base class for intersection
template<typename T,typename FPT>
struct Intersect {};

/// One point intersection
template<typename FPT>
class Intersect<Inters_1,FPT>: public IntersectCommon
{
	template<typename U>
	friend class ::h2d::Segment_;

	public:
		Point2d_<FPT>
		get() const
		{
			if( !_doesIntersect )
				HOMOG2D_THROW_ERROR_1( "No intersection points!" );
			return _ptIntersect;
		}
		void set( const Point2d_<FPT>& pt )
		{
			_ptIntersect = pt;
			_doesIntersect = true;
		}
		size_t size() const { return _doesIntersect?1:0; }

		Intersect() {}
		Intersect( const Point2d_<FPT>& ptInter )
			: _ptIntersect(ptInter)
		{
			_doesIntersect = true;
		}
/// To enable conversions from different floating-point types
		template<typename FPT2>
		Intersect( const Intersect<Inters_1,FPT2>& other )
			: IntersectCommon( other )
		{
			_ptIntersect   = other._ptIntersect;
		}

	private:
		Point2d_<FPT> _ptIntersect;
};

/// Two points intersection
template<typename FPT>
class Intersect<Inters_2,FPT>: public IntersectCommon
{
	template<typename U,typename V>
	friend class ::h2d::Root;

	public:
		Intersect() {}
		Intersect( const Point2d_<FPT>& p1, const Point2d_<FPT>& p2 )
			: _ptIntersect_1(p1), _ptIntersect_2(p2)
		{
				_doesIntersect = true;
		}
/// To enable conversions from different floating-point types
		template<typename FPT2>
		Intersect( const Intersect<Inters_2,FPT2>& other )
			: IntersectCommon( other )
		{
			auto ppts = other.get();
			_ptIntersect_1 = ppts.first;
			_ptIntersect_2 = ppts.second;
		}
		size_t size() const { return _doesIntersect?2:0; }

		std::pair<Point2d_<FPT>,Point2d_<FPT>>
		get() const
		{
			if( !_doesIntersect )
				HOMOG2D_THROW_ERROR_1( "No intersection points!" );
			return std::make_pair( _ptIntersect_1, _ptIntersect_2 );
		}

	private:
		Point2d_<FPT> _ptIntersect_1, _ptIntersect_2;

	friend std::ostream& operator << ( std::ostream& f, const Intersect<Inters_2,FPT>& inters )
	{
		f << "bool=" << inters._doesIntersect
			<< " p1:" << inters._ptIntersect_1
			<< " p2:" << inters._ptIntersect_2;
		return f;
	}
};

/// Multiple points intersections
template<typename FPT>
class IntersectM
{
	private:
		mutable std::vector<Point2d_<FPT>> _vecInters;
	public:
		IntersectM() {}
/// To enable conversions from different floating-point types
		template<typename FPT2>
		IntersectM( const IntersectM<FPT2>& other )
		{
			_vecInters.resize( other.size() );
			auto it = _vecInters.begin();
			for( const auto& elem: other.get() )
				*it++ = elem; // automatic type conversion
		}

		bool operator()() const
		{
			return !_vecInters.empty();
		}
		size_t size() const { return _vecInters.size(); }
		void add( const Point2d_<FPT>& pt )
		{
			_vecInters.push_back(pt);
		}

		void add( const std::vector<Point2d_<FPT>>& vpt )
		{
			for( const auto& pt: vpt )
				_vecInters.push_back(pt);
		}

		std::vector<Point2d_<FPT>> get() const
		{
			std::sort( std::begin(_vecInters), std::end(_vecInters) );
			return _vecInters;
		}
};

} // namespace detail


//------------------------------------------------------------------
/// A Flat Rectangle, modeled with its two opposite points
template<typename FPT>
class FRect_
{
	using FType = FPT;
	template<typename T> friend class FRect_;

private:
	Point2d_<FPT> _ptR1,_ptR2;

	public:
/// Default constructor, initialize rectangle to (0,0)-(1,1)
	FRect_()
	{
		_ptR2.set( 1., 1. );
	}
/// Constructor from 2 points
	FRect_( const Point2d_<FPT>& pa, const Point2d_<FPT>& pb )
	{
		set( pa, pb );
	}

/// Constructor from x1, y1, x2, y2
	template<typename T>
	FRect_( T x1, T y1, T x2, T y2 )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		set( Point2d_<FPT>(x1,y1), Point2d_<FPT>(x2,y2) );
	}

	void set( const Point2d_<FPT>& pa, const Point2d_<FPT>& pb )
	{
		auto ppts = detail::getCorrectPoints( pa, pb );
		_ptR1 = ppts.first;
		_ptR2 = ppts.second;
	}
	HOMOG2D_INUMTYPE height() const { return  _ptR2.getY() - _ptR1.getY(); }
	HOMOG2D_INUMTYPE width()  const { return  _ptR2.getX() - _ptR1.getX(); }

	Circle_<FPT> getBoundingCircle() const;

	template<typename T1, typename T2>
	void translate( T1 dx, T2 dy )
	{
		_ptR1.set( _ptR1.getX() + dx, _ptR1.getY() + dy );
		_ptR2.set( _ptR2.getX() + dx, _ptR2.getY() + dy );
	}

/// Returns the 2 major points of the rectangle
	std::pair<Point2d_<FPT>,Point2d_<FPT>>
	getPts() const
	{
		return std::make_pair( _ptR1, _ptR2 );
	}

/// Returns the 4 points of the rectangle, starting from "smallest" one, and
/// in clockwise order
	std::array<Point2d_<FPT>,4>
	get4Pts() const
	{
		std::array<Point2d_<FPT>,4> arr;
		arr[0] = _ptR1;
		arr[1] = Point2d_<FPT>( _ptR1.getX(), _ptR2.getY() );
		arr[2] = _ptR2;
		arr[3] = Point2d_<FPT>( _ptR2.getX(), _ptR1.getY() );
		return arr;
	}
/// Returns the 4 segments of the rectangle
/// \sa h2d::getSegs( const FRect& );
	std::array<Segment_<FPT>,4>
	getSegs() const
	{
		auto pts = get4Pts();
		std::array<Segment_<FPT>,4> out;
		out[0] = Segment_<FPT>( pts[0], pts[1] );
		out[1] = Segment_<FPT>( pts[1], pts[2] );
		out[2] = Segment_<FPT>( pts[2], pts[3] );
		out[3] = Segment_<FPT>( pts[3], pts[0] );
		return out;
	}
/// Returns true if rectangle is inside \c shape (circle or rectangle)
/// \todo maybe add some SFINAE to enable only for Circle_ or FRect_?
	template<typename T>
	bool isInside( const T& shape )
	{
		for( const auto& pt: get4Pts() )
			if( !pt.isInside( shape ) )
				return false;
		return true;
	}

/// FRect/Line intersection
	template<typename FPT2>
	detail::Intersect<detail::Inters_2,FPT> intersects( const Line2d_<FPT2>& line ) const
	{
		HOMOG2D_START;
		return line.intersects( *this );
	}

/// FRect/Segment intersection
	template<typename FPT2>
	detail::IntersectM<FPT> intersects( const Segment_<FPT2>& seg ) const
	{
		HOMOG2D_START;
		detail::IntersectM<FPT> out;
		for( const auto& rseg: getSegs() )
		{
			auto inters = rseg.intersects( seg ); // call of Segment/Segment
			if( inters() )
			{
				auto pt =  inters.get();
				bool addPoint = true;
				if( out.size() == 1 ) // if we have already one
					if( out.get()[0] == pt )
						addPoint = false;
				if( addPoint )
					out.add( pt );
				if( out.size() == 2 )
					break;
			}
		}
		return out;
	}

private:
/// Intersection of FRect vs FRect or Circle
	template<typename T>
	detail::IntersectM<FPT> p_intersects_R_C( const T& other ) const
	{
		std::set<Point2d_<FPT>> pts;
		for( const auto& rseg: getSegs() )
		{
			auto inters = rseg.intersects( other ); // call of Segment/FRect => FRect/Segment, or Segment/Circle
			if( inters() )
			{
				auto vpts = inters.get();
				assert( vpts.size() < 3 );
				if( vpts.size() > 0 )
					pts.insert( vpts[0] );
				if( vpts.size() > 1 )
					pts.insert( vpts[1] );
			}
		}
		detail::IntersectM<FPT> out;
		for( const auto& elem: pts )
			out.add( elem );
		return out;
	}

public:
/// FRect/Circle intersection
	template<typename FPT2>
	detail::IntersectM<FPT> intersects( const Circle_<FPT2>& circle ) const
	{
		HOMOG2D_START;
		return p_intersects_R_C( circle );
	}

/// FRect/FRect intersection
	template<typename FPT2>
	detail::IntersectM<FPT> intersects( const FRect_<FPT2>& rect ) const
	{
		HOMOG2D_START;
		if( *this == rect )
			return detail::IntersectM<FPT>();
		return p_intersects_R_C( rect );
	}

	template<typename FPT2>
	bool operator == ( const FRect_<FPT2>& other ) const
	{
		if( _ptR1 != other._ptR1 )
			return false;
		if( _ptR2 != other._ptR2 )
			return false;
		return true;
	}
	template<typename FPT2>
	bool operator != ( const FRect_<FPT2>& other ) const
	{
		return !( *this == other );
	}

	friend std::ostream&
	operator << ( std::ostream& f, const FRect_<FPT>& r )
	{
		f << "pt1: " << r._ptR1 << " pt2: " << r._ptR2;
		return f;
	}

#ifdef HOMOG2D_USE_OPENCV
	void draw( cv::Mat& mat, CvDrawParams dp=CvDrawParams() )  const
	{
		cv::rectangle(
			mat,
			_ptR1.getCvPti(),
			_ptR2.getCvPti(),
			dp._dpValues._color,
			dp._dpValues._lineThickness,
			dp._dpValues._lineType
		);
	}
#endif
};

/// Returns the 4 points of the rectangle (free function)
/// \sa FRect_::get4Pts()
template<typename FPT>
std::array<Point2d_<FPT>,4>
get4Pts( const FRect_<FPT>& rect )
{
	return rect.get4Pts();
}

/// Returns the 2 major points of the rectangle (free function)
/// \sa FRect_::getPts()
template<typename FPT>
std::pair<Point2d_<FPT>,Point2d_<FPT>>
getPts( const FRect_<FPT>& rect )
{
	return rect.getPts();
}

/// Free function
template<typename FPT>
HOMOG2D_INUMTYPE height( const FRect_<FPT>& rect )
{
	return rect.height();
}
/// Free function
template<typename FPT>
HOMOG2D_INUMTYPE width( const FRect_<FPT>& rect )
{
	return rect.width();
}

//------------------------------------------------------------------
/// A circle
template<typename FPT>
class Circle_
{
	template<typename T> friend class Circle_;

private:
	FPT           _radius;
	Point2d_<FPT> _center;

public:
/// Default constructor, unit-radius circle at (0,0)
	Circle_() : _radius(1.)
	{}
/// Constructor, given radius circle at (0,0)
	Circle_( FPT rad ) : _radius(rad)
	{
		if( std::abs(rad) < Point2d_<FPT>::nullDistance() )
			HOMOG2D_THROW_ERROR_1( "radius must not be 0" );
	}

/// Constructor, given radius circle at (0,0)
	template<typename FPT2>
	Circle_( const Point2d_<FPT2>& center, FPT rad )
		: _radius(rad), _center(center)
	{
		if( std::abs(rad) < Point2d_<FPT>::nullDistance() )
			HOMOG2D_THROW_ERROR_1( "radius must not be 0" );
	}

	FPT&       radius()       { return _radius; }
	const FPT& radius() const { return _radius; }

	Point2d_<FPT>       center()       { return _center; }
	const Point2d_<FPT> center() const { return _center; }

/// Returns Bounding Box
		FRect_<FPT> getBB() const
		{
			return FRect_<FPT>(
				_center.getX()-_radius, _center.getY()-_radius,
				_center.getX()+_radius, _center.getY()+_radius
			);
		}

	void set( const Point2d_<FPT>& center, FPT rad )
	{
		_radius = rad;
		_center = center;
	}
/// Returns true if circle is inside \c other circle
	template<typename FPT2>
	bool isInside( const Circle_<FPT2>& other ) const
	{
		return( _radius + _center.distTo( other.center() ) < other.radius() );
	}

/// Returns true if circle is inside rectangle defined by \c p1 and \c p2
	template<typename FPT2>
	bool isInside( const Point2d_<FPT2>& p1, const Point2d_<FPT2>& p2 ) const
	{
		return implC_isInside( detail::getCorrectPoints( p1, p2 ) );
	}

/// Returns true if circle is inside flat rectangle \c rect
	template<typename FPT2>
	bool isInside( const FRect_<FPT2>& rect )
	{
		return implC_isInside( rect.getPts() );
	}

/// Circle/Line intersection
	template<typename FPT2>
	detail::Intersect<detail::Inters_2,FPT>
	intersects( const Line2d_<FPT2>& li ) const
	{
		HOMOG2D_START;
		return li.intersects( *this );
	}

/// Circle/Segment intersection
	template<typename FPT2>
	detail::IntersectM<FPT>
	intersects( const Segment_<FPT2>& seg ) const
	{
		HOMOG2D_START;
		return seg.intersects( *this );
	}

// Circle/Circle intersection
	template<typename FPT2>
	detail::Intersect<detail::Inters_2,FPT>
	intersects( const Circle_<FPT2>& seg ) const;

/// Circle/FRect intersection
	template<typename FPT2>
	detail::IntersectM<FPT> intersects( const FRect_<FPT2>& rect ) const
	{
		HOMOG2D_START;
		return rect.intersects( * this );
	}

private:
	template<typename FPT2>
	bool implC_isInside( const std::pair<Point2d_<FPT2>, Point2d_<FPT2>>& ppts ) const
	{
		auto p1 = ppts.first;
		auto p2 = ppts.second;
		if( _center.getX() + _radius < p2.getX() )
			if( _center.getX() - _radius > p1.getX() )
				if( _center.getY() + _radius < p2.getY() )
					if( _center.getY() - _radius > p1.getY() )
						return true;
		return false;
	}

public:
	template<typename FPT2>
	bool operator == ( const Circle_<FPT2>& other ) const
	{
		if( _radius != other._radius )
			return false;
		if( _center != other._center )
			return false;
		return true;
	}
	template<typename FPT2>
	bool operator != ( const Circle_<FPT2>& other ) const
	{
		return !( *this == other );
	}

	friend std::ostream&
	operator << ( std::ostream& f, const Circle_<FPT>& r )
	{
		f << "center: " << r._center << ", radius=" << r._radius;
		return f;
	}

#ifdef HOMOG2D_USE_OPENCV
	void draw( cv::Mat& mat, CvDrawParams dp=CvDrawParams() )  const
	{
		cv::circle(
			mat,
			_center.getCvPti(),
			static_cast<int>(_radius),
			dp._dpValues._color,
			dp._dpValues._lineThickness,
			dp._dpValues._lineType
		);
	}
#endif // HOMOG2D_USE_OPENCV
};

//------------------------------------------------------------------
/// Return circle passing through 4 points of flat rectangle
template<typename FPT>
Circle_<FPT>
FRect_<FPT>::getBoundingCircle() const
{
	auto pts = get4Pts();
	auto seg1 = pts[1] * pts[3];
	auto seg2 = pts[0] * pts[2];

	auto middle_pt = seg1 * seg2;
	return Circle_<FPT>( middle_pt, middle_pt.distTo( pts[0] ) );
}

/// Return circle passing through 4 points of flat rectangle (free function)
template<typename FPT>
Circle_<FPT>
getBoundingCircle( const FRect_<FPT>& rect )
{
	return rect.getBoundingCircle();
}
//------------------------------------------------------------------
/// Holds private stuff
namespace priv {

/// Private free function, swap the points so that \c ptA.x <= \c ptB.x, and if equal, sorts on y
template<typename FPT>
void
fix_order( Point2d_<FPT>& ptA, Point2d_<FPT>& ptB )
{
	if( ptA.getX() > ptB.getX() )
		std::swap( ptA, ptB );
	else
		if( ptA.getX() == ptB.getX() )
			if( ptA.getY() > ptB.getY() )
				std::swap( ptA, ptB );
}


/// Helper function, factorized here for the two impl_getPoints_A() implementations
template<typename FPT, typename FPT2>
std::pair<Point2d_<FPT>,Point2d_<FPT>>
getPoints_B2( const Point2d_<FPT>& pt, FPT2 dist, const Line2d_<FPT>& li )
{
	auto arr = li.get();
	const HOMOG2D_INUMTYPE a = static_cast<HOMOG2D_INUMTYPE>(arr[0]);
	const HOMOG2D_INUMTYPE b = static_cast<HOMOG2D_INUMTYPE>(arr[1]);
	auto coeff = static_cast<HOMOG2D_INUMTYPE>(dist) / std::sqrt( a*a + b*b );

	Point2d_<FPT> pt1(
        pt.getX() -  b * coeff,
        pt.getY() +  a * coeff
	);
	Point2d_<FPT> pt2(
        pt.getX() +  b * coeff,
        pt.getY() -  a * coeff
	);
	fix_order( pt1, pt2 );
	return std::make_pair( pt1, pt2 );
}

/// Helper function for impl_getOrthogonalLine_A() and impl_getOrthogonalLine_B()
template<typename T1,typename T2>
Line2d_<T1>
getOrthogonalLine_B2( const Point2d_<T2>& pt, const Line2d_<T1>& li )
{
	auto arr = li.get();
	Line2d_<T1> out(
		-arr[1],
		arr[0],
		arr[1] * pt.getX() - arr[0] * pt.getY()
	);
	out.p_normalizeLine();
	return out;
}

} // namespace priv

/// Type of Root object, see Root::type()
enum class Type : char
{
	Line2d,Point2d
};

/// Type of underlying floating point, see Root::dtype()
enum class Dtype : char
{
	Float,Double,LongDouble
};

//------------------------------------------------------------------
template<typename FPT>
class Ellipse_
{
	template<typename T> friend class Ellipse_;

private:
	Point2d_<FPT> _ecenter;
	Point2d_<FPT> _semiMajor;
	Point2d_<FPT> _semiMinor;
//	Segment_<FPT> _semiMajor;
//	Segment_<FPT> _semiMinor;

public:
/// Constructor: straight ellipse
	template<typename FPT1, typename T2>
	Ellipse_( const Point2d_<FPT1>& pt, T2 major, T2 minor )
		: _ecenter( pt )
	{
		 _semiMajor.set( pt.getX()+major, pt.getY() );
		 _semiMinor.set( pt.getX(),       pt.getY()+minor );
	}

	Point2d_<FPT>&       center()       { return _ecenter; }
	const Point2d_<FPT>& center() const { return _ecenter; }

	HOMOG2D_INUMTYPE getMajor() const
	{
		return _ecenter.distTo( _semiMajor );
	}
	HOMOG2D_INUMTYPE getMinor() const
	{
		return _ecenter.distTo( _semiMinor );
	}
	std::pair<HOMOG2D_INUMTYPE,HOMOG2D_INUMTYPE> getMajMin() const
	{
		return std::make_pair(
			_ecenter.distTo( _semiMajor ),
			_ecenter.distTo( _semiMinor )
		);
	}
	void setAngle( FPT angle )
	{
		Homogr_<HOMOG2D_INUMTYPE> H( -_ecenter.getX(), -_ecenter.getY() );
		H.addRotation( angle ).addTranslation( _ecenter.getX(), _ecenter.getY() );
		_semiMajor = H * _semiMajor;
		_semiMinor = H * _semiMinor;
	}

	HOMOG2D_INUMTYPE getAngle() const
	{
		auto lineH = Point2d_<FPT>(0,0) * Point2d_<FPT>(100,0);
		return lineH.getAngle( Line2d_<FPT>(_ecenter, _semiMajor) );
	}

#ifdef HOMOG2D_USE_OPENCV
	void draw( cv::Mat& mat, CvDrawParams dp=CvDrawParams() ) const;
#endif // HOMOG2D_USE_OPENCV

	friend std::ostream&
	operator << ( std::ostream& f, const Ellipse_<FPT>& ell )
	{
		f << "center=" << ell._ecenter
			<< " maj=" << ell.getMajor()
			<< " min=" << ell.getMinor();

		return f;
	}
};

//------------------------------------------------------------------
/// Base class, will be instanciated as a \ref Point2d or a \ref Line2d
/**
Parameters:
- LP: Line or Point
- FPT: Floating Point Type
*/
template<typename LP,typename FPT>
class Root
{
public:
	using FType = FPT;

private:
	template<typename U,typename V> friend class Hmatrix_;

// This is needed so we can convert from, say, Point2d_<float> to Point2d_<double>
	template<typename U,typename V> friend class Root;

	template<typename FPT1,typename FPT2>
	friend Point2d_<FPT1>
	operator * ( const Line2d_<FPT1>&, const Line2d_<FPT2>& );

	template<typename FPT1,typename FPT2>
	friend Line2d_<FPT1>
	operator * ( const Point2d_<FPT1>&, const Point2d_<FPT2>& );

	template<typename T,typename U>
	friend Line2d_<T>
	operator * ( const Homogr_<U>&, const Line2d_<T>& );

	template<typename T1,typename T2,typename FPT1,typename FPT2>
	friend Root<T1,FPT1>
	detail::crossProduct( const Root<T2,FPT1>&, const Root<T2,FPT2>& );

	template<typename U,typename V>
	friend std::ostream&
	operator << ( std::ostream& f, const Root<U,V>& r );

	template<typename T1,typename T2,typename FPT1,typename FPT2>
	friend void
	detail::product( Root<T1,FPT1>&, const detail::Matrix_<FPT2>&, const Root<T2,FPT1>& );

	template<typename T1,typename T2>
	friend Line2d_<T1>
	priv::getOrthogonalLine_B2( const Root<type::IsPoint,T2>&, const Line2d_<T1>& );

	public:

/// Constructor: build a point from two lines
		template<typename FPT2>
		Root( const Line2d_<FPT2>& v1, const Line2d_<FPT2>& v2 )
		{
#ifndef HOMOG2D_NOCHECKS
			if( v1.isParallelTo(v2) )
				HOMOG2D_THROW_ERROR_1( "unable to build point from these two lines, are parallel" );
#endif
			*this = detail::crossProduct<type::IsPoint>( v1, v2 );
		}

/// Constructor: build a line from two points
		template<typename FPT2>
		Root( const Point2d_<FPT2>& v1, const Point2d_<FPT2>& v2 )
		{
#ifndef HOMOG2D_NOCHECKS
			if( v1 == v2 )
				HOMOG2D_THROW_ERROR_1( "unable to build line from these two points, are the same" );
#endif
			*this = detail::crossProduct<type::IsLine>( v1, v2 );
			p_normalizeLine();
		}

/// Constructor: copy-constructor for lines
/**
\todo We should be able to declare this "explicit". This fails at present when attempting
to convert a line (or point) from double to float, but I don't get why...
*/
		template<typename T>
//		explicit
		Root( const Line2d_<T>& li )
		{
			impl_init_1_Line<T>( li, detail::RootHelper<LP>() );
		}

/// Constructor with single arg of type "Point"
/**
This will call one of the two overloads of \c impl_init_1_Point(), depending on type of object:
- if type is a point, then it can be seen as a copy-constructor
- if type is a line, this will build a line from (0,0] to \c pt
*/
		template<typename T>
		Root( const Point2d_<T>& pt )
		{
			impl_init_1_Point<T>( pt, detail::RootHelper<LP>() );
		}

/// Constructor: build from two numerical values, depends on the type
		template<typename T1,typename T2>
		Root( const T1& v1, const T2& v2 )
		{
			HOMOG2D_CHECK_IS_NUMBER(T1);
			HOMOG2D_CHECK_IS_NUMBER(T2);
			impl_init_2( v1, v2, detail::RootHelper<LP>() );
		}

/// Constructor of line from 3 values
		template<typename T>
		Root( T v0, T v1, T v2 )
		{
			HOMOG2D_CHECK_IS_NUMBER(T);
			_v[0] = v0;
			_v[1] = v1;
			_v[2] = v2;
		}

/// Default constructor, depends on the type
		Root()
		{
			impl_init( detail::RootHelper<LP>() );
		}

	private:
		template<typename T,typename U>
		void p_copyFrom( const Root<T,U>& other )
		{
			_v[0] = static_cast<FPT>(other._v[0]);
			_v[1] = static_cast<FPT>(other._v[1]);
			_v[2] = static_cast<FPT>(other._v[2]);
		}
		/// Arg is a point, object is a point => copy-constructor
		template<typename T>
		void impl_init_1_Point( const Point2d_<T>& pt, const detail::RootHelper<type::IsPoint>& )
		{
			p_copyFrom( pt );
		}
		/// Arg is a point, object is a line: we build the line passing though (0,0) ant the given point
		template<typename T>
		void impl_init_1_Point( const Point2d_<T>& pt, const detail::RootHelper<type::IsLine>& )
		{
			*this = detail::crossProduct<type::IsLine>( pt, Point2d_<FPT>() );
			p_normalizeLine();
		}

		/// Arg is a line, object is a point: ILLEGAL INSTANCIATION
		template<typename T>
		void impl_init_1_Line( const Line2d_<T>&, const detail::RootHelper<type::IsPoint>& )
		{
			static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot build a point from a line" );
		}
		/// Arg is a line, object is a line => copy-constructor
		template<typename T>
		void impl_init_1_Line( const Line2d_<T>& li, const detail::RootHelper<type::IsLine>& )
		{
			p_copyFrom( li );
		}

	public:
		Type type() const
		{
			return impl_type( detail::RootHelper<LP>() );
		}
		Dtype dtype() const
		{
			return impl_dtype( detail::RootDataType<FPT>() );
		}

	private:
		Type impl_type( const detail::RootHelper<type::IsPoint>& ) const
		{
			return Type::Point2d;
		}
		Type impl_type( const detail::RootHelper<type::IsLine>& ) const
		{
			return Type::Line2d;
		}

		Dtype impl_dtype( const detail::RootDataType<float>& ) const
		{
			return Dtype::Float;
		}
		Dtype impl_dtype( const detail::RootDataType<double>& ) const
		{
			return Dtype::Double;
		}
		Dtype impl_dtype( const detail::RootDataType<long double>& ) const
		{
			return Dtype::LongDouble;
		}

	public:
		FPT
		getCoord( GivenCoord gc, FPT other ) const
		{
			return impl_getCoord( gc, other, detail::RootHelper<LP>() );
		}

		Point2d_<FPT>
		getPoint( GivenCoord gc, FPT other ) const
		{
			return impl_getPoint( gc, other, detail::RootHelper<LP>() );
		}

		/// Returns a pair of points that are lying on line at distance \c dist from a point defined by one of its coordinates.
		template<typename FPT2>
		std::pair<Point2d_<FPT>,Point2d_<FPT>>
		getPoints( GivenCoord gc, FPT coord, FPT2 dist ) const
		{
			return impl_getPoints_A( gc, coord, dist, detail::RootHelper<LP>() );
		}

		/// Returns a pair of points that are lying on line at distance \c dist from point \c pt, assuming that one is lying on the line.
		template<typename FPT2>
		std::pair<Point2d_<FPT>,Point2d_<FPT>>
		getPoints( const Point2d_<FPT>& pt, FPT2 dist ) const
		{
			return impl_getPoints_B( pt, dist, detail::RootHelper<LP>() );
		}

		/// Returns an orthogonal line to the one it is called on, at a point defined by one of its coordinates.
		Line2d_<FPT>
		getOrthogonalLine( GivenCoord gc, FPT other ) const
		{
			return impl_getOrthogonalLine_A( gc, other, detail::RootHelper<LP>() );
		}

		/// Returns an orthogonal line to the one it is called on, at point \c pt, assuming that one is lying on the line.
		Line2d_<FPT>
		getOrthogonalLine( const Point2d_<FPT>& pt ) const
		{
			return impl_getOrthogonalLine_B( pt, detail::RootHelper<LP>() );
		}

		/// Returns an parallel line to the one it is called on, with \c pt lying on it.
		Line2d_<FPT>
		getParallelLine( const Point2d_<FPT>& pt ) const
		{
			return impl_getParallelLine( pt, detail::RootHelper<LP>() );
		}
		/// Returns the pair of parallel lines at a distance \c dist from line.
		template<typename T>
		std::pair<Line2d_<FPT>,Line2d_<FPT>>
		getParallelLines( T dist )
		{
			return impl_getParallelLines( dist, detail::RootHelper<LP>() );
		}

		FPT getX() const         { return impl_getX( detail::RootHelper<LP>() ); }
		FPT getY() const         { return impl_getY( detail::RootHelper<LP>() ); }
		std::array<FPT,3> get() const { return impl_get( detail::RootHelper<LP>() ); }

		template<typename T1,typename T2>
		void set( T1 x, T2 y ) { impl_set( x, y, detail::RootHelper<LP>() ); }

		template<typename FPT2>
		HOMOG2D_INUMTYPE distTo( const Point2d_<FPT2>& pt ) const
		{
			return impl_distToPoint( pt, detail::RootHelper<LP>() );
		}
		template<typename FPT2>
		HOMOG2D_INUMTYPE distTo( const Line2d_<FPT2>& li ) const
		{
			return impl_distToLine( li, detail::RootHelper<LP>() );
		}

		template<typename T,typename FPT2>
		bool isParallelTo( const Root<T,FPT2>& li ) const
		{
			return impl_isParallelTo( li, detail::RootHelper<T>() );
		}
		template<typename T>
		bool isParallelTo( const Segment_<T>& seg ) const
		{
			return impl_isParallelTo( seg.getLine(), detail::RootHelper<LP>() );
		}
/// Returns angle in rad. between the lines. \sa h2d::getAngle()
/**
Please check out warning described in impl_getAngle()
*/
		template<typename T,typename FPT2>
		HOMOG2D_INUMTYPE getAngle( const Root<T,FPT2>& other ) const
		{
			return impl_getAngle( other, detail::RootHelper<T>() );
		}

/// Returns angle in rad. between line and segment \c seg. \sa  h2d::getAngle()
		template<typename FPT2>
		HOMOG2D_INUMTYPE getAngle( const Segment_<FPT2>& seg ) const
		{
			return impl_getAngle( seg.getLine(), detail::RootHelper<LP>() );
		}

	private:
		FPT impl_getX( const detail::RootHelper<type::IsPoint>& ) const
		{
			return _v[0]/_v[2];
		}
		FPT impl_getY( const detail::RootHelper<type::IsPoint>& ) const
		{
			return _v[1]/_v[2];
		}

		std::array<FPT,3> impl_get( const detail::RootHelper<type::IsPoint>& ) const
		{
			static_assert( detail::AlwaysFalse<LP>::value, "illegal for points" );
		}
		std::array<FPT,3> impl_get( const detail::RootHelper<type::IsLine>& ) const
		{
			return std::array<FPT,3> { _v[0], _v[1], _v[2] };
		}
		template<typename T1,typename T2>
		void impl_set( T1 x, T2 y, const detail::RootHelper<type::IsPoint>& )
		{
			_v[0] = x;
			_v[1] = y;
			_v[2] = 1.;
		}
		template<typename T1,typename T2>
		void impl_set( T1, T2, const detail::RootHelper<type::IsLine>& )
		{
			static_assert( detail::AlwaysFalse<LP>::value, "Invalid call for lines" );
		}

		template<typename FPT2>
		HOMOG2D_INUMTYPE impl_distToPoint( const Point2d_<FPT2>&, const detail::RootHelper<type::IsPoint>& ) const;
		template<typename FPT2>
		HOMOG2D_INUMTYPE impl_distToPoint( const Point2d_<FPT2>&, const detail::RootHelper<type::IsLine>&  ) const;
		template<typename FPT2>
		HOMOG2D_INUMTYPE impl_distToLine(  const Line2d_<FPT2>&,  const detail::RootHelper<type::IsPoint>& ) const;
		template<typename FPT2>
		HOMOG2D_INUMTYPE impl_distToLine(  const Line2d_<FPT2>&,  const detail::RootHelper<type::IsLine>&  ) const;

		HOMOG2D_INUMTYPE impl_getAngle(     const Root<LP,FPT>&, const detail::RootHelper<type::IsLine>&  ) const;
		HOMOG2D_INUMTYPE impl_getAngle(     const Root<LP,FPT>&, const detail::RootHelper<type::IsPoint>& ) const;

		template<typename FPT2>
		bool impl_isParallelTo( const Root<LP,FPT2>&, const detail::RootHelper<type::IsLine>&  ) const;
		template<typename FPT2>
		bool impl_isParallelTo( const Root<LP,FPT2>&, const detail::RootHelper<type::IsPoint>& ) const;

		FPT impl_getCoord( GivenCoord gc, FPT other, const detail::RootHelper<type::IsLine>& ) const;
		FPT impl_getCoord( GivenCoord gc, FPT other, const detail::RootHelper<type::IsPoint>& ) const;

		Point2d_<FPT> impl_getPoint( GivenCoord gc, FPT other, const detail::RootHelper<type::IsLine>& ) const;
		Point2d_<FPT> impl_getPoint( GivenCoord gc, FPT other, const detail::RootHelper<type::IsPoint>& ) const;

		template<typename FPT2>
		std::pair<Point2d_<FPT>,Point2d_<FPT>> impl_getPoints_A( GivenCoord gc, FPT coord, FPT2 dist, const detail::RootHelper<type::IsLine>& ) const;
		template<typename FPT2>
		std::pair<Point2d_<FPT>,Point2d_<FPT>> impl_getPoints_A( GivenCoord gc, FPT coord, FPT2 dist, const detail::RootHelper<type::IsPoint>& ) const;
		template<typename FPT2>
		std::pair<Point2d_<FPT>,Point2d_<FPT>> impl_getPoints_B( const Point2d_<FPT>&, FPT2 dist, const detail::RootHelper<type::IsLine>& ) const;
		template<typename FPT2>
		std::pair<Point2d_<FPT>,Point2d_<FPT>> impl_getPoints_B( const Point2d_<FPT>&, FPT2 dist, const detail::RootHelper<type::IsPoint>& ) const;

		void impl_op_stream( std::ostream&, const Point2d_<FPT>& ) const;
		void impl_op_stream( std::ostream&, const Line2d_<FPT>&  ) const;

	public:
/// Line/Line intersection
		template<typename FPT2>
		detail::Intersect<detail::Inters_1,FPT> intersects( const Line2d_<FPT2>& other ) const
		{
			detail::Intersect<detail::Inters_1,FPT> out;
			if( this->isParallelTo( other ) )
				return out;
			 out.set( *this * other );
			 return out;
		}
/// Line/FRect intersection
		template<typename FPT2>
		detail::Intersect<detail::Inters_2,FPT> intersects( const Point2d_<FPT2>& pt1, const Point2d_<FPT2>& pt2 ) const
		{
			HOMOG2D_START;
			return intersects( FRect_<FPT2>( pt1, pt2 ) ) ;
		}
/// Line/FRect intersection
		template<typename FPT2>
		detail::Intersect<detail::Inters_2,FPT> intersects( const FRect_<FPT2>& rect ) const
		{
			HOMOG2D_START;
			return impl_intersectsFRect( rect, detail::RootHelper<LP>() );
		}

/// Line/Segment intersection
/** \warning no implementation for points */
		template<typename FPT2>
		detail::Intersect<detail::Inters_1,FPT> intersects( const Segment_<FPT2>& seg ) const
		{
			HOMOG2D_START;
			return seg.intersects( *this );
		}

/// Line/Circle intersection
/** <br>The Sfinae below is needed to avoid ambiguity with the other 2 args function (with 2 points defining a FRect, see above) */
		template<
			typename T,
			typename std::enable_if<
				(std::is_arithmetic<T>::value && !std::is_same<T,bool>::value)
				,T
			>::type* = nullptr
		>
		detail::Intersect<detail::Inters_2,FPT> intersects( const Point2d_<FPT>& pt0, T radius ) const
		{
			HOMOG2D_START;
			return impl_intersectsCircle( pt0, radius, detail::RootHelper<LP>() );
		}
/// Line/Circle intersection
		template<typename T>
		detail::Intersect<detail::Inters_2,FPT> intersects( const Circle_<T>& cir ) const
		{
			HOMOG2D_START;
			return impl_intersectsCircle( cir.center(), cir.radius(), detail::RootHelper<LP>() );
		}
/// Line/Polyline intersection
		template<typename FPT2>
		detail::IntersectM<FPT> intersects( const Polyline_<FPT2>& pl ) const
		{
			HOMOG2D_START;
//			return impl_intersectsPolyline( pl, detail::RootHelper<LP>() );
			return pl.intersects( *this );
		}

/// Point is inside flat rectangle
		bool isInside( const Point2d_<FPT>& pt1, const Point2d_<FPT>& pt2 ) const
		{
			return impl_isInsideRect( FRect_<FPT>(pt1, pt2), detail::RootHelper<LP>() );
		}

/// Point is inside FRect
		template<typename FPT2>
		bool isInside( const FRect_<FPT2>& rect ) const
		{
			return impl_isInsideRect( rect, detail::RootHelper<LP>() );
		}

/// Point is inside circle
		template<typename T>
		bool isInside( const Point2d_<FPT>& center, T radius ) const
		{
			HOMOG2D_CHECK_IS_NUMBER(T);
			return impl_isInsideCircle( center, radius, detail::RootHelper<LP>() );
		}
/// Point is inside Circle
		template<typename T>
		bool isInside( Circle_<T> cir ) const
		{
			HOMOG2D_CHECK_IS_NUMBER(T);
			return impl_isInsideCircle( cir.center(), cir.radius(), detail::RootHelper<type::IsPoint>() );
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
		bool operator < ( const Root<LP,FPT>& other ) const
		{
			return impl_op_sort( other, detail::RootHelper<LP>() );
		}

#ifdef HOMOG2D_USE_OPENCV
		bool draw( cv::Mat& mat, CvDrawParams dp=CvDrawParams() )  const
		{
			return impl_draw( mat, dp, detail::RootHelper<LP>() );
		}

		template<typename RT>
		RT getCvPt() const { return RT( getX(), getY() ); }

		cv::Point2i getCvPti() const { return impl_getCvPt( detail::RootHelper<LP>(), cv::Point2i() ); }
		cv::Point2d getCvPtd() const { return impl_getCvPt( detail::RootHelper<LP>(), cv::Point2d() ); }
		cv::Point2f getCvPtf() const { return impl_getCvPt( detail::RootHelper<LP>(), cv::Point2f() ); }

/// Constructor: build from a single OpenCv point.
		template<typename T>
		Root( cv::Point_<T> pt )
		{
			impl_init_opencv( pt, detail::RootHelper<LP>() );
		}
#endif

	static HOMOG2D_INUMTYPE& nullAngleValue()     { return _zeroAngleValue; }
	static HOMOG2D_INUMTYPE& nullDistance()       { return _zeroDistance; }
	static HOMOG2D_INUMTYPE& nullOffsetValue()    { return _zeroOffset; }
	static HOMOG2D_INUMTYPE& nullOrthogDistance() { return _zeroOrthoDistance; }
	static HOMOG2D_INUMTYPE& nullDenom()          { return _zeroDenom; }

//////////////////////////
//      DATA SECTION    //
//////////////////////////

	private:
		std::array<FPT,3> _v; ///< data, uses the template parameter FPT (for "Floating Point Type")

		static HOMOG2D_INUMTYPE _zeroAngleValue;       /// Used in isParallel();
		static HOMOG2D_INUMTYPE _zeroDistance;         /// Used to define points as identical
		static HOMOG2D_INUMTYPE _zeroOffset;           /// Used to compare lines
		static HOMOG2D_INUMTYPE _zeroOrthoDistance;    /// Used to check for different points on a flat rectangle, see Root::getCorrectPoints()
		static HOMOG2D_INUMTYPE _zeroDenom;            /// Used to check for null denominator

//////////////////////////
//   PRIVATE FUNCTIONS  //
//////////////////////////
	private:
		void p_normalizeLine() const { impl_normalizeLine( detail::RootHelper<LP>() ); }

		template<typename FPT2>
		detail::Intersect<detail::Inters_2,FPT>
		impl_intersectsFRect( const FRect_<FPT2>& rect, const detail::RootHelper<type::IsLine>& ) const;

		template<typename FPT2>
		detail::Intersect<detail::Inters_2,FPT>
		impl_intersectsFRect( const FRect_<FPT2>& rect, const detail::RootHelper<type::IsPoint>& ) const;

		template<typename T>
		detail::Intersect<detail::Inters_2,FPT>
		impl_intersectsCircle( const Point2d_<FPT>& pt, T radius, const detail::RootHelper<type::IsLine>& ) const;

		template<typename T>
		detail::Intersect<detail::Inters_2,FPT>
		impl_intersectsCircle( const Point2d_<FPT>& pt, T radius, const detail::RootHelper<type::IsPoint>& ) const;

/*		template<typename FPT2>
		detail::IntersectM<FPT>
		impl_intersectsPolyline( const Polyline_<FPT2>& pl, const detail::RootHelper<type::IsLine>& ) const;
		template<typename FPT2>
		detail::IntersectM<FPT>
		impl_intersectsPolyline( const Polyline_<FPT2>& pl, const detail::RootHelper<type::IsPoint>& ) const;
*/
		bool impl_isInsideRect( const FRect_<FPT>&, const detail::RootHelper<type::IsPoint>& ) const;
		bool impl_isInsideRect( const FRect_<FPT>&, const detail::RootHelper<type::IsLine>&  ) const;

		template<typename T>
		bool impl_isInsideCircle( const Point2d_<FPT>&, T radius, const detail::RootHelper<type::IsLine>&  ) const;
		template<typename T>
		bool impl_isInsideCircle( const Point2d_<FPT>&, T radius, const detail::RootHelper<type::IsPoint>& ) const;

		void impl_normalizeLine( const detail::RootHelper<type::IsLine>& ) const;

		Line2d_<FPT> impl_getOrthogonalLine_A( GivenCoord gc, FPT val, const detail::RootHelper<type::IsLine>&  ) const;
		Line2d_<FPT> impl_getOrthogonalLine_A( GivenCoord gc, FPT val, const detail::RootHelper<type::IsPoint>& ) const;
		Line2d_<FPT> impl_getOrthogonalLine_B( const Point2d_<FPT>&,   const detail::RootHelper<type::IsLine>&  ) const;
		Line2d_<FPT> impl_getOrthogonalLine_B( const Point2d_<FPT>&,   const detail::RootHelper<type::IsPoint>& ) const;
		Line2d_<FPT> impl_getParallelLine( const Point2d_<FPT>&, const detail::RootHelper<type::IsLine>&  ) const;
		Line2d_<FPT> impl_getParallelLine( const Point2d_<FPT>&, const detail::RootHelper<type::IsPoint>& ) const;

		template<typename T>
		std::pair<Line2d_<FPT>,Line2d_<FPT>> impl_getParallelLines( T, const detail::RootHelper<type::IsLine>&  ) const;
		template<typename T>
		std::pair<Line2d_<FPT>,Line2d_<FPT>> impl_getParallelLines( T, const detail::RootHelper<type::IsPoint>& ) const;

		bool impl_op_equal( const Root<LP,FPT>&, const detail::RootHelper<type::IsLine>&  ) const;
		bool impl_op_equal( const Root<LP,FPT>&, const detail::RootHelper<type::IsPoint>& ) const;

		bool impl_op_sort( const Root<LP,FPT>&, const detail::RootHelper<type::IsLine>&  ) const;
		bool impl_op_sort( const Root<LP,FPT>&, const detail::RootHelper<type::IsPoint>& ) const;

		Point2d_<FPT> impl_op_product( const Line2d_<FPT>& , const Line2d_<FPT>& , const detail::RootHelper<type::IsPoint>& ) const;
		Line2d_<FPT>  impl_op_product( const Point2d_<FPT>&, const Point2d_<FPT>&, const detail::RootHelper<type::IsLine>&  ) const;

#ifdef HOMOG2D_USE_OPENCV
		template<typename OPENCVT>
		OPENCVT impl_getCvPt( const detail::RootHelper<type::IsPoint>&, const OPENCVT& ) const;

/// Build point from Opencv point
		template<typename T>
		void impl_init_opencv( cv::Point_<T> pt, const detail::RootHelper<type::IsPoint>& )
		{
			impl_init_2( pt.x, pt.y, detail::RootHelper<type::IsPoint>() );
		}
/// Build line from Opencv point
		template<typename T>
		void impl_init_opencv( cv::Point_<T> pt, const detail::RootHelper<type::IsLine>& )
		{
			Point2d_<FPT> p(pt);
			impl_init_1_Point<FPT>( p, detail::RootHelper<type::IsLine>() );
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
		/// Called by default constructor, overload for points. Initialize to (0,0)
		void impl_init( const detail::RootHelper<type::IsPoint>& )
		{
			_v[0] = 0.;
			_v[1] = 0.;
			_v[2] = 1.;
		}
		template<typename T1,typename T2>
		void impl_init_2( const T1&, const T2&, const detail::RootHelper<type::IsPoint>& );
		template<typename T1,typename T2>
		void impl_init_2( const T1&, const T2&, const detail::RootHelper<type::IsLine>& );
};


/// Instanciation of static variable
template<typename LP,typename FPT>
HOMOG2D_INUMTYPE Root<LP,FPT>::_zeroAngleValue = 0.001; // 1 thousand of a radian (tan = 0.001 too)

/// Instanciation of static variable
template<typename LP,typename FPT>
HOMOG2D_INUMTYPE Hmatrix_<LP,FPT>::_zeroDeterminantValue = 1E-20;

/// Instanciation of static variable
template<typename LP,typename FPT>
HOMOG2D_INUMTYPE Root<LP,FPT>::_zeroDistance = 1E-8;

/// Instanciation of static variable
template<typename LP,typename FPT>
HOMOG2D_INUMTYPE Root<LP,FPT>::_zeroOrthoDistance = 1E-18;

template<typename LP,typename FPT>
HOMOG2D_INUMTYPE Root<LP,FPT>::_zeroDenom = 1E-10;

/// Instanciation of static variable
template<typename LP,typename FPT>
HOMOG2D_INUMTYPE Root<LP,FPT>::_zeroOffset = 1E-15;

template<typename FPT>
FPT getX( const Point2d_<FPT>& pt) { return pt.getX(); }
template<typename FPT>
FPT getY( const Point2d_<FPT>& pt) { return pt.getY(); }

#ifdef HOMOG2D_USE_OPENCV
template<typename FPT>
FRect_<FPT> getFRect( cv::Mat& mat )
{
	if(  mat.cols == 0 || mat.rows == 0 )
		throw HOMOG2D_THROW_ERROR_1( "Illegal values: cols=" + std::to_string(mat.cols) + ", rows=" + std::to_string(mat.rows) );

	return FRect_<FPT>(
		Point2d_<FPT>(),                      // (0,0)
		Point2d_<FPT>( mat.cols, mat.rows )   // (w,h)
	);
}

/// Free function to return an OpenCv point
/**
- RT: return type
- FPT: Floating Point Type

User code needs to provide the requested type as template argument:
\code
auto p1 = getCvPt<cv::Point2di>( pt );
auto p2 = getCvPt<cv::Point2df>( pt );
auto p3 = getCvPt<cv::Point2dd>( pt );
\endcode
*/
template<typename RT,typename FPT>
RT
getCvPt( const Point2d_<FPT>& pt )
{
	return pt.template getCvPt<RT>();
}

/// Free function to return an OpenCv point (double)
template<typename FPT>
cv::Point2d
getCvPtd( const Point2d_<FPT>& pt )
{
	return pt.getCvPtd();
}
/// Free function to return an OpenCv point (float)
template<typename FPT>
cv::Point2f
getCvPtf( const Point2d_<FPT>& pt )
{
	return pt.getCvPtf();
}
/// Free function to return an OpenCv point (integer)
template<typename FPT>
cv::Point2i
getCvPti( const Point2d_<FPT>& pt )
{
	return pt.getCvPti();
}

/// Free function, returns a vector of OpenCv points from a vector of points
/**
- RT: return type
- FPT: Floating Point Type

User code needs to provide the requested type as template argument:
\code
auto v1 = getCvPts<cv::Point2di>( myvec );
auto v2 = getCvPts<cv::Point2df>( myvec );
auto v3 = getCvPts<cv::Point2dd>( myvec );
\endcode
*/
template<typename RT,typename FPT>
std::vector<RT>
getCvPts( const std::vector<Point2d_<FPT>>& vpt )
{
	std::vector<RT> vout( vpt.size() );
	auto it = vout.begin();
	for( const auto& pt: vpt )
		*it++ = getCvPt<RT>(pt);
	return vout;
}
#endif // HOMOG2D_USE_OPENCV


//------------------------------------------------------------------
/// This namespace holds some private stuff
namespace detail {

//------------------------------------------------------------------
/// Private free function, returns true if point \c pt is inside the rectangle defined by (\c p00 , \c p11)
template<typename FPT1,typename FPT2>
bool
ptIsInside( const Point2d_<FPT1>& pt, const Point2d_<FPT2>& p00, const Point2d_<FPT2>& p11 )
{
//	if( pt.getX() >= p00.getX() && pt.getX() <= p11.getX() )
//		if( pt.getY() >= p00.getY() && pt.getY() <= p11.getY() )
	if( pt.getX() > p00.getX() && pt.getX() < p11.getX() )
		if( pt.getY() > p00.getY() && pt.getY() < p11.getY() )
			return true;
	return false;
}


#ifdef HOMOG2D_USE_EIGEN
///  Build Homography from 2 sets of 4 points, using Eigen
/**
See
- https://eigen.tuxfamily.org/dox/group__TutorialLinearAlgebra.html
- https://eigen.tuxfamily.org/dox/group__DenseMatrixManipulation__chapter.html
*/
template<typename FPT>
Homogr_<FPT>
buildFrom4Points_Eigen(
	const std::vector<Point2d_<FPT>>& vpt1, ///< source points
	const std::vector<Point2d_<FPT>>& vpt2  ///< destination points
)
{
	Eigen::MatrixXd A = Eigen::MatrixXd::Zero(8,8);
	Eigen::VectorXd b(8);

	for( int i=0; i<4; i++ )
	{
		auto u1 = vpt1[i].getX();
		auto v1 = vpt1[i].getY();
		auto u2 = vpt2[i].getX();
		auto v2 = vpt2[i].getY();

		b(2*i)   = u2;
		b(2*i+1) = v2;

		A(i*2,0) = A(i*2+1,3) = u1;
		A(i*2,1) = A(i*2+1,4) = v1;
		A(i*2,2) = A(i*2+1,5) = 1.;

		A(i*2,6)   = - u1 * u2;
		A(i*2,7)   = - v1 * u2;
		A(i*2+1,6) = - u1 * v2;
		A(i*2+1,7) = - v1 * v2;
	}


#if 0
	Eigen::VectorXd X = A.ldlt().solve(b); // for some reason this does not work...
#else
	Eigen::MatrixXd Ai = A.inverse();
	Eigen::VectorXd X = Ai * b;
#endif

	Homogr_<FPT> H;
	for( int i=0; i<8; i++ )
		H.set( i/3, i%3, X(i) );
	H.set(2, 2, 1.);

	return H;
}
#endif

//------------------------------------------------------------------
#ifdef HOMOG2D_USE_OPENCV
///  Build Homography from 2 sets of 4 points, using Opencv
/**
- see https://docs.opencv.org/master/d9/d0c/group__calib3d.html#ga4abc2ece9fab9398f2e560d53c8c9780

\note With current Opencv installed on current machine, it seems that \c cv::getPerspectiveTransform()
requires that the points are "CV_32F" (\c float), and NOT double.
*/
template<typename FPT>
Homogr_<FPT>
buildFrom4Points_Opencv (
	const std::vector<Point2d_<FPT>>& vpt1, ///< source points
	const std::vector<Point2d_<FPT>>& vpt2  ///< destination points
)
{
	const auto& src = getCvPts<cv::Point2f>( vpt1 );
	const auto& dst = getCvPts<cv::Point2f>( vpt2 );
	return cv::getPerspectiveTransform( src, dst ); // automatic type conversion to Hmatrix_
}
#endif

} // namespace detail

//------------------------------------------------------------------
/// Build Homography from 2 sets of 4 points (free function)
template<typename FPT>
Homogr_<FPT>
buildFrom4Points(
	const std::vector<Point2d_<FPT>>& vpt1,     ///< source points
	const std::vector<Point2d_<FPT>>& vpt2,     ///< destination points
	int                               method=1  ///< 0: Eigen, 1: Opencv
)
{
	Homogr_<FPT> H;
	H.buildFrom4Points( vpt1, vpt2, method );
	return H;
}

//------------------------------------------------------------------
/// Build Homography from 2 sets of 4 points
/**
- Requires either Eigen or Opencv
- we build a 8x8 matrix A and a 8x1 vector B, and get the solution from X = A^-1 B
- see this for details:
https://skramm.lautre.net/files/misc/Kramm_compute_H_from_4pts.pdf

\sa free function: h2d::buildFrom4Points()

\todo fix this so that user can provide a std::array of points
*/
template<typename M,typename FPT>
void
Hmatrix_<M,FPT>::buildFrom4Points(
	const std::vector<Point2d_<FPT>>& vpt1,   ///< source points
	const std::vector<Point2d_<FPT>>& vpt2,   ///< destination points
	int                               method  ///< 0: Eigen, 1: Opencv (default)
)
{
	if( vpt1.size() != 4 )
		HOMOG2D_THROW_ERROR_1( "invalid vector size for source points, should be 4, value=" + std::to_string(vpt1.size()) );
	if( vpt2.size() != 4 )
		HOMOG2D_THROW_ERROR_1( "invalid vector size for dest points, should be 4, value=" + std::to_string(vpt2.size()) );
	assert( method == 0 || method == 1 );

	if( method == 0 )
	{
#ifdef HOMOG2D_USE_EIGEN
		*this = detail::buildFrom4Points_Eigen( vpt1, vpt2 );
#else
		throw std::runtime_error( "Unable, build without Eigen support" );
#endif
	}
	else
	{
#ifdef HOMOG2D_USE_OPENCV
		*this = detail::buildFrom4Points_Opencv( vpt1, vpt2 );
#else
		throw std::runtime_error( "Unable, build without Opencv support" );
#endif
	}
}

//------------------------------------------------------------------
/// A line segment, defined by two points
/**
- Storage: "smallest" point is always stored as first element (see constructor)
*/
template<typename FPT>
class Segment_
{
	template<typename T> friend class Segment_;

private:
	Point2d_<FPT> _ptS1, _ptS2;

public:
/// Default constructor: initializes segment to (0,0)--(1,1)
	Segment_(): _ptS2(1.,1.)
	{}
/// Contructor 2: build segment from two points
	Segment_( Point2d_<FPT> p1, Point2d_<FPT> p2 )
		: _ptS1(p1), _ptS2(p2)
	{
#ifndef HOMOG2D_NOCHECKS
		if( p1 == p2 )
			HOMOG2D_THROW_ERROR_1( "cannot build a segment with two identical points" );
#endif
		priv::fix_order( _ptS1, _ptS2 );
	}

/// Contructor 3: build segment from two points coordinates
	template<typename T>
	Segment_( T x1, T y1, T x2, T y2 )
	{
		HOMOG2D_CHECK_IS_NUMBER(T);
		_ptS1.set( x1, y1 );
		_ptS2.set( x2, y2 );
#ifndef HOMOG2D_NOCHECKS
		if( _ptS1 == _ptS2 )
			HOMOG2D_THROW_ERROR_1( "cannot build a segment with two identical points" );
#endif
		priv::fix_order( _ptS1, _ptS2 );
	}

/// Setter
	void set( const Point2d_<FPT>& p1, const Point2d_<FPT>& p2 )
	{
#ifndef HOMOG2D_NOCHECKS
		if( p1 == p2 )
			HOMOG2D_THROW_ERROR_1( "cannot define a segment with two identical points" );
#endif
		_ptS1 = p1;
		_ptS2 = p2;
		priv::fix_order( _ptS1, _ptS2 );
	}

/// Get segment length
	HOMOG2D_INUMTYPE length() const
	{
		return _ptS1.distTo( _ptS2 );
	}

/// Returns Bounding Box
	FRect_<FPT> getBB() const
	{
		return FRect_<FPT>(	_ptS1. _ptS2 );
	}

/// Get angle between segment and other segment/line
	template<typename U>
	HOMOG2D_INUMTYPE getAngle( const U& other ) const
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

/// Returns the points as a std::pair
/** The one with smallest x coordinate will be returned as "first". If x-coordinate are equal, then
the one with smallest y-coordinate will be returned first */
	std::pair<Point2d_<FPT>,Point2d_<FPT>>
	getPts() const
	{
		return std::make_pair( _ptS1, _ptS2 );
	}

/// Segment "isInside", S can be Circle or FRect
	template<typename S>
	bool isInside( const S& shape ) const
	{
		if( !_ptS1.isInside( shape ) )
			return false;
		if( !_ptS2.isInside( shape ) )
			return false;
		return true;
	}

/// Returns supporting line
	Line2d_<FPT> getLine() const
	{
		return _ptS1 * _ptS2;
	}

	template<typename FPT2>
	detail::Intersect<detail::Inters_1,FPT> intersects( const Segment_<FPT2>& ) const;
	template<typename FPT2>
	detail::Intersect<detail::Inters_1,FPT> intersects( const Line2d_<FPT2>&  ) const;
	template<typename FPT2>
	detail::IntersectM<FPT>                 intersects( const Circle_<FPT2>&  ) const;
/// Segment/FRect intersection
	template<typename FPT2>
	detail::IntersectM<FPT> intersects( const FRect_<FPT2>& r ) const
	{
		HOMOG2D_START;
		return r.intersects( *this );
	}

/// Segment/Polyline intersection
	template<typename FPT2>
	detail::IntersectM<FPT> intersects( const Polyline_<FPT2>& other ) const
	{
		HOMOG2D_START;
		return other.intersects( *this );
	}

	template<typename T>
	bool isParallelTo( const T& other ) const
	{
		static_assert(
			std::is_same<T,Segment_<FPT>>::value ||
			std::is_same<T,Line2d_<FPT>>::value,
			"type needs to be a segment or a line" );
		return getLine().isParallelTo( other );
	}

	/// Returns point that at middle distance between \c p1 and \c p2
	Point2d_<FPT>
	getMiddlePoint() const
	{
		return Point2d_<FPT>(
			( _ptS1.getX() + _ptS2.getX() ) / 2.,
			( _ptS1.getY() + _ptS2.getY() ) / 2.
		);
	}

#ifdef HOMOG2D_USE_OPENCV
	void draw( cv::Mat& mat, CvDrawParams dp=CvDrawParams() )
	{
		cv::line( mat, _ptS1.getCvPtd(), _ptS2.getCvPtd(), dp._dpValues._color, dp._dpValues._lineThickness, dp._dpValues._lineType );
	}
#endif

	friend std::ostream&
	operator << ( std::ostream& f, const Segment_<FPT>& seg )
	{
		f << seg._ptS1 << "-" << seg._ptS2;
		return f;
	}
};

//------------------------------------------------------------------
/// Returns the points as a std::pair (free function)
/// \sa Segment_::getPts()
template<typename FPT>
std::pair<Point2d_<FPT>,Point2d_<FPT>>
getPts( const Segment_<FPT>& seg )
{
	return seg.getPts();
}

/// Returns supporting line (free function)
template<typename FPT>
Line2d_<FPT> getLine( const Segment_<FPT>& seg )
{
	return seg.getLine();
}

//------------------------------------------------------------------
/// Free function, returns segment between two circle centers
template<typename FPT1,typename FPT2>
Segment_<FPT1>
getSegment( const Circle_<FPT1>& c1, const Circle_<FPT2>& c2 )
{
	return Segment_<FPT1>( c1.center(), c2.center() );
}

/// Free function, returns line between two circle centers
template<typename FPT1,typename FPT2,typename FPT3>
Line2d_<FPT1>
getLine( const Circle_<FPT2>& c1, const Circle_<FPT3>& c2 )
{
	return Line2d_<FPT1>( c1.center(), c2.center() );
}

/// Free function, returns middle point of segment
/// \sa Segment_::getMiddlePoint()
template<typename FPT>
Point2d_<FPT>
getMiddlePoint( const Segment_<FPT>& seg )
{
	return seg.getMiddlePoint();
}

/// Free function, returns segments of the rectangle
/// \sa FRect_::getSegs()
template<typename FPT>
std::array<Segment_<FPT>,4>
getSegs( const FRect_<FPT>& seg )
{
	return seg.getSegs();
}

/// Free function, returns the pair of segments tangential to the two circles
template<typename FPT1,typename FPT2>
std::pair<Segment_<FPT1>,Segment_<FPT1>>
getTanSegs( const Circle_<FPT1>& c1, const Circle_<FPT2>& c2 )
{
#ifndef HOMOG2D_NOCHECKS
	if( c1 == c2 )
		HOMOG2D_THROW_ERROR_1( "c1 and c2 identical" );
#endif

	auto li0 = Line2d_<FPT1>( c1.center(), c2.center() );
	auto li1 = li0.getOrthogonalLine( c1.center() );
	auto li2 = li0.getOrthogonalLine( c2.center() );

	const auto ri1 = li1.intersects( c1 );
	const auto ri2 = li2.intersects( c2 );
	assert( ri1() && ri2() );
	const auto& ppts1 = ri1.get();
	const auto& ppts2 = ri2.get();

	return std::make_pair(
		Segment_<FPT1>( ppts1.first,  ppts2.first  ),
		Segment_<FPT1>( ppts1.second, ppts2.second )
	);
}

/// Circle/Circle intersection
/**
Ref:
- https://stackoverflow.com/questions/3349125/

\todo benchmark the two approaches below
*/
template<typename FPT>
template<typename FPT2>
detail::Intersect<detail::Inters_2,FPT>
Circle_<FPT>::intersects( const Circle_<FPT2>& other ) const
{
	HOMOG2D_START;

	if( *this == other )
		return detail::Intersect<detail::Inters_2,FPT>();

	HOMOG2D_INUMTYPE r1 = _radius;
	HOMOG2D_INUMTYPE r2 = other._radius;
	Point2d_<HOMOG2D_INUMTYPE> pt1 = _center;
	Point2d_<HOMOG2D_INUMTYPE> pt2 = other._center;

#if 0
	HOMOG2D_INUMTYPE d  = _center.distTo( other._center );
	if( d > r1 + r2 )                                     // no intersection
		return detail::Intersect<detail::Inters_2,FPT>();

	if( d < std::abs( r1 - r2 ) )                         // no intersection: one circle inside the other
		return detail::Intersect<detail::Inters_2,FPT>();
	auto a = (r1*r1 - r2*r2 + d*d) / 2. / d;
#else
	HOMOG2D_INUMTYPE x1 = pt1.getX();
	HOMOG2D_INUMTYPE y1 = pt1.getY();
	HOMOG2D_INUMTYPE x2 = pt2.getX();
	HOMOG2D_INUMTYPE y2 = pt2.getY();
	HOMOG2D_INUMTYPE d_squared = (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2);

	if( d_squared > r1*r1 + r2*r2 + 2.*r1*r2 )              // no intersection
		return detail::Intersect<detail::Inters_2,FPT>();

	if( d_squared < ( r1*r1 + r2*r2 - 2.*r1*r2 ) )          // no intersection: one circle inside the other
		return detail::Intersect<detail::Inters_2,FPT>();

	auto d = std::sqrt( d_squared );
	auto a = (r1*r1 - r2*r2 + d_squared) / 2. / d;
#endif

	auto h = std::sqrt( r1*r1 - a*a );

	Point2d_<FPT> P0(
		( pt2.getX() - pt1.getX() ) * a / d + pt1.getX(),
		( pt2.getY() - pt1.getY() ) * a / d + pt1.getY()
	);

	Point2d_<FPT> pt3(
		P0.getX() + h*( pt1.getY() - pt2.getY() ) / d,
		P0.getY() - h*( pt1.getX() - pt2.getX() ) / d
	);
	Point2d_<FPT> pt4(
		P0.getX() - h*( pt1.getY() - pt2.getY() ) / d,
		P0.getY() + h*( pt1.getX() - pt2.getX() ) / d
	);
	return detail::Intersect<detail::Inters_2,FPT>( pt3, pt4 );
}

//------------------------------------------------------------------
namespace priv {

/// Traits class, used in intersects() for Polyline
template<typename T> struct IsShape              : std::false_type {};
template<typename T> struct IsShape<Circle_<T>>  : std::true_type  {};
template<typename T> struct IsShape<FRect_<T>>   : std::true_type  {};
template<typename T> struct IsShape<Segment_<T>> : std::true_type  {};
template<typename T> struct IsShape<Line2d_<T>>  : std::true_type  {};

/// Traits class used in operator * ( const Hmatrix_<type::IsHomogr,FPT>& h, const Cont& vin ),
/// used to detect if container is valid
template <typename T>               struct Is_Container: std::false_type { };
template <typename T,std::size_t N> struct Is_Container<std::array<T,N>>:     std::true_type { };
template <typename... Ts>           struct Is_Container<std::vector<Ts...>> : std::true_type { };
template <typename... Ts>           struct Is_Container<std::list<Ts...  >> : std::true_type { };

/// Traits class used to detect if container \c T is a \c std::array
/** (because allocation is different, see \ref alloc() ) */
template <typename T>
struct Is_std_array: std::false_type {};
template <typename V, size_t n>
struct Is_std_array<std::array<V, n>>: std::true_type {};

} // namespace priv

//------------------------------------------------------------------
/// Used in Polyline_ constructors
enum class IsClosed: char
{
	Yes, No
};
//------------------------------------------------------------------
/// Polyline, can be closed or not
template<typename FPT>
class Polyline_
{
	private:
		std::vector<Point2d_<FPT>> _plinevec;
		bool _isClosed = false;

	public:
/// Default constructor
		Polyline_( IsClosed ic = IsClosed::No )
		{
			if( ic == IsClosed::Yes )
				_isClosed = true;
		}
/// Constructor for single point
		template<typename FPT2>
		Polyline_( const Point2d_<FPT2>& pt, IsClosed ic = IsClosed::No )
		{
			_plinevec.push_back( pt );
			if( ic == IsClosed::Yes )
				_isClosed = true;
		}
/// Constructor for single point as x,y
		template<typename FPT2>
		Polyline_( FPT2 x, FPT2 y, IsClosed ic = IsClosed::No )
		{
			_plinevec.push_back( Point2d_<FPT>(x,y) );
			if( ic == IsClosed::Yes )
				_isClosed = true;
		}
/// Constructor from FRect
		template<typename FPT2>
		Polyline_( const FRect_<FPT2>& rect, IsClosed ic = IsClosed::No )
		{
			for( const auto& pt: rect.get4Pts() )
				_plinevec.push_back( pt );
			if( ic == IsClosed::Yes )
				_isClosed = true;
		}

/// Returns the number of points (not segments!)
		size_t size() const { return _plinevec.size(); }

/// Returns length
		HOMOG2D_INUMTYPE length() const
		{
			HOMOG2D_INUMTYPE sum = 0.L;
			for( const auto& seg: getSegs() )
				sum += static_cast<HOMOG2D_INUMTYPE>( seg.length() );
			return sum;
		}

		FRect_<FPT> getBB() const;

/// Returns the number of segments
		size_t nbSegs() const
		{
			if( size() == 0 )
				return 0;
			if( size() < 3 )     // if 1 or 2, then 0 or 1 segment
				return size() - 1;
			return size() - 1 + (size_t)_isClosed;
		}

/// Returns the points
		std::vector<Point2d_<FPT>>& getPts()
		{
			return _plinevec;
		}
/// Returns the points (const)
		const std::vector<Point2d_<FPT>>& getPts() const
		{
			return _plinevec;
		}

/// Returns the segments of the polyline
		std::vector<Segment_<FPT>> getSegs() const
		{
			std::vector<Segment_<FPT>> out;
			if( size() < 2 ) // nothing to draw
				return out;

			for( size_t i=0; i<size()-1; i++ )
			{
				const auto& pt1 = _plinevec[i];
				const auto& pt2 = _plinevec[i+1];
				out.push_back( Segment_<FPT>( pt1,pt2) );
			}
			if( _isClosed )
				out.push_back( Segment_<FPT>(_plinevec.front(),_plinevec.back() ) );
			return out;
		}

/// Returns one point of the polyline.
		Point2d_<FPT> getPoint( size_t idx ) const
		{
#ifndef HOMOG2D_NOCHECKS
			if( idx >= size() )
				HOMOG2D_THROW_ERROR_1( "requesting point " + std::to_string(idx)
					+ ", only has "  + std::to_string(size()) );
#endif
			return _plinevec[idx];
		}

/// Returns one segment of the polyline.
/**
Segment \c n is the one between point \c n and point \c n+1
*/
		Segment_<FPT> getSegment( size_t idx ) const
		{
#ifndef HOMOG2D_NOCHECKS
			if( idx >= nbSegs() )
				HOMOG2D_THROW_ERROR_1( "requesting segment " + std::to_string(idx)
					+ ", only has "  + std::to_string(nbSegs()) );

			if( size() < 2 ) // nothing to draw
				HOMOG2D_THROW_ERROR_1( "no segment "  + std::to_string(idx) );
#endif
//			auto lastPoint = _isClosed?
			return Segment_<FPT>(
				_plinevec[idx],
				_plinevec[idx+1==nbSegs()&&_isClosed?0:idx+1]
			);
		}

/// Clear all
		void clear() { _plinevec.clear(); }

/// Add single point as x,y
		template<typename FPT1,typename FPT2>
		void add( FPT1 x, FPT2 y )
		{
			add( Point2d_<FPT>( x, y ) );
		}

/// Add single point
		template<typename FPT2>
		void add( const Point2d_<FPT2>& pt )
		{
#ifndef HOMOG2D_NOCHECKS
			if( size() )
				if( pt == _plinevec.back() )
					HOMOG2D_THROW_ERROR_1( "cannot add a point identical to previous one" );
#endif
			_plinevec.push_back( pt );
		}

/// Set from vector of points
		template<typename FPT2>
		void set( const std::vector<Point2d_<FPT2>>& vec )
		{
			_plinevec.resize( vec.size() );
			auto it = std::begin( _plinevec );
			for( const auto& pt: vec )   // copying one by one will
				*it++ = pt;              // allow type conversions (std::copy implies same type)
		}

/// Add vector of points
		template<typename FPT2>
		void add( const std::vector<Point2d_<FPT2>>& vec )
		{
			if( vec.size() == 0 )
				return;
//			auto it = std::end( _plinevec );
			std::cout << "resizing to " << _plinevec.size() << "+" << vec.size() << '\n';
			_plinevec.reserve( _plinevec.size() + vec.size() );
			for( const auto& pt: vec )  // we cannot use std::copy because vec might not hold points of same type
				_plinevec.push_back( pt );
		}

		const bool& isClosed() const { return _isClosed; }
		bool& isClosed()             { return _isClosed; }

/// Polyline intersection with Line, Segment, FRect, Circle
		template<
			typename T,
			typename std::enable_if<
				priv::IsShape<T>::value,
				T
			>::type* = nullptr
		>
		detail::IntersectM<FPT> intersects( const T& other ) const
		{
			detail::IntersectM<FPT> out;
			for( const auto& pseg: getSegs() )
			{
				auto inters = pseg.intersects( other );
				if( inters() )
					out.add( inters.get() );
			}
			return out;
		}

		template<typename T>
		bool
		isInside( const T& cont )
		{
			for( const auto& pt: getPts() )
				if( !pt.isInside( cont ) )
					return false;
			return true;
		}

		bool isPolygon() const;

		friend std::ostream&
		operator << ( std::ostream& f, const Polyline_<FPT>& pl )
		{
			f << "Polyline: ";
			for( const auto& pt: pl._plinevec )
				f << pt << "-";
			f << (pl._isClosed ? "CLOSED" : "NOT-CLOSED");

			return f;
		}

#ifdef HOMOG2D_USE_OPENCV
		void draw( cv::Mat& mat, CvDrawParams dp=CvDrawParams() )
		{
			if( size() < 2 ) // nothing to draw
				return;

			for( size_t i=0; i<size()-1; i++ )
			{
				const auto& pt1 = _plinevec[i];
				const auto& pt2 = _plinevec[i+1];
				Segment_<FPT>(pt1,pt2).draw( mat, dp );
			}
			if( size() < 3 ) // nothing to draw
				return;
			if( _isClosed )
				Segment_<FPT>(_plinevec.front(),_plinevec.back() ).draw( mat, dp );
		}
#endif
};

//------------------------------------------------------------------
namespace priv {

/// Returns the bounding box of points in vector/list/array \c vpts
/**
\todo This loops twice on the points. Maybe some improvement here.
*/
template<
	typename T,
	typename std::enable_if<
		priv::Is_Container<T>::value,
		T
	>::type* = nullptr
>
FRect_<typename T::value_type::FType>
getPointsBB( const T& vpts )
{
	using FPT = typename T::value_type::FType;
#ifndef HOMOG2D_NOCHECKS
	if( vpts.empty() )
		HOMOG2D_THROW_ERROR_1( "cannot get bounding box of empty set !"	);
#endif
	auto mm_x = std::minmax_element(
		std::begin( vpts ),
		std::end( vpts ),
		[]                  // lambda
		( const Point2d_<FPT>& pt1, const Point2d_<FPT>& pt2 )
		{
			return pt1.getX() < pt2.getX();
		}
	);
	auto mm_y = std::minmax_element(
		std::begin( vpts ),
		std::end( vpts ),
		[]                  // lambda
		( const Point2d_<FPT>& pt1, const Point2d_<FPT>& pt2 )
		{
			return pt1.getY() < pt2.getY();
		}
	);

	return FRect_<typename T::value_type::FType>(
		mm_x.first->getX(),  mm_y.first->getY(),
		mm_x.second->getX(), mm_y.second->getY()
	);
}

} // namespace priv

//------------------------------------------------------------------
/// Returns true if is a polygon (i.e. no segment crossing)
template<typename FPT>
bool
Polyline_<FPT>::isPolygon() const
{
	if( !_isClosed )   // cant be a polygon if
		return false;  // it's not closed !

	auto nbs = nbSegs();
	for( size_t i=0; i<nbs; i++ )
	{
		auto seg1 = getSegment(i);
		for( auto j=i+2; j<nbs-1; j++ )
			if( getSegment(j).intersects(seg1)() )
				return false;
	}
	return true;
}

/// Returns true if is a polygon (free function)
///  \sa Polyline_::isPolygon()
template<typename FPT>
bool
isPolygon( const Polyline_<FPT>& pl )
{
	return pl.isPolygon();
}

/// Returns the number of segments (free function)
/// \sa Polyline_::nbSegs()
template<typename FPT>
size_t nbSegs( const Polyline_<FPT>& pl )
{
	return pl.nbSegs();
}

/// Get segment length (free function)
/// \sa Segment_::length()
template<typename FPT>
HOMOG2D_INUMTYPE
length( const Segment_<FPT>& seg )
{
	return seg.length();
}

/// Returns length (free function)
/// \sa Polyline_::length()
template<typename FPT>
HOMOG2D_INUMTYPE
length( const Polyline_<FPT>& pl )
{
	return pl.length();
}

/// Returns the segments of the polyline (free function)
/// \sa Polyline_::getSegs()
template<typename FPT>
std::vector<Segment_<FPT>>
getSegs( const Polyline_<FPT>& pl )
{
	return pl.getSegs();
}

/// Returns the number of points (free function)
/// \sa Polyline_::size()
template<typename FPT>
size_t size( const Polyline_<FPT>& pl )
{
	return pl.size();
}


/// Returns Bounding Box (free function)
/// \sa Segment_::getBB()
template<typename FPT>
FRect_<FPT>
getBB( const Segment_<FPT>& seg )
{
	return seg.getBB();
}

/// Returns Bounding Box (free function)
/// \sa Circle_::getBB()
template<typename FPT>
FRect_<FPT>
getBB( const Circle_<FPT>& cir )
{
	return cir.getBB();
}

/// Returns Bounding Box (free function)
/// \sa FRect_::getBB()
template<typename FPT>
FRect_<FPT>
getBB( const Polyline_<FPT>& pl )
{
	return pl.getBB();
}

/// Returns Bounding Box of arbitrary container holding points (free function)
template<
	typename T,
	typename std::enable_if<
		priv::Is_Container<T>::value,
		T
	>::type* = nullptr
>
//FRect_<typename T::value_type::FType> // if we dont have C++14 but only C++11
auto
getBB( const T& vpts )
{
	return priv::getPointsBB( vpts );
}


/// Returns Bounding Box
template<typename FPT>
FRect_<FPT>
Polyline_<FPT>::getBB() const
{
	return priv::getPointsBB( getPts() );
}


//------------------------------------------------------------------
namespace detail {

/// Used in isBetween()
enum class Rounding: char { Yes, No };

/// Helper function
template<typename T1,typename T2>
bool
isBetween( T1 v, T2 v1, T2 v2 )
{
	HOMOG2D_CHECK_IS_NUMBER(T1);
	HOMOG2D_CHECK_IS_NUMBER(T2);
//std::cout << std::scientific << std::setprecision(18);
//HOMOG2D_LOG("v="<<v << " v1=" << v1 << " v2=" << v2 );
	if( v >= std::min( v1, v2 ) )
		if( v <= std::max( v1, v2 ) )
			return true;
	return false;
}

/// Does some small rounding (if requested), to avoid some numerical issues
/// \todo provide access to the coefficient in API
template<typename FPT>
long double
doRounding( FPT value, Rounding r )
{
	if( r == Rounding::No )
		return value;
	long double coeff=1E6;
	return std::llroundl( value * coeff ) / coeff;
}

/// Helper function, checks if \c pt is in the area defined by \c pt1 and \c pt2
template<typename T1,typename T2>
bool
isInArea(
	const Point2d_<T1>& pt,
	const Point2d_<T2>& pt1,
	const Point2d_<T2>& pt2,
	Rounding r = Rounding::No
)
{
	HOMOG2D_START;
	if( isBetween( doRounding(pt.getX(), r), pt1.getX(), pt2.getX() ) )
		if( isBetween( doRounding(pt.getY(), r), pt1.getY(), pt2.getY() ) )
			return true;
	return false;
}

//------------------------------------------------------------------
/// See getPtLabel( const Point2d_<FPT>& pt, const Circle_<FPT2>& circle )
enum class PtTag: char
{
	Inside, Outside, OnEdge
};

/// Returns a label characterizing point \c pt, related to \c circle
/// \todo use HOMOG2D_INUMTYPE for computation
template<typename FPT,typename FPT2>
PtTag
getPtLabel( const Point2d_<FPT>& pt, const Circle_<FPT2>& circle )
{
	if( pt.isInside( circle ) )
		return PtTag::Inside;
	if(
		std::abs(
			static_cast<HOMOG2D_INUMTYPE>(pt.distTo( circle.center() ) ) - circle.radius()
		)
		< Point2d_<FPT>::nullDistance()
	)
		return PtTag::OnEdge;
	return PtTag::Outside;
}
#if 0
/// Debug, can be removed after
void
printTag( std::string txt, PtTag tag )
{
	std::cout << "point " << txt << ": ";
	switch( tag )
	{
		case PtTag::Inside: std::cout << "Inside\n"; break;
		case PtTag::Outside: std::cout << "Outside\n"; break;
		case PtTag::OnEdge: std::cout << "OnEdge\n"; break;
	}
}
#endif
} // namespace detail

//------------------------------------------------------------------
/// Segment/Segment intersection
/**
Algorithm:<br>
We check if the intersection point lies in between the range of both segments, both on x and on y
*/
template<typename FPT>
template<typename FPT2>
detail::Intersect<detail::Inters_1,FPT>
Segment_<FPT>::intersects( const Segment_<FPT2>& s2 ) const
{
	HOMOG2D_START;
	if( *this == s2 )              // same segment => no intersection
		return detail::Intersect<detail::Inters_1,FPT>();

	Line2d_<HOMOG2D_INUMTYPE> l1 = getLine();
	Line2d_<HOMOG2D_INUMTYPE> l2 = s2.getLine();
	if( l1.isParallelTo( l2 ) )                                // if parallel,
			return detail::Intersect<detail::Inters_1,FPT>();  // then, no intersection

	const auto& ptA1 = getPts().first;
	const auto& ptA2 = getPts().second;
	const auto& ptB1 = s2.getPts().first;
	const auto& ptB2 = s2.getPts().second;

	auto ptInter = l1 * l2;   // intersection point

	if( detail::isInArea( ptInter, ptA1, ptA2 ) )
		if( detail::isInArea( ptInter, ptB1, ptB2 ) )
			return detail::Intersect<detail::Inters_1,FPT>( ptInter );

	return detail::Intersect<detail::Inters_1,FPT>(); // no intersection
}

//------------------------------------------------------------------
/// Segment/Line intersection
/**
Algorithm:<br>
We check if the intersection point lies in between the range of the segment, both on x and on y
*/
template<typename FPT>
template<typename FPT2>
detail::Intersect<detail::Inters_1,FPT>
Segment_<FPT>::intersects( const Line2d_<FPT2>& li1 ) const
{
	HOMOG2D_START;
	HOMOG2D_LOG( "seg=" << *this << " line=" << li1 );
	detail::Intersect<detail::Inters_1,FPT> out;
	auto li2 = getLine();

	if( li1.isParallelTo( li2 ) ) // if parallel, no intersection
		return out;

	out._ptIntersect = li1 * li2;   // intersection point

	const auto& pi   = out._ptIntersect;
	const auto& ptA1 = getPts().first;
	const auto& ptA2 = getPts().second;

	HOMOG2D_LOG( "pi=" << pi << " ptA1=" <<ptA1  << " ptA2=" <<ptA2 );
	if( detail::isInArea( pi, ptA1, ptA2, detail::Rounding::Yes ) )
	{
		HOMOG2D_LOG( "Is in area" );
		out._doesIntersect = true;
	}
	else
		HOMOG2D_LOG( "Is NOT in area" );
	return out;
}

//------------------------------------------------------------------
/// Segment/Circle intersection
/**
For each point of the segment, we need to consider 3 different situations
 - point is inside (PI)
 - point is outside (PO)
 - point is on the edge (PE)

That makes 6 different situations to handle:

 - S1: PI-PI => no intersection
 - S2: PI-PO => 1 intersection
 - S3: PI-PE => 1 intersection
 - S4: PO-PO => depends on the support line:
  - S4A: if line does NOT intersects circle, no intersection pts
  - S4B: if line does intersects circle, and intersections point in segment area => 2 intersection pts
  - S4C: if line does intersects circle, and intersections point NOT in segment area => no intersection pts
 - S5: PO-PE => 1 intersection
 - S6: PE-PE => 2 intersections
*/
template<typename FPT>
template<typename FPT2>
detail::IntersectM<FPT>
Segment_<FPT>::intersects( const Circle_<FPT2>& circle ) const
{
	HOMOG2D_START;
	using detail::PtTag;

	auto tag_ptS1 = detail::getPtLabel( _ptS1, circle );
	auto tag_ptS2 = detail::getPtLabel( _ptS2, circle );

	if( tag_ptS1 == PtTag::Inside )
		if( tag_ptS2 == PtTag::Inside )
			return detail::IntersectM<FPT>();

	auto int_lc = getLine().intersects( circle );
	if( !int_lc() )
		return detail::IntersectM<FPT>();

	auto p_pts = int_lc.get();      // get the line intersection points
	const auto& p1 = p_pts.first;
	const auto& p2 = p_pts.second;

	if(
		( tag_ptS1 == PtTag::Inside  && tag_ptS2 == PtTag::Outside )
		||
		( tag_ptS1 == PtTag::Outside && tag_ptS2 == PtTag::Inside )
	)
	{
		detail::IntersectM<FPT> out;
        if( detail::isInArea( p1, _ptS1, _ptS2 ) )  // check which one of the intersections
			out.add( p1 );                          // points is inside
		else
			out.add( p2 );
		return out;
	}

	detail::IntersectM<FPT> out;
	if( tag_ptS1 == PtTag::Outside &&  tag_ptS2 == PtTag::Outside ) // both outside
	{
		if( !detail::isInArea( p1, _ptS1, _ptS2 ) ) //could have done for p2, doesn't matter
			return detail::IntersectM<FPT>();
		out.add( p1 );
		out.add( p2 );
		return out;
	}

	if( tag_ptS1 == PtTag::OnEdge )
		out.add( _ptS1 );
	if( tag_ptS2 == PtTag::OnEdge )
		out.add( _ptS2 );

	return out;
}

//------------------------------------------------------------------
/// Overload for points
template<typename LP,typename FPT>
void
Root<LP,FPT>::impl_op_stream( std::ostream& f, const Point2d_<FPT>& r ) const
{
	f
//	<< std::scientific << std::setprecision(25)
	 << '[' << r.getX() << ',' << r.getY() << "]";
}

/// Overload for lines
template<typename LP,typename FPT>
void
Root<LP,FPT>::impl_op_stream( std::ostream& f, const Line2d_<FPT>& r ) const
{
	f << '[' << r._v[0] << ',' << r._v[1] << ',' << r._v[2] << "]";
}

/// Stream operator, free function, call member function pseudo operator impl_op_stream()
template<typename LP,typename FPT>
std::ostream&
operator << ( std::ostream& f, const Root<LP,FPT>& r )
{
	r.impl_op_stream( f, r );
	return f;
}

/// Stream operator for a container of points/lines, free function
template<
	typename T,
	typename std::enable_if<
		priv::Is_Container<T>::value,
		T
	>::type* = nullptr
>
std::ostream&
operator << ( std::ostream& f, const T& vec )
{
	for( const auto& elem: vec )
		f << elem << '\n';
	return f;
}

//------------------------------------------------------------------
/// Normalize to unit length, and make sure \c a is always >0
/**
\todo Checkout: in what situation will we be unable to normalize?
Is the test below relevant? Clarify.
*/
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
Root<LP,FPT>::impl_getCoord( GivenCoord, FPT, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot call getCoord() on a point" );
}

template<typename LP,typename FPT>
FPT
Root<LP,FPT>::impl_getCoord( GivenCoord gc, FPT other, const detail::RootHelper<type::IsLine>& ) const
{
	const auto a = static_cast<HOMOG2D_INUMTYPE>( _v[0] );
	const auto b = static_cast<HOMOG2D_INUMTYPE>( _v[1] );
	auto denom = ( gc == GivenCoord::X ? b : a );
#ifndef HOMOG2D_NOCHECKS
	if( std::abs(denom) < nullDenom() )
		HOMOG2D_THROW_ERROR_2( "getCoord", "null denominator encoutered" );
#endif
	if( gc == GivenCoord::X )
		return ( -a * other - _v[2] ) / b;
	else
		return ( -b * other - _v[2] ) / a;
}


template<typename LP,typename FPT>
Point2d_<FPT>
Root<LP,FPT>::impl_getPoint( GivenCoord, FPT, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot call getPoint() on a point" );
}

template<typename LP,typename FPT>
Point2d_<FPT>
Root<LP,FPT>::impl_getPoint( GivenCoord gc, FPT other, const detail::RootHelper<type::IsLine>& ) const
{
	auto coord = impl_getCoord( gc, other, detail::RootHelper<type::IsLine>() );
	if( gc == GivenCoord::X )
		return Point2d_<FPT>( other, coord );
	return Point2d_<FPT>( coord, other );
}

//------------------------------------------------------------------
/// ILLEGAL INSTANCIATION
template<typename LP,typename FPT>
template<typename FPT2>
std::pair<Point2d_<FPT>,Point2d_<FPT>>
Root<LP,FPT>::impl_getPoints_A( GivenCoord, FPT, FPT2, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot call getPoints() on a point" );
}
template<typename LP,typename FPT>
template<typename FPT2>
std::pair<Point2d_<FPT>,Point2d_<FPT>>
Root<LP,FPT>::impl_getPoints_B( const Point2d_<FPT>&, FPT2, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot call getPoints() on a point" );
}

/// Returns pair of points on line at distance \c dist from point on line at coord \c coord. Implementation for lines
template<typename LP,typename FPT>
template<typename FPT2>
std::pair<Point2d_<FPT>,Point2d_<FPT>>
Root<LP,FPT>::impl_getPoints_A( GivenCoord gc, FPT coord, FPT2 dist, const detail::RootHelper<type::IsLine>& ) const
{
	const auto pt = impl_getPoint( gc, coord, detail::RootHelper<type::IsLine>() );
	return priv::getPoints_B2( pt, dist, *this );
}

/// Returns pair of points on line at distance \c dist from point on line at coord \c coord. Implementation for lines
template<typename LP,typename FPT>
template<typename FPT2>
std::pair<Point2d_<FPT>,Point2d_<FPT>>
Root<LP,FPT>::impl_getPoints_B( const Point2d_<FPT>& pt, FPT2 dist, const detail::RootHelper<type::IsLine>& ) const
{
#ifndef HOMOG2D_NOCHECKS
	if( this->distTo( pt ) > nullDistance() )
	{
		std::cerr << "distance=" << std::scientific << this->distTo( pt ) << " nD=" << nullDistance() << "\n";
		HOMOG2D_THROW_ERROR_2( "getPoints", "point is not on line" );
	}
#endif

	return priv::getPoints_B2( pt, dist, *this );
}


//------------------------------------------------------------------
/// Illegal instanciation
template<typename LP,typename FPT>
Line2d_<FPT>
Root<LP,FPT>::impl_getOrthogonalLine_A( GivenCoord, FPT, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot call getOrthogonalLine() on a point" );
}

/// Illegal instanciation
template<typename LP,typename FPT>
Line2d_<FPT>
Root<LP,FPT>::impl_getOrthogonalLine_B( const Point2d_<FPT>&, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot call getOrthogonalLine() on a point" );
}

/// Returns an orthogonal line, implementation of getOrthogonalLine().
template<typename LP,typename FPT>
Line2d_<FPT>
Root<LP,FPT>::impl_getOrthogonalLine_A( GivenCoord gc, FPT val, const detail::RootHelper<type::IsLine>& ) const
{
	auto other_val = impl_getCoord( gc, val, detail::RootHelper<type::IsLine>() );

	Point2d_<FPT> pt( other_val, val ) ;
	if( gc == GivenCoord::X )
		pt.set( val, other_val );

	return priv::getOrthogonalLine_B2( pt, *this );
}

/// Returns an orthogonal line, implementation of getOrthogonalLine().
template<typename LP,typename FPT>
Line2d_<FPT>
Root<LP,FPT>::impl_getOrthogonalLine_B( const Point2d_<FPT>& pt, const detail::RootHelper<type::IsLine>& ) const
{
#ifndef HOMOG2D_NOCHECKS
	if( this->distTo( pt ) > nullDistance() )
	{
		std::cerr << "distance=" << std::scientific << this->distTo( pt ) << " nD=" << nullDistance() << "\n";
		HOMOG2D_THROW_ERROR_2( "getOrthogonalLine", "point is not on line" );
	}
#endif

	return priv::getOrthogonalLine_B2( pt, *this );
}

//------------------------------------------------------------------
/// Illegal instanciation
template<typename LP,typename FPT>
Line2d_<FPT>
Root<LP,FPT>::impl_getParallelLine( const Point2d_<FPT>&, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot call getParallelLine() on a point" );
}

/// Returns an parallel line, implementation of getParallelLine().
template<typename LP,typename FPT>
Line2d_<FPT>
Root<LP,FPT>::impl_getParallelLine( const Point2d_<FPT>& pt, const detail::RootHelper<type::IsLine>& ) const
{
	Line2d_<FPT> out = *this;
	out._v[2] = static_cast<HOMOG2D_INUMTYPE>(-_v[0]) * pt.getX() - _v[1] * pt.getY();
	out.p_normalizeLine();
	return out;
}

//------------------------------------------------------------------
/// Illegal instanciation
template<typename LP,typename FPT>
template<typename T>
std::pair<Line2d_<FPT>,Line2d_<FPT>>
Root<LP,FPT>::impl_getParallelLines( T, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot call getParallelLines() on a point" );
}

/// Implementation for lines
template<typename LP,typename FPT>
template<typename T>
std::pair<Line2d_<FPT>,Line2d_<FPT>>
Root<LP,FPT>::impl_getParallelLines( T dist, const detail::RootHelper<type::IsLine>& ) const
{
	Line2d_<FPT> l1 = *this;
	Line2d_<FPT> l2 = *this;
	l1._v[2] = static_cast<HOMOG2D_INUMTYPE>(this->_v[2]) + dist;
	l2._v[2] = static_cast<HOMOG2D_INUMTYPE>(this->_v[2]) - dist;
	return std::make_pair( l1, l2 );
}

//------------------------------------------------------------------
/// Comparison operator, used for lines
/**
Definition: two lines will be equal:
- if they are not parallel
AND
- if their offset (3 third value) is less than nullOffsetValue()
*/
template<typename LP,typename FPT>
bool
Root<LP,FPT>::impl_op_equal( const Root<LP,FPT>& other, const detail::RootHelper<type::IsLine>& ) const
{
	if( !this->isParallelTo( other ) )
		return false;

	if( std::fabs( _v[2] - other._v[2] ) > nullOffsetValue() )
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
/// Sorting operator, for points
template<typename LP,typename FPT>
bool
Root<LP,FPT>::impl_op_sort( const Root<LP,FPT>& other, const detail::RootHelper<type::IsPoint>& ) const
{
	if( getX() < other.getX() )
		return true;
	if( getX() > other.getX() )
		return false;
	if( getY() < other.getY() )
		return true;
	return false;
}
/// Sorting operator, for lines
template<typename LP,typename FPT>
bool
Root<LP,FPT>::impl_op_sort( const Root<LP,FPT>&, const detail::RootHelper<type::IsLine>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid < operator: you cannot sort lines" );
	return false; // to avoid a warning
}

//------------------------------------------------------------------
/// Inner implementation details
namespace detail {

	/// Cross product, see https://en.wikipedia.org/wiki/Cross_product#Coordinate_notation
	template<typename Out,typename In,typename FPT1,typename FPT2>
	Root<Out,FPT1> crossProduct( const Root<In,FPT1>& r1, const Root<In,FPT2>& r2 )
	{
		auto r1_a = static_cast<HOMOG2D_INUMTYPE>(r1._v[0]);
		auto r1_b = static_cast<HOMOG2D_INUMTYPE>(r1._v[1]);
		auto r1_c = static_cast<HOMOG2D_INUMTYPE>(r1._v[2]);
		auto r2_a = static_cast<HOMOG2D_INUMTYPE>(r2._v[0]);
		auto r2_b = static_cast<HOMOG2D_INUMTYPE>(r2._v[1]);
		auto r2_c = static_cast<HOMOG2D_INUMTYPE>(r2._v[2]);

		Root<Out,FPT1> res;
		res._v[0] = static_cast<FPT1>( r1_b * r2_c  - r1_c * r2_b );
		res._v[1] = static_cast<FPT1>( r1_c * r2_a  - r1_a * r2_c );
		res._v[2] = static_cast<FPT1>( r1_a * r2_b  - r1_b * r2_a );

		return res;
	}
} // namespace detail


//------------------------------------------------------------------
///////////////////////////////////////////
// CONSTRUCTORS
///////////////////////////////////////////

/// Points overload: generic init from two numeric args
template<typename LP, typename FPT>
template<typename T1,typename T2>
void
Root<LP,FPT>::impl_init_2( const T1& v1, const T2& v2, const detail::RootHelper<type::IsPoint>& )
{
	_v[0] = v1;
	_v[1] = v2;
	_v[2] = 1.;
}

/// Lines overload: generic init from two numeric args
template<typename LP, typename FPT>
template<typename T1,typename T2>
void
Root<LP,FPT>::impl_init_2( const T1& v1, const T2& v2, const detail::RootHelper<type::IsLine>& )
{
	Point2d_<FPT> pt1;                // 0,0
	Point2d_<FPT> pt2(v1,v2);
	*this = detail::crossProduct<type::IsLine>( pt1, pt2 );
	p_normalizeLine();
}

/// Overload for point to point distance
template<typename LP, typename FPT>
template<typename FPT2>
HOMOG2D_INUMTYPE
Root<LP,FPT>::impl_distToPoint( const Point2d_<FPT2>& pt, const detail::RootHelper<type::IsPoint>& ) const
{
	return static_cast<double>(
		std::hypot(
			static_cast<HOMOG2D_INUMTYPE>( getX() ) - static_cast<HOMOG2D_INUMTYPE>( pt.getX() ),
			static_cast<HOMOG2D_INUMTYPE>( getY() ) - static_cast<HOMOG2D_INUMTYPE>( pt.getY() )
		)
	);
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
template<typename FPT2>
HOMOG2D_INUMTYPE
Root<LP,FPT>::impl_distToPoint( const Point2d_<FPT2>& pt, const detail::RootHelper<type::IsLine>& ) const
{
	return std::fabs( _v[0] * pt.getX() + _v[1] * pt.getY() + _v[2] ) / std::hypot( _v[0], _v[1] );
}

/// overload for line to point distance
template<typename LP, typename FPT>
template<typename FPT2>
HOMOG2D_INUMTYPE
Root<LP,FPT>::impl_distToLine( const Line2d_<FPT2>& li, const detail::RootHelper<type::IsPoint>& ) const
{
	return li.distTo( *this );
}

/// overload for line to line distance. Aborts build if instanciated (distance between two lines makes no sense).
template<typename LP, typename FPT>
template<typename FPT2>
HOMOG2D_INUMTYPE
Root<LP,FPT>::impl_distToLine( const Line2d_<FPT2>&, const detail::RootHelper<type::IsLine>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot compute distance between two lines" );
	return 0.;    // to avoid warning message on build
}

//------------------------------------------------------------------
/// Free function, returns the angle (in Rad) between two lines/ or segments
/// \sa Segment_::getAngle()
/// \sa Line2d_::getAngle()
template<typename T1,typename T2>
HOMOG2D_INUMTYPE
getAngle( const T1& li1, const T2& li2 )
{
	return li1.getAngle( li2 );
}


template<typename LP, typename FPT>
template<typename FPT2>
bool
Root<LP,FPT>::impl_isParallelTo( const Root<LP,FPT2>& li, const detail::RootHelper<type::IsLine>& ) const
{
	if( getAngle(li) < Root::nullAngleValue() )
		return true;
	return false;
}
template<typename LP, typename FPT>
template<typename FPT2>
bool
Root<LP,FPT>::impl_isParallelTo( const Root<LP,FPT2>&, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot use IsParallel() with a point" );
	return false;    // to avoid a warning message on build
}

//------------------------------------------------------------------
/// Returns the angle (in rad.) between the line and the other one.
/**
- The returned value will be in the range <code> [0, M_PI/2] </code>

If the lines \f$ (a_0,a_1,a_2) \f$ and \f$ (b_0,b_1,b_2) \f$ are correctly normalized (should be, but...)
then the angle between them is \f$ \alpha = acos( a_0*b_0 + a_1*b_1) \f$. <br>
However, in "some situations", even if the lines have been previously normalized (which is the case here)
we can encounter numerical issues, so here we "reinforce the normalization and compute:
\f[
\alpha = acos \left(
	\frac{a_0*b_0 + a_1*b_1} { \sqrt{ a_0*a_0 + a_1*a_1 } * \sqrt{ b_0*b_0 + b_1*b_1 } }
\right)
\f]
In some situations, the value inside the parenthesis "may" be equal to \f$ 1+\epsilon \f$
(typically, something like "1.0000000000123").
This is out of bounds for the \f$ acos() \f$ function, that will then return "nan"
(thus induce some failure further on). <br>
To avoid this, a checking is done, and any value higher than 1 will be truncated.
This is logged on \c std::cerr so that the user may take that into consideration.

\todo more investigation needed ! : what are the exact situation that will lead to this event?
*/
template<typename LP, typename FPT>
HOMOG2D_INUMTYPE
Root<LP,FPT>::impl_getAngle( const Root<LP,FPT>& li, const detail::RootHelper<type::IsLine>& ) const
{
	HOMOG2D_INUMTYPE l1_a = _v[0];
	HOMOG2D_INUMTYPE l1_b = _v[1];
	HOMOG2D_INUMTYPE l2_a = li._v[0];
	HOMOG2D_INUMTYPE l2_b = li._v[1];
	HOMOG2D_INUMTYPE res = l1_a * l2_a + l1_b * l2_b;

	res /= std::sqrt( (l1_a*l1_a + l1_b*l1_b) * (l2_a*l2_a + l2_b*l2_b) );
	HOMOG2D_INUMTYPE fres = std::abs(res);
	if( fres > 1.0 )
	{
		std::cerr << "homog2d: angle computation overflow detected, value " << std::setprecision(20) << fres << ", truncated to 1.0\n";
		fres = 1.0;
	}
	return std::acos( fres );
}

template<typename LP, typename FPT>
HOMOG2D_INUMTYPE
Root<LP,FPT>::impl_getAngle( const Root<LP,FPT>&, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "cannot get angle of a point" );
	return 0.; // to avoid a warning
}


//------------------------------------------------------------------
/// Returns true if point is inside (or on the edge) of a flat rectangle defined by (p0,p1)
template<typename LP, typename FPT>
bool
Root<LP,FPT>::impl_isInsideRect( const FRect_<FPT>& rect, const detail::RootHelper<type::IsPoint>& ) const
{
	auto pair_pts = rect.getPts();
	const auto& p00 = pair_pts.first;
	const auto& p11 = pair_pts.second;
	return detail::ptIsInside( *this, p00, p11 );
}
template<typename LP, typename FPT>
bool
Root<LP,FPT>::impl_isInsideRect( const FRect_<FPT>&, const detail::RootHelper<type::IsLine>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "cannot use isInside(Rectangle) with a line" );
	return false; // to avoid a warning
}

template<typename LP, typename FPT>
template<typename T>
bool
Root<LP,FPT>::impl_isInsideCircle( const Point2d_<FPT>& center, T radius, const detail::RootHelper<type::IsPoint>& ) const
{
	if( distTo( center ) < radius )
		return true;
	return false;
}

template<typename LP, typename FPT>
template<typename T>
bool
Root<LP,FPT>::impl_isInsideCircle( const Point2d_<FPT>&, T, const detail::RootHelper<type::IsLine>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "cannot use isInside(Circle) with a line" );
	return false; // to avoid a warning
}

//------------------------------------------------------------------
/// Intersection of line and circle: implementation for points
template<typename LP, typename FPT>
template<typename T>
detail::Intersect<detail::Inters_2,FPT>
Root<LP,FPT>::impl_intersectsCircle( const Point2d_<FPT>&, T, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "cannot use intersects(Circle) with a point" );
}

/// Line/Circle intersection: implementation
/// For computation details, checkout http://skramm.lautre.net/files/misc/intersect_circle_line.pdf
template<typename LP, typename FPT>
template<typename T>
detail::Intersect<detail::Inters_2,FPT>
Root<LP,FPT>::impl_intersectsCircle(
	const Point2d_<FPT>& pt,       ///< circle origin
	T                    radius,   ///< radius
	const detail::RootHelper<type::IsLine>&  ///< dummy arg, needed so that this overload is only called for lines
) const
{
	detail::Intersect<detail::Inters_2,FPT> out;
	HOMOG2D_CHECK_IS_NUMBER(T);
	const HOMOG2D_INUMTYPE a = static_cast<HOMOG2D_INUMTYPE>(_v[0]); // just to lighten a bit...
	const HOMOG2D_INUMTYPE b = static_cast<HOMOG2D_INUMTYPE>(_v[1]);
	const HOMOG2D_INUMTYPE c = static_cast<HOMOG2D_INUMTYPE>(_v[2]);

// step 1: translate to origin
	auto cp = pt.getX() * a + pt.getY() * b + c;

// step 2: compute distance	between center (origin) and middle point
	auto a2b2 = a * a + b * b;
	auto d0 = std::abs(cp) / std::sqrt( a2b2 );
	if( radius < d0 )                            // if less than radius,
		return out;                         // no intersection

	auto d2 = radius*radius - d0*d0;

// step 3: compute coordinates of middle point B
	auto xb = - a * cp / a2b2;
	auto yb = - b * cp / a2b2;

// step 4: compute coordinates of intersection points, with center at (0,0)
	auto m  = std::sqrt( d2 / a2b2 );
	auto x1 = xb + m*b;
	auto y1 = yb - m*a;

	auto x2 = xb - m*b;
	auto y2 = yb + m*a;

// last step: translate back
	out._ptIntersect_1.set( x1 + pt.getX(), y1 + pt.getY() );
	out._ptIntersect_2.set( x2 + pt.getX(), y2 + pt.getY() );
	out._doesIntersect = true;

	priv::fix_order( out._ptIntersect_1, out._ptIntersect_2 );
	return out;
}
//------------------------------------------------------------------
/// Overload used when attempting to use that on a point
template<typename LP, typename FPT>
template<typename FPT2>
detail::Intersect<detail::Inters_2,FPT>
Root<LP,FPT>::impl_intersectsFRect( const FRect_<FPT2>&, const detail::RootHelper<type::IsPoint>& ) const
{
	static_assert( detail::AlwaysFalse<LP>::value, "Invalid: you cannot call intersects(FRect) on a point" );
}

/// Line/FRect intersection
template<typename LP, typename FPT>
template<typename FPT2>
detail::Intersect<detail::Inters_2,FPT>
Root<LP,FPT>::impl_intersectsFRect( const FRect_<FPT2>& rect, const detail::RootHelper<type::IsLine>& ) const
{
	HOMOG2D_START;

//	std::cout << "Line/FRect intersection, line=" << *this << " rect=" << rect << "\n";
	std::vector<Point2d_<FPT>> pti;
	for( const auto seg: rect.getSegs() )
	{
		auto ppts_seg = seg.getPts();
		auto inters = seg.intersects( *this );
		if( inters() )
		{
			bool storePoint(true);
			auto pt = inters.get();
			if( pt == ppts_seg.first || pt == ppts_seg.second )  // if point is one of the segments
				if( pti.size() == 1 )                            // AND if there is already one
					if( pti[0] == pt )                           // AND that one is already stored
						storePoint = false;
			if( storePoint )
				pti.push_back( pt );

			if( pti.size() == 2 )  // already got 2, done
				break;
		}
	}

#ifndef HOMOG2D_DEBUGMODE
	assert( pti.size() == 0 || pti.size() == 2 ); // only two points
#else
	HOMOG2D_DEBUG_ASSERT(
		( pti.size() == 0 || pti.size() == 2 ),
		"Line/FRect intersection:" << std::scientific << std::setprecision(15) << "\n -line=" << *this << "\n -frect=" << rect
	);
#endif

	if( pti.empty() )
		return detail::Intersect<detail::Inters_2,FPT>();

	priv::fix_order( pti[0], pti[1] );
	return detail::Intersect<detail::Inters_2,FPT>( pti[0], pti[1] );
}


//------------------------------------------------------------------
namespace detail {

/// Implementation of product 3x3 by 3x1
/**
- T1 and T2: type::IsLine or type::IsPoint
*/
template<typename T1,typename T2,typename FPT1,typename FPT2>
void
product(
	Root<T1,FPT1>&       out,
	const Matrix_<FPT2>& h,
	const Root<T2,FPT1>& in
)
{
	for( int i=0; i<3; i++ )
	{
		auto sum  = static_cast<HOMOG2D_INUMTYPE>(h[i][0]) * in._v[0];
		sum      += h[i][1] * in._v[1];
		sum      += h[i][2] * in._v[2];
		out._v[i] = sum;
	}
}

} // namespace detail


/////////////////////////////////////////////////////////////////////////////
// SECTION  - PRODUCT OPERATORS DEFINITIONS
/////////////////////////////////////////////////////////////////////////////

/// Free function template, product of two lines, returns a point
template<typename FPT,typename FPT2>
Point2d_<FPT>
operator * ( const Line2d_<FPT>& lhs, const Line2d_<FPT2>& rhs )
{
#ifndef HOMOG2D_NOCHECKS
	if( lhs.isParallelTo(rhs) )
		HOMOG2D_THROW_ERROR_1( "lines are parallel, unable to compute product" );
#endif

	return detail::crossProduct<type::IsPoint,type::IsLine,FPT>(lhs, rhs);
}

/// Free function template, product of two points, returns a line
template<typename FPT,typename FPT2>
Line2d_<FPT>
operator * ( const Point2d_<FPT>& lhs, const Point2d_<FPT2>& rhs )
{
#ifndef HOMOG2D_NOCHECKS
	if( lhs == rhs )
		HOMOG2D_THROW_ERROR_1( "points are identical, unable to compute product" );
#endif
	Line2d_< FPT> line = detail::crossProduct<type::IsLine,type::IsPoint,FPT>(lhs, rhs);
	line.p_normalizeLine();
	return line;
}

/// Apply Epipolar matrix to a point or line, this will return the opposite type.
/// Free function, templated by point or line
template<typename T,typename U,typename V>
Root<typename detail::HelperPL<T>::OtherType,V>
operator * ( const Hmatrix_<type::IsEpipmat,U>& h, const Root<T,V>& in )
{
	Root<typename detail::HelperPL<T>::OtherType,V> out;
	detail::product( out, h._data, in );
	return out;
}

/// Free function, apply homography to a point.
template<typename T,typename U>
Point2d_<T>
operator * ( const Homogr_<U>& h, const Point2d_<T>& in )
{
	Point2d_<T> out;
	detail::product( out, h._data, in );
	return out;
}

/// Free function, apply homography to a line.
template<typename T,typename U>
Line2d_<T>
operator * ( const Homogr_<U>& h, const Line2d_<T>& in )
{
	if( h._hmt == nullptr )             // if H^-T	not allocated yet, do it
	{
		h._hmt = std::unique_ptr<detail::Matrix_<U>>( new detail::Matrix_<U>() );
		h._hasChanged = true;
	}

	if( h._hasChanged )                  // if homography has changed, recompute inverse and transposed
	{
		auto mat_inv = h.p_inverse();
		*h._hmt = h.p_transpose( mat_inv );
		h._hasChanged = false;
	}

	Line2d_<T> out;
	detail::product( out, *h._hmt, in );
	out.p_normalizeLine();
	return out;
}

/// Apply homography to a Segment
template<typename FPT1,typename FPT2>
Segment_<FPT1>
operator * ( const Homogr_<FPT2>& h, const Segment_<FPT1>& seg )
{
	const auto& pts = seg.getPts();
	Point2d_<FPT1> pt1 = h * pts.first;
	Point2d_<FPT1> pt2 = h * pts.second;
	return Segment_<FPT1>( pt1, pt2 );
}

/// Apply homography to a Polyline
template<typename FPT1,typename FPT2>
Polyline_<FPT1>
operator * ( const Homogr_<FPT2>& h, const Polyline_<FPT1>& pl )
{
	Polyline_<FPT1> out;
	const auto& pts = pl.getPts();
	for( const auto pt: pts )
		out.add( h * pt );
	out.isClosed() = pl.isClosed();
	return out;
}

/// Apply homography to a flat rectangle produces a closed polyline
template<typename FPT1,typename FPT2>
Polyline_<FPT1>
operator * ( const Homogr_<FPT2>& h, const FRect_<FPT1>& rin )
{
	Polyline_<FPT1> out( IsClosed::Yes );
	for( const auto& pt: rin.get4Pts() )
		out.add( h * pt );
	return out;
}

//------------------------------------------------------------------
namespace priv {

/// Allocation for \c std::array container
template<
	typename Cont,
	typename std::enable_if<
		Is_std_array<Cont>::value,
		Cont
	>::type* = nullptr
>
Cont
alloc( std::size_t /* unused here */ )
{
	return Cont();
}

/// Allocation for \c std::vector and \c std::list container
template<
	typename Cont,
	typename std::enable_if<
		!Is_std_array<Cont>::value,
		Cont
	>::type* = nullptr
>
Cont
alloc( std::size_t nb )
{
	return Cont(nb);
}

} // namespace priv

//------------------------------------------------------------------
/// Used to proceed multiple products, whatever the container (\c std::list, \c std::vector, or \c std::array).
/// Returned container is of same type as given input
template<typename FPT,typename Cont>
typename std::enable_if<priv::Is_Container<Cont>::value,Cont>::type
operator * (
	const Hmatrix_<type::IsHomogr,FPT>& h,    ///< Matrix
	const Cont&                         vin   ///< Input container
)
{
	Cont vout = priv::alloc<Cont>( vin.size() );
	auto it = std::begin( vout );
	for( const auto& elem: vin )
		*it++ = h * elem;
	return vout;
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
/// Returns the 2 parallel lines at distance \c dist from \c li
template<typename FPT,typename T>
std::pair<Line2d_<FPT>,Line2d_<FPT>>
getParallelLines( const Line2d_<FPT>& li, T dist )
{
	return li.getParallelLines( dist );
}

/// Returns the distance between 2 parallel lines (free function)
/**
- ref: https://en.wikipedia.org/wiki/Distance_between_two_parallel_lines

We first check that the two lines are indeed parallel.
This should ensure that a1=a2 and b1=b2.
But due to numeric issues they could by slightly different. In order to gain accuracy,
we use the geometric mean of these.
*/
template<typename FPT>
HOMOG2D_INUMTYPE
getParallelDistance( const Line2d_<FPT>& li1, const Line2d_<FPT>& li2 )
{
#ifndef HOMOG2D_NOCHECKS
	if( !li1.isParallelTo(li2) )
		HOMOG2D_THROW_ERROR_1( "lines are not parallel" );
#endif
	const auto ar1 = li1.get();
	const auto ar2 = li2.get();
	const HOMOG2D_INUMTYPE a1 = ar1[0];
	const HOMOG2D_INUMTYPE b1 = ar1[1];
	const HOMOG2D_INUMTYPE c1 = ar1[2];
	const HOMOG2D_INUMTYPE a2 = ar2[0];
	const HOMOG2D_INUMTYPE b2 = ar2[1];
	const HOMOG2D_INUMTYPE c2 = ar2[2];

	HOMOG2D_INUMTYPE a = std::sqrt( a1*a2 );
	HOMOG2D_INUMTYPE b = std::sqrt( b1*b2 );
	return std::abs( c1 - c2 ) / std::sqrt( a*a + b*b );
}


/////////////////////////////////////////////////////////////////////////////
// SECTION  - OPENCV BINDING
/////////////////////////////////////////////////////////////////////////////

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
#ifndef HOMOG2D_NOCHECKS
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
#ifndef HOMOG2D_NOCHECKS
	if( mat.rows != 3 || mat.cols != 3 )
		throw std::runtime_error( "invalid matrix size, rows=" + std::to_string(mat.rows) + " cols=" + std::to_string(mat.cols) );
	if( mat.channels() != 1 )
		throw std::runtime_error( "invalid matrix nb channels: " + std::to_string(mat.channels() ) );
#endif
	auto type = mat.type();
#ifndef HOMOG2D_NOCHECKS
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
drawPt( cv::Mat& mat, PtStyle ps, std::vector<cv::Point2d> vpt, const CvDrawParams& dp, bool drawDiag=false )
{
	auto delta  = dp._dpValues._ptDelta;
	auto delta2 = std::round( 0.85 * delta);
	switch( ps )
	{
		case PtStyle::Times:
			vpt[0].x -= delta2;
			vpt[0].y += delta2;
			vpt[1].x += delta2;
			vpt[1].y -= delta2;

			vpt[2].x += delta2;
			vpt[2].y += delta2;
			vpt[3].x -= delta2;
			vpt[3].y -= delta2;
		break;

		case PtStyle::Plus:
		case PtStyle::Diam:
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
Root<LP,FPT>::impl_draw( cv::Mat& mat, const CvDrawParams& dp, const detail::RootHelper<type::IsPoint>& )  const
{
	if( getX()<0 || getX()>=mat.cols )
		return false;
	if( getY()<0 || getY()>=mat.rows )
		return false;

	std::vector<cv::Point2d> vpt( 4, getCvPtd() );
	switch( dp._dpValues._ptStyle )
	{
		case PtStyle::Plus:   // "+" symbol
			detail::drawPt( mat, PtStyle::Plus,  vpt, dp );
		break;

		case PtStyle::Star:
			detail::drawPt( mat, PtStyle::Plus,  vpt, dp );
			detail::drawPt( mat, PtStyle::Times, vpt, dp );
		break;

		case PtStyle::Diam:
			detail::drawPt( mat, PtStyle::Plus,  vpt, dp, true );
		break;

		case PtStyle::Times:      // "times" symbol
			detail::drawPt( mat, PtStyle::Times, vpt, dp );
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
Root<LP,FPT>::impl_draw( cv::Mat& mat, const CvDrawParams& dp, const detail::RootHelper<type::IsLine>& ) const
{
	assert( mat.rows > 2 );
	assert( mat.cols > 2 );

	Point2d_<FPT> pt1; // 0,0
	Point2d_<FPT> pt2( mat.cols-1, mat.rows-1 );
    auto ri = this->intersects( pt1,  pt2 );
    if( ri() )
    {
    	auto ppts = ri.get();
		cv::Point2d ptcv1 = ppts.first.getCvPtd();
		cv::Point2d ptcv2 = ppts.second.getCvPtd();
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
/// Free function, draws a pair of points
/**
Template type can be std::array<type> or std::vector<type>, with \c type being Point2d or \c Line2d
*/
template<typename T>
void draw( cv::Mat& mat, const std::pair<T,T>& ppts, const CvDrawParams& dp=CvDrawParams() )
{
	ppts.first.draw( mat, dp );
	ppts.second.draw( mat, dp );
}


template<typename FPT>
void
Ellipse_<FPT>::draw( cv::Mat& mat, CvDrawParams dp ) const
{
	cv::ellipse(
		mat,
		_ecenter.getCvPti(),
		cv::Size( getMajor(), getMinor() ),
		getAngle() * 180./M_PI,
		0.,
		360.,
		dp._dpValues._color,
		dp._dpValues._lineThickness,
		dp._dpValues._lineType
	);
}


//------------------------------------------------------------------
#endif // HOMOG2D_USE_OPENCV

/////////////////////////////////////////////////////////////////////////////
// SECTION  - TYPEDEFS
/////////////////////////////////////////////////////////////////////////////

/// Default line type, uses \c double as numerical type
using Line2d = Line2d_<double>;

/// Default point type, uses \c double as numerical type
using Point2d = Root<type::IsPoint,double>;

/// Default homography (3x3 matrix) type, uses \c double as numerical type
using Homogr = Homogr_<double>;

/// Default homogeneous matrix, uses \c double as numerical type
using Epipmat = Hmatrix_<type::IsEpipmat,double>;

/// Default segment type
using Segment = Segment_<double>;

/// Default circle type
using Circle = Circle_<double>;

/// Default rectangle type
using FRect = FRect_<double>;

/// Default polyline type
using Polyline = Polyline_<double>;

// float types
using Line2dF  = Line2d_<float>;
using Point2dF = Root<type::IsPoint,float>;
using HomogrF  = Homogr_<float>;
using SegmentF = Segment_<float>;
using CircleF  = Circle_<float>;
using FRectF   = FRect_<float>;
using PolylineF= Polyline_<float>;

// double types
using Line2dD  = Line2d_<double>;
using Point2dD = Root<type::IsPoint,double>;
using HomogrD  = Homogr_<double>;
using SegmentD = Segment_<double>;
using CircleD  = Circle_<double>;
using FRectD   = FRect_<double>;
using PolylineD= Polyline_<double>;

// long double types
using Line2dL  = Line2d_<long double>;
using Point2dL = Root<type::IsPoint,long double>;
using HomogrL  = Homogr_<long double>;
using SegmentL = Segment_<long double>;
using CircleL  = Circle_<long double>;
using FRectL   = FRect_<long double>;
using PolylineL= Polyline_<long double>;

} // namespace h2d end


#endif // HG_HOMOG2D_HPP

