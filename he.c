#include <unistd.h>

int main(void)
{
	while(1)
	{
		write(STDERR_FILENO,"Hello World!\n",13);
		sleep(1);
	}
}
