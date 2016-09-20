/*#Author - Pranav Goswami#*/
#include <xinu.h> 
pid32 producer_id;
pid32 consumer_id;
int32 consumed_count = 0; 

const int32 CONSUMED_MAX = 100;

int32 buffer[100];


/* Define your circular buffer structure and semaphore variables here */ /* */
process producer(sid32,sid32);
process consumer(sid32,sid32);

/* Place your code for entering a critical section here */ 


void mutex_acquire(sid32 mutex) {
	wait(mutex);

}


/* Place your code for leaving a critical section here */
 void mutex_release(sid32 mutex) {
 	signal(mutex);
}


/* Place the code for the buffer producer here */ 
process producer(sid32 producer_mutex,sid32 consumer_mutex) {

	int32 prod_counter,i;

	for(i=1; i<=2000; i++){ 

		if (prod_counter <= 100)
			prod_counter = prod_counter + 1;
		else
			prod_counter = 0;
	

	mutex_acquire(producer_mutex);
	buffer[prod_counter] = i;
	kprintf("%d \n",buffer[prod_counter]);
	mutex_release(consumer_mutex);
	
	}
	return OK;
}



/* Place the code for the buffer consumer here */
 process consumer(sid32 producer_mutex,sid32 consumer_mutex) {
 	
 	int32 consumer_counter,i;
 	
 	for(i=1; i<=2000; i++){
 		if(consumer_counter <= 100)
 			consumer_counter = consumer_counter + 1;
 		else
 			consumer_counter = 0;

 	mutex_acquire(consumer_mutex);
 	kprintf("%d \n",buffer[consumer_counter]);
 	consumed_count = consumed_count +1;
 	mutex_release(producer_mutex);
 	}

/* Every time your consumer consumes another buffer element,  * make sure to include the statement:  *   consumed_count += 1;  * this will allow the timing function to record performance */ /* */
return OK;
}
/* Timing utility function - please ignore */ 

void time_and_end(void) {
int32 times[5]; int32 i;
for (i = 0; i < 5; ++i) { 
		times[i] = clktime_ms;
	while (consumed_count < CONSUMED_MAX * (i+1));
	times[i] = clktime_ms - times[i];
			consumed_count = 0;
			}
	kill(producer_id);
	kill(consumer_id);

	for (i = 0; i < 5; ++i) {
	kprintf("TIME ELAPSED (%i): %i\n", (i+1) * CONSUMED_MAX, times[i]);
	}
	}

process main(void) { 
	recvclr();
	sid32 producer_mutex,consumer_mutex;

	producer_mutex = semcreate(1); //initialize mutex 
	consumer_mutex = semcreate(0);
	kprintf("pranav");

/* Create the shared circular buffer and semaphores here */ /* */
	producer_id = create(producer, 4096, 50, "producer", 2,producer_mutex,consumer_mutex); 
	consumer_id = create(consumer, 4096, 50, "consumer", 2,producer_mutex ,consumer_mutex); 
	resume(producer_id); 
	resume(consumer_id);
/* Uncomment the following line for part 3 to see timing results */ /* time_and_end(); */
	return OK;
}
