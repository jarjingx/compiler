int main( )
{
	int i, sum = 0;
	for( i = 1; i <= 5; i++ )
	{
		sum = sum + i;
		if( sum > 5 ) break;
	}
	return 0;
}
