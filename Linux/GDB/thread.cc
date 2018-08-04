#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *thread1(void *)
{
    for(int i = 0; i < 30; i++)
    {
        printf("thread1, %d  thread id : %ld\n", i, pthread_self());
        sleep(1);
    }
    return NULL;
}

void *thread2(void *)
{
    for(int i = 0; i < 30; i++)
    {
        printf("thread2, %d  thread id : %ld\n", i, pthread_self());
        sleep(1);
    }
    return NULL;
}

int main()
{
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread1, NULL);
    pthread_create(&t2, NULL, thread2, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}