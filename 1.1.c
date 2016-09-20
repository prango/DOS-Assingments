/*  main.c  - main */

#include <xinu.h>

pid32 producer_id;
pid32 consumer_id;
pid32 timer_id;

int32 consumed_count = 0;
const int32 CONSUMED_MAX = 100;

void mutex_acquire(sid32);
void mutex_release(sid32);

process producer(sid32,sid32);
process consumer(sid32,sid32);

int32 global_buffer[20];



/* Place your code for entering a critical section here */
void mutex_acquire(sid32 mutex)
{
	wait(mutex);
}

/* Place your code for leaving a critical section here */
void mutex_release(sid32 mutex)
{
	signal(mutex);
}

/* Place the code for the buffer producer here */
process producer(sid32 producer_mutex,sid32 consumer_mutex)
{	
	int32 producer_index,data ;
	
	while(1)
	{
		if(data <= 1500)
		{
			if(producer_index <= 20)
			{
				producer_index = producer_index + 1;
			}
			else
			{
				producer_index = 0;
			}

			mutex_acquire(producer_mutex);
			global_buffer[producer_index] = data;
			kprintf("Produced : %d \n",global_buffer[producer_index]);
			mutex_release(consumer_mutex);
			data = data + 1;
		}	
	}	
	

	return OK;
}

/* Place the code for the buffer consumer here */
process consumer(sid32 producer_mutex,sid32 consumer_mutex)
{
	/* Every time your consumer consumes another buffer element,
	 * make sure to include the statement:
	 *   consumed_count += 1;
	 * this will allow the timing function to record performance */
	/* */
	int32 consumer_index;
	int32 i;

	while(1)
	{
		
		if(consumer_index <= 20)
		{
			consumer_index = consumer_index + 1;
		}
		else
		{
			consumer_index = 0;
		}
		mutex_acquire(consumer_mutex);
		kprintf("Consumed : %d \n",global_buffer[consumer_index]);
		consumed_count = consumed_count + 1;
		//kprintf("Consumedcount : %d \n",consumed_count);
		mutex_release(producer_mutex);
			
		
	}
	return OK;
}


/* Timing utility function - please ignore */
process time_and_end(void)
{
	int32 times[5];
	int32 i;

	for (i = 0; i < 5; ++i)
	{
		times[i] = clktime_ms;
		yield();

		consumed_count = 0;
		while (consumed_count < CONSUMED_MAX * (i+1))
		{
			yield();
		}

		times[i] = clktime_ms - times[i];
	}

	kill(producer_id);
	kill(consumer_id);

	for (i = 0; i < 5; ++i)
	{
		kprintf("TIME ELAPSED (%d): %d\n", (i+1) * CONSUMED_MAX, times[i]);
	}
}

process	main(void)
{
	recvclr();
	sid32 producer_mutex,consumer_mutex;
	
	producer_mutex = semcreate(1);
	consumer_mutex = semcreate(0);
	
	/* Create the shared circular buffer and semaphores here */
	/* */
	
	producer_id = create(producer, 4096, 50, "producer", 2,producer_mutex,consumer_mutex);
	consumer_id = create(consumer, 4096, 50, "consumer", 2,producer_mutex,consumer_mutex);
	timer_id = create(time_and_end, 4096, 50, "timer", 0);

	resched_cntl(DEFER_START);
	resume(producer_id);
	resume(consumer_id);
	resume(timer_id);
	
	resched_cntl(DEFER_STOP);

	return OK;
}
