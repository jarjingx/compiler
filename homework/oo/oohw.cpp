#include<stdio.h>

class AAA
{
	public:
	int key;
	int triple( )
	{
		return ( key * 3 );
	}
	double work( int offset )
	{
		return ( key + offset );
	}
 
};

class BBB1 : public AAA
{
};

class BBB2 : public AAA
{
};

class BBB3 : public virtual AAA
{
};

class BBB4 : public virtual AAA
{
};

class CCC1 : public BBB1, public BBB2
{
};

class CCC2 : public BBB3, public BBB4
{
};

int main( )
{
	// C1为非虚继承的产物，具有多个A的实例，C2为虚继承的产物，具有唯一A的实例
	CCC1 multiple;
	CCC2 single;

	// 访问数据成员
	multiple.BBB1::key = 1;
	multiple.BBB2::key = 2;
	single.BBB3::key = 3;
	single.BBB4::key = 4;

	// 输出数据成员的值
	printf( "====================================================\n" );
	printf( "           multiple.BBB1::key == %d\n", multiple.BBB1::key );
	printf( "           multiple.BBB2::key == %d\n", multiple.BBB2::key );
	printf( "             single.BBB3::key == %d\n", single.BBB3::key );
	printf( "             single.BBB4::key == %d\n", single.BBB4::key );

	// 访问方法成员
	printf( "====================================================\n" );
	printf( "        multiple.BBB1::triple == %d\n", multiple.BBB1::triple() );
	printf( "        multiple.BBB2::triple == %d\n", multiple.BBB2::triple() );
	printf( "          single.BBB3::triple == %d\n", single.BBB3::triple() );
	printf( "          single.BBB4::triple == %d\n", single.BBB4::triple() );
	printf( "        multiple.BBB1::triple == %f\n", multiple.BBB1::work(1) );
	printf( "        multiple.BBB2::triple == %f\n", multiple.BBB2::work(1) );
	printf( "          single.BBB3::triple == %f\n", single.BBB3::work(1) );
	printf( "          single.BBB4::triple == %f\n", single.BBB4::work(1) );

	// 打印数据成员的地址
	printf( "====================================================\n" );
	printf( "  add of 'multiple.BBB1::key' == %p\n", &multiple.BBB1::key );
	printf( "  add of 'multiple.BBB2::key' == %p\n", &multiple.BBB2::key );
	printf( "  add of   'single.BBB3::key' == %p\n", &single.BBB3::key );
	printf( "  add of   'single.BBB4::key' == %p\n", &single.BBB4::key );

	// 打印方法成员的地址 —— 失败
	//void ( A::*f( ) );
	//printf( "  add of 'multiple.BBB1::triple' == %p\n", multiple.BBB1::triple );
	//printf( "  add of 'multiple.BBB1::triple' == %p\n", &multiple.BBB1::triple );
	//printf( "  add of 'multiple.BBB1::triple' == %p\n", (void *)(&multiple.CCC1::triple ));
	//printf( "%d\n", &single.B3::triple );
	
	return 0;
}
