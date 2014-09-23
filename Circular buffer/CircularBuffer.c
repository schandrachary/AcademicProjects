#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "circular_buffer.h"

void store_in_buffer(unsigned char data, struct circular_buffer *my_buf);
char read_from_buffer(struct circular_buffer *my_buf);
//struct CIRCULAR_BUFFER my_buf;
int main()
{
	unsigned char data;
	char readData; 
	int option=0, j;
	
	// Initialize all elements
	struct circular_buffer my_buf;
	my_buf.head = 0;
	my_buf.tail = 0;
	//my_buf.buffer[BUFFER_SIZE] = {'\0'};
	memset(my_buf.buffer, '\0', sizeof(my_buf.buffer));


	printf("This is a circular buffer with one slot open. Enter Buffer_size elements only \n");

	printf("Data in the queue is as shown below: \n");
	printf("-----------------------------\n");
	for(j = 0; j < BUFFER_SIZE; j++){		
		printf("| %c | ", my_buf.buffer[j]);
	}
	printf("\n-----------------------------\n");

	while(1){
		printf("\nChoose 1 to  write characters and 2 to read characters \n");
		scanf("%d", &option);
	
		if( option == 1)
		{
			printf("Enter a character (a-z or A-Z) to be written in the Circular Buffer \n");
			scanf(" %c", &data);
			store_in_buffer(data, &my_buf);

		}

		else if(option == 2)
		{
			readData= read_from_buffer(&my_buf);
			printf("The character | %c | is read from the buffer\n", readData);
			
		}
		
		
		else{
			printf("Wrong option. Enter either 1 or 2\n");
		}

		}

		
	return 0;
}



void store_in_buffer(unsigned char data, struct circular_buffer *my_buf)
{
	int j = 0;
  unsigned int next = (unsigned int)(my_buf->head + 1) % BUFFER_SIZE;
  if (next != my_buf->tail)
  {
      my_buf->buffer[my_buf->head] = data;
      my_buf->head = next;
	  printf("The data in queue is : \n");
	  printf("-----------------------------\n");
		for(j = 0; j < BUFFER_SIZE; j++){
				printf("| %c | ", my_buf->buffer[j]);
			}
	  printf("\n-----------------------------\n");
  }
  else{
	  printf("\nError: The queue is full. Please remove some characters before write operation is performed \n");
  }

}

char read_from_buffer(struct circular_buffer *my_buf)
{
  // if the head isn't ahead of the tail, we don't have any characters
  if (my_buf->head == my_buf->tail) {
	  printf("The queue is empty and therefore there is nothing to read \n ");
  return -1;        // quit with an error
  }
  else {
    char data = my_buf->buffer[my_buf->tail];
    my_buf->tail = (unsigned int)(my_buf->tail + 1) % BUFFER_SIZE;

    return data;
  }
}