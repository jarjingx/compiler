#include<stdio.h>

class A
{
	public:
	int key;
	int triple( )
	{
		return key * 3;
	}
 
};

class B1 : public A
{
};

class B2 : public A
{
};

class B3 : public virtual A
{
};

class B4 : public virtual A
{
};

class C1 : public B1, public B2
{
};

class C2 : public B3, public B4
{
};

int main( )
{
	// C1为非虚继承的产物，具有多个A的实例，C2为虚继承的产物，具有唯一A的实例
	C1 multiple;
	C2 single;

	// 访问数据成员
	multiple.B1::key = 1;
	multiple.B2::key = 2;
	single.B3::key = 3;
	single.B4::key = 4;

	// 输出数据成员的值
	printf( "====================================================\n" );
	printf( "           multiple.B1::key == %d\n", multiple.B1::key );
	printf( "           multiple.B2::key == %d\n", multiple.B2::key );
	printf( "             single.B3::key == %d\n", single.B3::key );
	printf( "             single.B4::key == %d\n", single.B4::key );

	// 访问方法成员
	printf( "====================================================\n" );
	printf( "        multiple.B1::triple == %d\n", multiple.B1::triple() );
	printf( "        multiple.B2::triple == %d\n", multiple.B2::triple() );
	printf( "          single.B3::triple == %d\n", single.B3::triple() );
	printf( "          single.B4::triple == %d\n", single.B4::triple() );

	// 打印数据成员的地址
	printf( "====================================================\n" );
	printf( "  add of 'multiple.B1::key' == %p\n", &multiple.B1::key );
	printf( "  add of 'multiple.B2::key' == %p\n", &multiple.B2::key );
	printf( "  add of   'single.B3::key' == %p\n", &single.B3::key );
	printf( "  add of   'single.B4::key' == %p\n", &single.B4::key );

	// 打印方法成员的地址 —— 失败
	//void ( A::*f( ) );
	//printf( "  add of 'multiple.B1::triple' == %p\n", multiple.B1::triple );
	//printf( "  add of 'multiple.B1::triple' == %p\n", &multiple.B1::triple );
	//printf( "  add of 'multiple.B1::triple' == %p\n", (void *)(&multiple.C1::triple ));
	//printf( "%d\n", &single.B3::triple );
	
	return 0;
}
