#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#define BUFFER_SIZE 5

typedef struct circular_buffer
{
  unsigned char buffer[BUFFER_SIZE];
  volatile unsigned int head ;
  volatile unsigned int tail ;
};


#endif