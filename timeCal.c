#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>
#include<sys/time.h>
#include<unistd.h>
#include<signal.h>
#include<math.h>
#include "cs402.h"
#include "my402list.h"

double timeInms()
{
	struct timeval t;
	gettimeofday(&t,NULL);
	long muSec=(t.tv_sec)*1000000+t.tv_usec;
	
	return ((double)muSec/1000.0);
}
