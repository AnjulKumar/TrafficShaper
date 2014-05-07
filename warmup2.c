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
double timeInms();
typedef struct nodedata
{
	int number;
	double systemArrival;
	double arrivalTimeQ1;
	double departureTimeQ1;		
	double arrivalTimeQ2;
	double departureTimeQ2;
	double serviceTimeS;
	double serviceTimeE;
	double responseTime;
	double waitingTime;
	int noOfTokens;
}PacketData;

typedef struct token
{
	int number;
	double arrivalTime;
}TokenData;

void transferPacket();
void *arrival();
void *server();
void *token_arrival();
void *handler(), interrupt();

pthread_t user_threadID;
sigset_t new;
struct sigaction act;

//extern int  My402ListLength(My402List*);
//extern int  My402ListAppend(My402List*, void*);
//extern void My402ListUnlink(My402List*, My402ListElem*);
//extern My402ListElem *My402ListFirst(My402List*);

static pthread_cond_t condition_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

My402List* myListq1 = NULL;
My402List* myListq2;
My402List* myListt;

pthread_t threadA,threadS,threadTa;

float lambda;
float mu;
float r;
int B;
int P;
int num;
char * t;
int filearg;
float lambdaFile[500];
float muFile[500];
int PFile[500];
int countFile;
int countNum;

int existT;

int cntPcktQ1;
int cntServed;
int cntToken;
int droppedPackets;
int droppedTokens;
double prevTime;
double sumInterATime;
double sumPacketTimeQ1;
double sumPacketTimeQ2;
double sumServerTime;
double sumSystemTime;
double sqSumSystemTime;
double emTime;
int exitValue;
int killValue;


char buffer[1026];
char buffer1[10];
int argCount;
int arrivalT;
int serviceT;

double start;
int sig;
int killV=0;
FILE *fp;

