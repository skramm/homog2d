/// attempt to reproduce build failure in branch dev6c
/**

*/
#include <vector>
#include <iostream>

using namespace std;

template<typename T,typename U> struct A;
template<typename T,typename U> struct B;


template<typename T>
struct Result
{
};

template<typename T,typename U>
struct CommonPrim
{
//	virtual Result<T> foo( const CommonPrim<T>& ) const = 0;

	virtual Result<T> foo( const U& ) const = 0;
};

/*
template<typename T>
struct CPTT
{
//	using RealType = T;
};

template<typename T>
struct CPTT<A<T>>
{
	using RealType = A<T>;
};
template<typename T>
struct CPTT<B<T>>
{
	using RealType = B<T>;
};
*/

template<typename T,typename U>
struct A: public CommonPrim<T,U>
{
	Result<T> foo( const U& ) const
	{
		cout << "foo A\n";
	}
/*	Result<T> foo( const CommonPrim<T>& cp ) const
	{
		cout << "foo A\n";
		auto& arg = dynamic_cast<CPTT<T>&>(cp);
		return this->foo(arg);
	}*/

	Result<T> foo( const A<T,U>& cp ) const
	{ cout << "foo A::A\n"; return Result<T>(); }

	Result<T> foo( const B<T,U>& cp ) const
	{ cout << "foo A::B\n"; return Result<T>(); }
};

template<typename T,typename U>
struct B: public CommonPrim<T,U>
{

	Result<T> foo( const U& ) const
	{
		cout << "foo B\n";
//		auto& arg = dynamic_cast<CPTT<T>&>(cp);
//		return this->foo(arg);
	}

	Result<T> foo( const A<T,U>& cp ) const
	{ cout << "foo B::A\n"; return cp.foo(*this); }

	Result<T> foo( const B<T,U>& cp ) const
	{ cout << "foo B::B\n"; return Result<T>(); }

};

int main()
{
	A<float> a0;
	std::vector<CommonPrim<float>*> vec;
	A<float> a1,a2;
	B<float> b1,b2;

	int i=0;
	cout << i++ << "\n";
	a1.foo(b1);
	cout << i++ << "\n";
	a1.foo(a2);
	cout << i++ << "\n";
	b1.foo(b2);
	cout << i++ << "\n";
	b1.foo(a1);

	vec.push_back( &a1 );
	vec.push_back( &b2 );

	for(auto elem: vec )
		auto res = elem->foo( a0 );

//	vec.push_back( &B<float>() );
}
