#include <stdio.h>


using namespace std;

void testcoredump(){
	int i=0;
	int s = 10/i;

	char* ss = nullptr;
	*ss = 'd';
	printf("fun,%s\n", ss);
}

int main(){
	int i=0;
	printf("1\n");
    printf("2\n");
	i++;
	printf("3, %d\n",i);

	testcoredump();
	printf("end\n");
	return 0;
}