int main(int argc, char *argv[])
{
	
	lambda=0.5;
	mu=0.35;
	r=1.5;
	B=10;
	P=3;
	num=20;
	countFile=0;
	countNum=0;

	existT=0;

	cntPcktQ1=0;
	cntServed=0;
	cntToken=0;
	droppedPackets=0;
	droppedTokens=0;
	prevTime=0.0;
	sumInterATime=0.0;
	sumPacketTimeQ1=0.0;
	sumPacketTimeQ2=0.0;
	sumServerTime=0.0;
	sumSystemTime=0.0;
	sqSumSystemTime=0.0;
	emTime=0.0;
	exitValue=0;
	killValue=0;
	
	
	argCount=0;
	arrivalT=0;
	serviceT=0;

	
	while(argCount!=argc)
	{
		if(strcmp(argv[argCount],"-t")==0)
		{
			existT=1;
			filearg=argCount+1;
			fp = fopen(argv[filearg],"r");
		}
		argCount++;
	}
	argCount=0;
	while(argCount!=argc)
	{
		if((strcmp(argv[argCount],"-lambda")==0) && (existT!=1))
		{
			lambda=atof(argv[argCount+1]);
		}
		else if((strcmp(argv[argCount],"-mu")==0) && (existT!=1))
		{
			mu=atof(argv[argCount+1]);
		}
		else if((strcmp(argv[argCount],"-r")==0) && (existT!=1))
		{
			r=atof(argv[argCount+1]);
		}
		else if((strcmp(argv[argCount],"-B")==0) && (existT!=1))
		{
			B=atoi(argv[argCount+1]);
		}
		else if((strcmp(argv[argCount],"-P")==0) && (existT!=1))
		{
			P=atoi(argv[argCount+1]);
		}
		else if((strcmp(argv[argCount],"-n")==0) && (existT!=1))
		{
			num=atoi(argv[argCount+1]);
		}
		argCount++;
	}
	if(existT==1)
	{
		printf("Emulation Parameters\n");
		printf("\ttsfile = %s\n",argv[filearg]);
		//if(fgets(buffer1,10,fp)!=NULL)
		//{
			
		//	printf("num:%d\n",num);
		//}
		while((fgets(buffer,1026,fp))!=NULL)
		{
			countNum++;
			if(countNum==1)
			{
				sscanf(buffer,"%d\n",&num);
			}
			else
			{
				
				sscanf(buffer,"%d\t%d\t%d\n",&arrivalT,&PFile[countFile],&serviceT);
				//printf("%d\t%d\t%d\n",arrivalT,PFile[countFile],serviceT);
				lambdaFile[countFile]=(1.0/(float)arrivalT)*1000.0;
				muFile[countFile]=(1.0/(float)serviceT)*1000.0;
				//printf("Count = %d %f\t%d\t%f\n",countFile,lambdaFile[countFile],PFile[countFile],muFile[countFile]);
				countFile++;
			}		
		}
	
	}
	else
	{
		printf("Emulation Parameters\n");
		printf("\tlambda = %f\n",lambda);
		printf("\tmu = %f\n",mu);
		printf("\tr = %f\n",r);
		printf("\tB = %d\n",B);
		printf("\tP = %d\n",P);
		printf("\tnumber to arrive = %d\n",num);
	}
	
	myListq1 =(My402List*)malloc(sizeof(My402List));
	My402ListInit(myListq1);
	myListq2 =(My402List*)malloc(sizeof(My402List));
	My402ListInit(myListq2);
	myListt =(My402List*)malloc(sizeof(My402List));
	My402ListInit(myListt);
	
	sigemptyset(&new);
	sigaddset(&new, SIGINT);
	pthread_sigmask(SIG_BLOCK, &new, NULL);
	pthread_create(&user_threadID, NULL, &handler, argv[1]);				
	
	pthread_create(&threadA,NULL,&arrival,NULL);
	pthread_create(&threadS,NULL,&server,NULL);		
	pthread_create(&threadTa,NULL,&token_arrival,NULL);

	
	pthread_join(threadA,NULL);	
	pthread_join(threadS,NULL);
	pthread_join(threadTa,NULL);
	//pthread_join(user_threadID, NULL);	

	printf("\nStatistics\n");
	printf("\taverage packet inter-arrival time = %.6gs\n",((sumInterATime/(cntPcktQ1-droppedPackets))/1000));
	printf("\taverage packet service time = %.6gs\n\n",((sumServerTime/cntServed)/1000));

	printf("\taverage number of packets in Q1 = %.6g packets\n", sumPacketTimeQ1/emTime);
	printf("\taverage number of packets in Q2 = %.6g packets\n", sumPacketTimeQ2/emTime);
	printf("\taverage number of packets in S = %.6g packets\n\n",sumServerTime/emTime);

	printf("\taverage time a packet spent in system = %.6gs\n",((sumSystemTime/(cntPcktQ1-droppedPackets))/1000));
	
	double var=((sqSumSystemTime/(cntPcktQ1-droppedPackets))-((sumSystemTime/(cntPcktQ1-droppedPackets))*(sumSystemTime/(cntPcktQ1-droppedPackets))))/1000;
	
	printf("\tstandard deviation for time spent in system = %.6gs\n\n",(sqrt(var))/1000);

	printf("\ttoken drop probability = %.6g\n",((((double)droppedTokens)/((double)cntToken))));
	printf("\tpacket drop probability = %.6g\n",(((double)droppedPackets)/((double)cntPcktQ1)));
	
	return 0;
}



