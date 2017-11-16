#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/select.h>
#include <unistd.h>
#include <SDL2/SDL_bgi.h>
#include <math.h>

#define thinking 1
#define waiting 2
#define eating 3

#define vacant -1

#define pi 3.14159265

struct philosopher{
	int t;
	int id;
	int thinktime;
	int eattime;
	int status;
	int remaining;
};

struct chopstick{
	int id;
	int status;
};

struct philosopher *p;
struct chopstick *ch;
pthread_mutex_t *lock;
pthread_mutex_t lockgui;
int n;
int current=0;
int globaltime=0;

void update()
{
	cleardevice();
	circle(400,300,200);
	double angle = 360/(double)(n);
	double presentangle=0;
	for(int i=0;i<n;i++)
	{
		printf("%lf  ",presentangle);
		double smallinput = presentangle + (angle/2.00);
		double input = presentangle*pi;
		input = input/180.00;
		double si = sin(input);
		double co = cos(input);
		si=si*200.00;
		co=co*200.00;
		line(400,300,400+co,300-si);

		smallinput = smallinput*pi;
		smallinput = smallinput/180.00;
		si = sin(smallinput);
		co = cos(smallinput);
		si = si*150.00;
		co = co*150.00;
		circle(400+co, 300-si, 30);

		si=si/150.00;
		co=co/150.00;
		si=si*250.00;
		co=co*250.00;
		char s[100];
		s[0]='P';
		s[1]=i+1+'0';
		s[2]='-';
		if(p[i].status==thinking)
		{
			s[3]='T';s[4]='h';s[5]='i';s[6]='n';s[7]='k';s[8]='i';s[9]='n';s[10]='g';s[11]='\0';
		}
		else if(p[i].status==eating)
		{
			s[3]='E';s[4]='a';s[5]='t';s[6]='i';s[7]='n';s[8]='g';s[9]='\0';
		}
		else if(p[i].status==waiting)
		{
			s[3]='W';s[4]='a';s[5]='i';s[6]='t';s[7]='i';s[8]='n';s[9]='g';s[10]='\0';
		}

		outtextxy(400+co,300-si,s);
		presentangle+=angle;
	}
	printf("\n");
}

void *performTask(void *id)
{
	int *myid = (int *)(id);
	while(1)
	{
		/*if(p[*myid].status==waiting)
		{
			int a = (*myid)%n;
			int b = ((*myid)+1)%n;
			pthread_mutex_lock(&lock[a]);
			pthread_mutex_lock(&lock[b]);
			if(ch[a].status == vacant && ch[b].status == vacant)
			{
				p[*myid].status = eating;
				p[*myid].remaining = p[*myid].eattime;
				ch[a].status = *myid;
				ch[b].status = *myid;
			}
			pthread_mutex_unlock(&lock[a]);
			pthread_mutex_unlock(&lock[b]);
		}*/
		char *s;
		if(p[*myid].status==thinking)
		{
			s="Thinking";
//			printf("Thinking\n");
		}
		else if(p[*myid].status == waiting)
		{
			s="Waiting";
//			printf("Waiting\n");
		}
		else if(p[*myid].status == eating)
		{
			s="Eating";
//			printf("Eating\n");
		} 
		printf("Philosopher%d time = %d status = %s\n",(*myid+1),p[*myid].t, s);
		if(p[*myid].status==thinking)
		{
			p[*myid].remaining--;
			if(p[*myid].remaining<=0)
			{
				int a = (*myid)%n;
				int b = ((*myid)+1)%n;
				pthread_mutex_lock(&lock[a]);
				pthread_mutex_lock(&lock[b]);
				if(ch[a].status == vacant && ch[b].status == vacant)
				{
					p[*myid].status = eating;
					p[*myid].remaining = p[*myid].eattime;
					ch[a].status = *myid;
					ch[b].status = *myid;
				}
				else
				{
					p[*myid].status = waiting;
					p[*myid].remaining=0;
				}
				pthread_mutex_unlock(&lock[a]);
				pthread_mutex_unlock(&lock[b]);
			//	p[*myid].status=waiting;
			}
			//p[*myid].remaining--;
		}
		else if(p[*myid].status==eating)
		{
			p[*myid].remaining--;
			if(p[*myid].remaining<=0)
			{
				if(p[*myid].thinktime>0)
				{
					p[*myid].status = thinking;
					p[*myid].remaining = p[*myid].thinktime;
				}
				else
				{
					p[*myid].status = waiting;
					p[*myid].remaining = 0;
				}
				int a = (*myid)%n;
				int b = ((*myid)+1)%n;
				pthread_mutex_lock(&lock[a]);
				pthread_mutex_lock(&lock[b]);
				ch[a].status = vacant;
				ch[b].status = vacant;
				pthread_mutex_unlock(&lock[a]);
				pthread_mutex_unlock(&lock[b]);
			}
			//p[*myid].remaining--;
		}
		else if(p[*myid].status==waiting)
		{
			int a = (*myid)%n;
			int b = ((*myid)+1)%n;
			pthread_mutex_lock(&lock[a]);
			pthread_mutex_lock(&lock[b]);
			if(ch[a].status == vacant && ch[b].status == vacant)
			{
				p[*myid].status = eating;
				p[*myid].remaining = p[*myid].eattime;
				ch[a].status = *myid;
				ch[b].status = *myid;
			}
			else
			{
				p[*myid].remaining++;
			}
			pthread_mutex_unlock(&lock[a]);
			pthread_mutex_unlock(&lock[b]);
		}
		pthread_mutex_lock(&lockgui);
		globaltime++;
		if(globaltime%n==0)
		{
			int count=0;
			int x=n/2;
			for(int i=0;i<n;i++)
			{
				if(p[i].status==eating)
					count++;
			}
			if(count<x)
			{
				for(int i=0;i<n;i++)
				{
					int a=i%n;
					int b=(i+1)%n;
					if(p[i].status==waiting && ch[a].status == vacant && ch[b].status == vacant)
					{
						p[i].status = eating;
						p[i].remaining = p[i].eattime;
						ch[a].status = i;
						ch[b].status = i;
					}
				}
			}
			update();
		}
		pthread_mutex_unlock(&lockgui);
		p[*myid].t++;
		sleep(5);
	}
}

