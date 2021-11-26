// http://www.lucadavidian.com/2019/08/26/type-erasure-in-c/?unapproved=1120&moderation-hash=54ca66975ee5ead239548b5a37f019cc#comment-1120

#include <iostream>
#include <vector>
#include <utility>

class Object
{
	private:
    class Concept
    {
		public:
			virtual ~Concept() = default;
			virtual void Print()  = 0;
			virtual Concept *Clone() = 0;
		protected:
			Concept() = default;
    };

    template <typename T>
    class Model : public Concept
    {
		public:
			template <typename U>
			Model(U &&u) : mInstance(std::forward<U>(u)) {}
			void Print()  { mInstance.Print(); }

			template <typename U>
			Concept *Clone()
			{
				return new std::forward<U>(*this);
			}
		private:
			T mInstance;
    };

	public:
		template <typename T>
		Object (T&& t) : mConcept(new Model<T>(std::forward<T>(t))) {}    // forwarding constructor
		~Object() { delete mConcept; }                                    // destructor
		void Print()  { mConcept->Print(); }
	private:
		Concept *mConcept;
};

class C
{
	public:
		void Print()  { std::cout << "hello from C" << std::endl; }
};

class A
{
	public:
		void Print()  { std::cout << "hello from A" << std::endl; }
};

void PrintVec( std::vector<Object> &vec)
{
    for ( Object &object : vec)
        object.Print();
}

int main(int argc, char **argv)
{
    C c;
    A a;

    Object o1(c);
    Object o2(a);

    o1.Print();  // prints "hello from C"
    o2.Print();  // prints "hello from A"

    std::vector<Object> vec;

//    vec.push_back(C());
 //   vec.push_back(A());

    vec.push_back(c);
    vec.push_back(a);

    PrintVec(vec);   // prints "hello from C" and "hello from A"

    return 0;
}