void transferPacket()
{
	My402ListElem* elemQ1;
	//printf("Entered Transfer\n");
	if(My402ListLength(myListq1)>0)
	{
		elemQ1=My402ListFirst(myListq1);	
		PacketData* packet = (PacketData*)(elemQ1->obj);
		//printf("PACKET NUMBER TRANSFERED:%d,noofpacketsinq1:%d\n",packet->number, My402ListLength(myListq1));
		//elemQ1->obj=packet;
		int removeTokens=0;

		int listtLen=My402ListLength(myListt);

		int requiredTokens = packet->noOfTokens;
		if(requiredTokens<=listtLen)
		{
			while(removeTokens!=requiredTokens)
			{
				My402ListUnlink(myListt, My402ListFirst(myListt));
				removeTokens++;
			}
			double unlinkQ1;
			unlinkQ1=timeInms();
			packet->departureTimeQ1 = (unlinkQ1-start);
			printf("%012.3fms: p%d leaves Q1, time in Q1=%012.3fms, token bucket now has %d tokens\n",(timeInms()-start), packet->number, ((packet->departureTimeQ1)-(packet->arrivalTimeQ1)), My402ListLength(myListt));			
			sumPacketTimeQ1 = ((packet->departureTimeQ1)-(packet->arrivalTimeQ1))+ sumPacketTimeQ1;		
			//printf("SUMPACKETTIME:%f\n",sumPacketTimeQ1);	
			My402ListUnlink(myListq1, elemQ1);

			double appendQ2;
			appendQ2=timeInms();
			packet->arrivalTimeQ2 = (appendQ2-start);
			printf("%012.3fms: p%d enters Q2\n",(timeInms()-start),packet->number); 			My402ListAppend(myListq2,(void*)packet);			
						
	
			if(My402ListLength(myListq2)==1)
			{
				pthread_cond_broadcast(&condition_cond);
			}
		}
	}
	//printf("Exited Transfer\n");
}
		