int main()
{
	printf("Enter number of philosopher\n");
	scanf("%d",&n);
	//printf("5");
	p=(struct philosopher*)malloc(n*sizeof(struct philosopher));
	ch=(struct chopstick*)malloc(n*sizeof(struct chopstick));
	lock=(pthread_mutex_t*)malloc(n*sizeof(pthread_mutex_t));
	for(int i=0;i<n;i++)
	{
		int think;
		int eat;
		printf("Philosopher%d\n",i+1);
		printf("Enter Thinking Time : ");
		scanf("%d",&think);
		printf("Enter Eat Time : ");
		scanf("%d",&eat);
		p[i].t=0;
		p[i].id=i;
		p[i].thinktime=think;
		p[i].eattime=eat;
		p[i].status=thinking;
		p[i].remaining=p[i].thinktime;

		ch[i].id=i;
		ch[i].status = vacant;

		//printf("3");
		pthread_mutex_init(&lock[i], NULL);
	}
	pthread_mutex_init(&lockgui, NULL);
	int gd = DETECT, gm;
	initgraph (&gd, &gm, "");
	setbkcolor (BLACK);
	cleardevice ();
	circle(400,300,200);
	double angle = 360/(double)(n);
	double presentangle=0;
	for(int i=0;i<n;i++)
	{
		printf("%lf  ",presentangle);
		double smallinput = presentangle + (angle/2.00);
		double input = presentangle*pi;
		input = input/180.00;
		double si = sin(input);
		double co = cos(input);
		si=si*200.00;
		co=co*200.00;
		line(400,300,400+co,300-si);

		smallinput = smallinput*pi;
		smallinput = smallinput/180.00;
		si = sin(smallinput);
		co = cos(smallinput);
		si = si*150;
		co = co*150;
		circle(400+co, 300-si, 30);

		si=si/150;
		co=co/150;
		si=si*250;
		co=co*250;
		char s[100];
		s[0]='P';
		s[1]=i+1+'0';
		s[2]='-';
		if(p[i].status==thinking)
		{
			s[3]='t';s[4]='h';s[5]='i';s[6]='n';s[7]='k';s[8]='i';s[9]='n';s[10]='g';s[11]='\0';
		}
		else if(p[i].status==eating)
		{
			s[3]='E';s[4]='a';s[5]='t';s[6]='i';s[7]='n';s[8]='g';s[9]='\0';
		}
		else if(p[i].status==waiting)
		{
			s[3]='W';s[4]='a';s[5]='i';s[6]='t';s[7]='i';s[8]='n';s[9]='g';s[10]='\0';
		}

		outtextxy(400+co,300-si,s);
		presentangle+=angle;
	}
	printf("\n");

	//printf("%d%d",getmaxx(),getmaxy());
	//printf("1");
	pthread_t thread[n];
	int phil_num[n];
	for(int i=0;i<n;i++)
	{
		phil_num[i]=i;
	}
	for(int i=0;i<n;i++)
	{
		//printf("2");
		pthread_create(&thread[i],NULL,performTask,&phil_num[i]);
	}
	for(int i=0;i<n;i++)
        pthread_join(thread[i],NULL);
    closegraph();
}
