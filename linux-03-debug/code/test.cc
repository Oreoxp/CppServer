#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <execinfo.h>

using namespace std;

void testcoredump(){
	printf("into func\n");
	//int i=0;
	//int s = 10/i;

	printf("into func2\n");

	char* ss = nullptr;
	*ss = 'd';
	printf("fun,%s\n", ss);
}

void handle_segv(int signum) {
    void *array[100];
    size_t size;
    char **strings;
    size_t i;
    signal(signum, SIG_DFL); // 还原默认的信号处理handler 
    size = backtrace (array, 100);
    strings = (char **)backtrace_symbols (array, size);
	// 这里是打印到日志中，其实也可以打印到某个文件中
    printf("Launcher received SIG: %d Stack trace:\n", signum);
    for (i = 0; i < size; i++)
    {
        printf("%d %s \n",i,strings[i]);
    }

    free(strings);
	//g_launcher->stop();
}


int main(){
    signal(SIGSEGV, handle_segv); // SIGSEGV    11  Core Invalid memory reference
    signal(SIGABRT, handle_segv); // SIGABRT    6       Core Abort signal from
	signal(SIGINT, handle_segv);
	signal(SIGTSTP, handle_segv);
	signal(SIGTERM, handle_segv);


	int i=0;
	printf("1\n");
    printf("2\n");
	i++;
	printf("3, %d\n",i);

	testcoredump();
	printf("end\n");
	return 0;
}