void *arrival()
{
	//printf("ENTER arrival\n");
	
		
	start=timeInms();
	printf("\n%012.3fms: emulation begins\n",(start-start));	

	while(((cntPcktQ1-droppedPackets)<num)&&(droppedPackets<num))
	{
		double sleepLess=0.0;
		
		if((1/lambda)>10)
		{
			lambda=0.1;
			if(((1.0/lambda)*1000000.0)>(sleepLess*1000.0))
			{
				usleep((((1.0/lambda)*1000000.0)-(sleepLess*1000.0)));
			}
		}
		else
		{
			if(((1.0/lambda)*1000000.0)>(sleepLess*1000.0))
			{
				usleep((((1.0/lambda)*1000000.0)-(sleepLess*1000.0)));
			}
		}
		pthread_mutex_lock(&mutex1);
		
		double sleepStart = timeInms();
		double systemStart= timeInms();
		
		PacketData *packetQ1=(PacketData*)malloc(sizeof(PacketData));
		packetQ1->number=++cntPcktQ1;
		
		packetQ1->systemArrival= systemStart - start;
		packetQ1->arrivalTimeQ1=0.0;
		packetQ1->departureTimeQ1=0.0;
		packetQ1->serviceTimeS=0.0;
		packetQ1->arrivalTimeQ2=0.0;
		packetQ1->departureTimeQ2=0.0;
		packetQ1->serviceTimeE=0.0;
		packetQ1->responseTime=0.0;
		packetQ1->waitingTime=0.0;
		packetQ1->noOfTokens=P;
		pthread_mutex_unlock(&mutex1);
		if(P>B)
		{	
			double dropT =timeInms();
			printf("%012.3fms: p%d arrives, needs %d tokens, dropped\n", (dropT-start),packetQ1->number, packetQ1->noOfTokens);
			droppedPackets++;
			continue;
		}
		pthread_mutex_lock(&mutex1);
		int pktNum;
		if(existT==1)
		{
			pktNum= packetQ1->number;
			lambda=lambdaFile[pktNum-1];
			packetQ1->noOfTokens=PFile[packetQ1->number-1];
		}	
		//printf("test arrival!!\n");
		double t=timeInms();	
		packetQ1->arrivalTimeQ1 = (t-start);
		sumInterATime = sumInterATime +((packetQ1->arrivalTimeQ1)-prevTime);
		//printf("Arrivaltimeq1 = %f, prevtime = %f\n",packetQ1->arrivalTimeQ1,prevTime);
		printf("%012.3fms: p%d arrives, needs %d tokens, interarrival time = %012.3fms\n", (t-start),packetQ1->number, packetQ1->noOfTokens,((packetQ1->arrivalTimeQ1)-prevTime));	

		prevTime=packetQ1->arrivalTimeQ1;
		//printf("prevTime:%f\n",prevTime);
		double sleepEnd = timeInms();
		sleepLess =  sleepEnd - sleepStart;
		
		int successQ1=My402ListAppend(myListq1,(void*)packetQ1);
		
		double stamp=timeInms();		
		if(successQ1==1)		
			printf("%012.3fms: p%d enters Q1\n",stamp-start,packetQ1->number);
		
		transferPacket();
		
		pthread_mutex_unlock(&mutex1);
		
	}
	if(droppedPackets==num)
	{
		pthread_mutex_lock(&mutex1);
		pthread_cond_broadcast(&condition_cond);
		pthread_mutex_unlock(&mutex1);
	}
	//printf("ARRIVAL THREAD\n");
	pthread_exit(NULL);
	//printf("ARRIVAL THREAD\n");
	return NULL;
}
void *server()
{	
	//elemQ2->obj=packetQ2;
	//printf("SERVER\n");
	for(;;)/*(((cntPcktQ1-droppedPackets)==num)&&(My402ListLength(myListq2)==0))!=1*/
	{	
		
		//printf("ENTER server\n");
		pthread_mutex_lock(&mutex1);
		//printf("Enter server mutex\n");
		if(My402ListLength(myListq2)==0)
		{
			pthread_cond_wait(&condition_cond,&mutex1);		
		}
		
		if(droppedPackets==num)
		{
			exitValue=1;
			printf("SERVER\n");	
				pthread_mutex_unlock(&mutex1);
				pthread_exit(NULL);
		}
		
		pthread_mutex_unlock(&mutex1);
		pthread_mutex_lock(&mutex1);
		if(My402ListLength(myListq2)>0)
		{
			killValue=1;
			double sleepStart = timeInms();
			My402ListElem *elemQ2;	
			elemQ2=My402ListFirst(myListq2);
			PacketData* packetQ2 = (PacketData*)(elemQ2->obj);
			//printf("packet number:%d\n",packetQ2->number);
			if(existT==1)
			{
				//printf("Mu file\n");
				mu=muFile[packetQ2->number-1];
			}	
			
			cntServed++;	
			
			double unlinkQ2=timeInms();
			packetQ2->departureTimeQ2 = (unlinkQ2-start);
			
			sumPacketTimeQ2=sumPacketTimeQ2+((packetQ2->departureTimeQ2)-(packetQ2->arrivalTimeQ2));
			printf("%012.3fms: p%d begins service at S, time in Q2 = %012.3fms\n",(timeInms()-start),packetQ2->number,((packetQ2->departureTimeQ2)-(packetQ2->arrivalTimeQ2)));
				
			double serviceStart=timeInms();
			packetQ2->serviceTimeS=(serviceStart-start);
			
			double sleepEnd = timeInms();
			double sleepLess = sleepStart - sleepEnd;
		
			pthread_mutex_unlock(&mutex1);
			
			if((1/mu)>10)
			{
				mu=0.1;
				if(((1/mu)*1000000)>(sleepLess*1000))
				{	
					usleep(((1/mu)*1000000)-(sleepLess*1000));
				}
			}
			else
			{
				if(((1/mu)*1000000)>(sleepLess*1000))
				{	
					usleep(((1/mu)*1000000)-(sleepLess*1000));
				}
			}
			
			double serviceEnd=timeInms();
			packetQ2->serviceTimeE=(serviceEnd-start);			
			
			sumServerTime= sumServerTime+ ((packetQ2->serviceTimeE)-(packetQ2->serviceTimeS));
			sumSystemTime= sumSystemTime+ ((packetQ2->serviceTimeE)-(packetQ2->arrivalTimeQ1));
			//printf("sumSystemTime: %f",sumSystemTime);
			sqSumSystemTime = sqSumSystemTime + (sumSystemTime * sumSystemTime);
			
			printf("%012.3fms: p%d departs from S, service time = %012.3fms, time in system = %.3fms\n",(timeInms()-start),packetQ2->number,((packetQ2->serviceTimeE)-(packetQ2->serviceTimeS)),((packetQ2->serviceTimeE)-(packetQ2->arrivalTimeQ1)));
	
			emTime=packetQ2->serviceTimeE;
			//printf("emtime:%f",emTime);
			//printf("kill:%d",killV);
			if(killV==1)
				break;
			
			My402ListUnlink(myListq2, elemQ2);
			
			killValue=0;
			pthread_mutex_lock(&mutex1);
			if(((cntPcktQ1-droppedPackets)==num)&&(packetQ2->serviceTimeE!=0.0)&&(My402ListLength(myListq1)==0) && (My402ListLength(myListq2)==0))
			{
				exitValue=1;
				//printf("SERVER\n");	
				pthread_mutex_unlock(&mutex1);
				pthread_exit(NULL);
				//printf("SERVER\n");
			} 
			pthread_mutex_unlock(&mutex1);
			
		}
		
		pthread_mutex_unlock(&mutex1);
	}
	return NULL;
}
void *token_arrival()
{	
	TokenData* token=(TokenData*)malloc(sizeof(TokenData));
	double sleepStart,sleepEnd,sleepLess=0.0;
	
	for(;;)
	{
		
		double t=timeInms();
		double s;
		if((1/r)>10)
		{
			r=0.1;
			if(((1/r)*1000000)>(sleepLess*1000))		
			{		
				s=((1/r)*1000000)-(sleepLess*1000);
				usleep(s);		
			}
		}
		else
		{
			if(((1/r)*1000000)>(sleepLess*1000))		
			{		
				s=((1/r)*1000000)-(sleepLess*1000);
				usleep(s);		
			}
		}
		//printf("test\n");
		
				
		token->number=++cntToken;
		token->arrivalTime=t;
		pthread_mutex_lock(&mutex1);
		//printf("token entered\n");		
		if(My402ListLength(myListt)<B && exitValue==0)
		{
			
			My402ListAppend(myListt,(void*)token);
			
			sleepStart = timeInms();			

			double stamp=timeInms();
			printf("%012.3fms: token t%d arrives, token bucket now has %d tokens\n",(stamp-start),token->number,My402ListLength(myListt));

		}
		else if((My402ListLength(myListt)==B) && ((cntPcktQ1-droppedPackets)==num)&&My402ListLength(myListq1)==0)
		{
			//printf("NO MORE TOKENS\n");
			//pthread_mutex_unlock(&mutex1);
		}
		else if(My402ListLength(myListt)>=B)
		{	
			sleepStart = timeInms();
			double stamp=timeInms();
			printf("%012.3fms: token t%d arrives, dropped\n",(stamp-start),token->number);
			droppedTokens++;
			//pthread_mutex_unlock(&mutex1);
			//continue;
		}
			
		sleepEnd = timeInms();
		sleepLess = sleepStart - sleepEnd;		
		transferPacket();
		pthread_mutex_unlock(&mutex1);
		//printf("OUT OF TOKEN MUTEX\n");
		if(exitValue==1)
		{
			//printf("TOKENS\n");
			pthread_exit(NULL);
			//printf("TOKENS\n");
		}
	}
	//pthread_exit(NULL);
	return NULL;
}

void *handler(char argv1[])
{
	act.sa_handler = interrupt;
	sigaction(SIGINT, &act, NULL);
	pthread_sigmask(SIG_UNBLOCK, &new, NULL);
	//printf("Press CTRL-C to deliver SIGINT\n");
	while(exitValue==0)
	{
		sleep(1);
	}
	//printf("HANDLER\n");
	pthread_exit(NULL);
	//printf("HANDLER\n");
	
	 /* give user time to hit CTRL-C */
		
	return NULL;
}

void interrupt(int sig)
{
	killV=1;
	//printf("thread caught signal %d\n", sig);
	pthread_cancel(threadA);
	pthread_cancel(threadTa);
	pthread_mutex_lock(&mutex1);	
	My402ListUnlinkAll(myListq1);
	exitValue=1;
	//pthread_cond_broadcast(&condition_cond);	
	pthread_mutex_unlock(&mutex1);	
		
	if(killValue==0)
	{
		pthread_cancel(threadS);
		
	}
	My402ListUnlinkAll(myListq2);
} 

