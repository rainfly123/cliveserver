#include <pthread.h>

pthread_t clive_task_create(void * (*Run)(void * inData) , void *inParam)
{
    int max_priority = 0;
    pthread_t tid = 0 ;
    int err = -1 ;
    //struct sched_param param;
    pthread_attr_t attr;
    pthread_attr_init(&attr); 
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    //pthread_attr_setschedpolicy(&attr,SCHED_RR);
    // max_priority = sched_get_priority_max(SCHED_RR);
    //param.sched_priority=max_priority;
    //pthread_attr_setschedparam(&attr,&param);
    err = pthread_create(&tid, &attr, Run, inParam);
    if(err != 0 ) 
    {
        perror("Error:thread:");
        return -1 ;
    }
    pthread_attr_destroy(&attr);
    return tid ;
}
