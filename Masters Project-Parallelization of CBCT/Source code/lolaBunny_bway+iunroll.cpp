
/** RabbitCT - Version 1.0



	RabbitCT enables easy benchmarking of backprojection algorithms.

*/





// include the required header files

#include <iostream>

#include <cmath>

#if !WIN32

#include <string.h> // USES (memset)

#endif

#include "rabbitct.h"

#include <pthread.h>
#include <time.h>
 

using namespace std;

 const int num_thrd=8;

//extern float clk_sum;

//extern RabbitCtGlobalData* s_rcgd = NULL;

 copyStruct threadCopy[num_thrd];

//copyStruct threads[num_thrd];

	

/** \brief Initialization routine.



	This method is required for initializing the data required for

	backprojection. It is called right before the first iteration.

	Here any time intensive preliminary computations and 

	initializations should be performed.

*/

bool RCTLoadAlgorithm(RabbitCtGlobalData* rcgd)

{

	std::cout<<"Loading Algorithm"<<std::endl;

	// calculate the number of voxels

	int N = rcgd->L * rcgd->L * rcgd->L;



	// allocate the required volume

	rcgd->f_L = new float[N];



	// set the volume to zero

	memset(rcgd->f_L, 0, N * sizeof(float));



	return true;

}





/** \brief Finish routine.



	This method is called after the last projection image. Here

	it should be made sure the the rcgd->out_volume pointer

	is set correctly.

*/

bool RCTFinishAlgorithm(RabbitCtGlobalData* rcgd)

{

	// Lola has nothing to do

	std::cout<<"Finishing Algorithm routine"<<std::endl;

	return true;

}



/** \brief Cleanup routine.



	This method can be used to clean up the allocated 

	data required for backprojection. It is called just before

	the benchmark finishes.

*/

bool RCTUnloadAlgorithm(RabbitCtGlobalData* rcgd)

{

	// delete the previously allocated volume
	std::cout<<"Unloading Algorithm"<<std::endl;
	delete [] (rcgd->f_L);
	rcgd->f_L = NULL;
	return true;

}

/** \brief Backprojection iteration.

	

	This function is the C++ implementation of the pseudo-code

	in the technical note.

*/



bool RCTAlgorithmBackprojection(RabbitCtGlobalData* r)

{ 
     pthread_t thread[8];
 	   cpu_set_t cpu[8];	
 	   pthread_attr_t attr[8];

    for(int itr=0; itr< num_thrd; itr++){
    pthread_attr_init(&attr[itr]);
    CPU_ZERO(&cpu[itr]);
    CPU_SET(itr, &cpu[itr]);
    pthread_attr_setaffinity_np(&attr[itr], 16, &cpu[itr]);

    }      

	for(int i =0; i< num_thrd; i++)

	{
		threadCopy[i].L = r->L;
		threadCopy[i].O_L = r->O_L;
		threadCopy[i].R_L = r->R_L;
		threadCopy[i].A_n = r->A_n;
		threadCopy[0].I_n = r->I_n;
		threadCopy[i].A_n_2 = r->A_n_2;
		threadCopy[0].I_n_2 = r->I_n_2;
		threadCopy[i].f_L = r->f_L;
		threadCopy[i].slice= i;
		threadCopy[i].S_x = r->S_x;
		threadCopy[i].S_y = r->S_y;
		int rc=pthread_create(&thread[i], &attr[i], backProject, (void*)&threadCopy[i]);
		if (rc!=0)

		{

			cout<<"Can't create thread\n"<<endl;
            return -1;

        }

    }

    for (int i = 0; i < num_thrd; i++) {

        pthread_join(thread[i], NULL);
        pthread_attr_destroy(&attr[i]);

    }	
	return true;

}

void* backProject (void* parm)

{
clock_t start_clk, end_clk;
float clk_sum=0;

	copyStruct* s = (copyStruct*)parm;   // retrive the slice info
	unsigned int L   = s->L;
	float        O_L = s->O_L;
	float        R_L = s->R_L;
	double*      A_n = s->A_n;
	float*       I_n = s->I_n;
	double*      A_n_2 = s->A_n_2;
	float*       I_n_2 = s->I_n_2;
	float*       f_L = s->f_L;


   	int slice1 = s->slice; 
	int px_x[4], px_y[4];
	double alpha[4], beta[4];
	double pixel_1[4], pixel_2[4], pixel_3[4], pixel_4[4];
	double pix_1, pix_2,pix_3, pix_4;
	int from = (slice1 * L) / num_thrd; // note that this 'slicing' works fine
	int to = ((slice1+1) * L) / num_thrd; // even if SIZE is not divisible by num_thrd

	for (unsigned int k=from; k<to; k++)
	{
		double z = O_L + (double)k * R_L;
		for (unsigned int j=0; j<L; j++)

		{
			double y = O_L + (double)j * R_L;
			for (unsigned int i=0; i<L; i+=2)

			{				
				double x[2] = { O_L + (double)i * R_L, O_L + (double)(i+1) * R_L } ;
				double w_n[4] = { (A_n[2] * x[0] + A_n[5] * y + A_n[8] * z + A_n[11]), (A_n_2[2] * x[0] + A_n_2[5] * y + A_n_2[8] * z + A_n_2[11]),(A_n[2] * x[1] + A_n[5] * y + A_n[8] * z + A_n[11]),(A_n_2[2] * x[1] + A_n_2[5] * y + A_n_2[8] * z + A_n_2[11]) };
				double u_n[4] = { ((A_n[0] * x[0] + A_n[3] * y + A_n[6] * z + A_n[9] ) / w_n[0]), ((A_n_2[0] * x[0] + A_n_2[3] * y + A_n_2[6] * z + A_n_2[9] ) / w_n[1]),  ((A_n[0] * x[1] + A_n[3] * y + A_n[6] * z + A_n[9] ) / w_n[2]), ((A_n_2[0] * x[1] + A_n_2[3] * y + A_n_2[6] * z + A_n_2[9] ) / w_n[3]) };
				double v_n[4] = { ((A_n[1] * x[0] + A_n[4] * y + A_n[7] * z + A_n[10]) / w_n[0]),  ((A_n_2[1] * x[0] + A_n_2[4] * y + A_n_2[7] * z + A_n_2[10]) / w_n[1]), ((A_n[1] * x[1] + A_n[4] * y + A_n[7] * z + A_n[10]) / w_n[2]), ((A_n_2[1] * x[1] + A_n_2[4] * y + A_n_2[7] * z + A_n_2[10]) / w_n[3]) };
				///////////u[0]==u[1]= pixel 1 for image 1 and 2 respt. u[2]==u[3]=pixel 2 for image 1 and 2 respt.
				for(int loop=0; loop<4;loop++)
				{
					px_x[loop] = (int) floor(u_n[loop]);
					px_y[loop] = (int) floor(v_n[loop]);
					alpha[loop] = u_n[loop] - px_x[loop] ;
					beta[loop]  = v_n[loop] - px_y[loop] ;
				}
				///////////////////////////image 1- pixel-1 (y,x) and (y,x+1)/ ///////////////////////////////////
				if(px_y[0]>=0 && px_y[0]<(int)threadCopy[0].S_y)
				{
					if (px_x[0]>=0 && px_x[0]<(int)threadCopy[0].S_x )
						///////////////////(i,j) pixels ///////////////////////////////
						pixel_1[0] = threadCopy[0].I_n[px_y[0] * threadCopy[0].S_x + px_x[0]];
					else
						pixel_1[0] =0.0; 									

					if (px_x[0]+1>=0 && px_x[0]+1<(int)threadCopy[0].S_x)
						/////////////////// (i+1, j) pixels/////////////////////////
						pixel_1[2] = threadCopy[0].I_n[px_y[0] * threadCopy[0].S_x + (px_x[0]+1)];
					else
						pixel_1[2] = 0.0;		
				}
				else{
					pixel_1[0] =0.0; 									
					pixel_1[2] =0.0; 									
				}
				///////////////////////////image 2 pixel-1 (y,x) and (y,x+1)/ ///////////////////////////////////
				if(px_y[1]>=0 && px_y[1]<(int)threadCopy[0].S_y)
				{
					if (px_x[1]>=0 && px_x[1]<(int)threadCopy[0].S_x )
						///////////////////(i,j) pixels ///////////////////////////////
						pixel_2[0] = threadCopy[0].I_n_2[px_y[1] * threadCopy[0].S_x + px_x[1]];
					else
						pixel_2[0] =0.0; 									

					if (px_x[1]+1>=0 && px_x[1]+1<(int)threadCopy[0].S_x)
						/////////////////// (i+1, j) pixels/////////////////////////
						pixel_2[2] = threadCopy[0].I_n_2[px_y[1] * threadCopy[0].S_x + (px_x[1]+1)];
					else
						pixel_2[2] = 0.0;		
				}
				else{
					pixel_2[0] =0.0; 									
					pixel_2[2] =0.0; 									
				}
				///////////////////////////image 1- pixel-1 (y+1,x) and (y+1,x+1)/ ///////////////////////////////////
				if( px_y[0]+1>=0 && px_y[0]+1<(int)threadCopy[0].S_y)
				{

					if (px_x[0]>=0 && px_x[0]<(int)threadCopy[0].S_x)
						pixel_1[1] = threadCopy[0].I_n[(px_y[0]+1) * threadCopy[0].S_x + px_x[0]];
					else
						pixel_1[1] = 0.0;
					

					if (px_x[0]+1>=0 && px_x[0]+1<(int)threadCopy[0].S_x)
						pixel_1[3] = threadCopy[0].I_n[(px_y[0]+1) * threadCopy[0].S_x + (px_x[0]+1)];
					else 
						pixel_1[3] = 0.0;
				}
				else{
					pixel_1[1] = 0.0;
					pixel_1[3] = 0.0;
				}
				///////////////////////////image 2- pixel-1 (y+1,x) and (y+1,x+1)/ ///////////////////////////////////
				if( px_y[1]+1>=0 && px_y[1]+1<(int)threadCopy[0].S_y)
				{

					if (px_x[1]>=0 && px_x[1]<(int)threadCopy[0].S_x)
						pixel_2[1] = threadCopy[0].I_n_2[(px_y[1]+1) * threadCopy[0].S_x + px_x[1]];
					else
						pixel_2[1] = 0.0;
					

					if (px_x[1]+1>=0 && px_x[1]+1<(int)threadCopy[0].S_x)
						pixel_2[3] = threadCopy[0].I_n_2[(px_y[1]+1) * threadCopy[0].S_x + (px_x[1]+1)];
					else 
						pixel_2[3] = 0.0;
				}
				else{
					pixel_2[1] = 0.0;
					pixel_2[3] = 0.0;
				}
//////////////////////////////////////////////////////PIXEL-2//////////////////////////////////////////////////////////////////

					///////////////////////////image 1- pixel-2 (y,x) and (y,x+1)/ ///////////////////////////////////
				if(px_y[2]>=0 && px_y[2]<(int)threadCopy[0].S_y)
				{
					if (px_x[2]>=0 && px_x[2]<(int)threadCopy[0].S_x )
						///////////////////(i,j) pixels ///////////////////////////////
						pixel_3[0] = threadCopy[0].I_n[px_y[2] * threadCopy[0].S_x + px_x[2]];
					else
						pixel_3[0] =0.0; 									

					if (px_x[2]+1>=0 && px_x[2]+1<(int)threadCopy[0].S_x)
						/////////////////// (i+1, j) pixels/////////////////////////
						pixel_3[2] = threadCopy[0].I_n[px_y[2] * threadCopy[0].S_x + (px_x[2]+1)];
					else
						pixel_3[2] = 0.0;		
				}
				else{
					pixel_3[0] =0.0; 									
					pixel_3[2] =0.0; 									
				}
				///////////////////////////image 2 pixel-2 (y,x) and (y,x+1)/ ///////////////////////////////////
				if(px_y[3]>=0 && px_y[3]<(int)threadCopy[0].S_y)
				{
					if (px_x[3]>=0 && px_x[3]<(int)threadCopy[0].S_x )
						///////////////////(i,j) pixels ///////////////////////////////
						pixel_4[0] = threadCopy[0].I_n_2[px_y[3] * threadCopy[0].S_x + px_x[3]];
					else
						pixel_4[0] =0.0; 									

					if (px_x[3]+1>=0 && px_x[3]+1<(int)threadCopy[0].S_x)
						/////////////////// (i+1, j) pixels/////////////////////////
						pixel_4[2] = threadCopy[0].I_n_2[px_y[3] * threadCopy[0].S_x + (px_x[3]+1)];
					else
						pixel_4[2] = 0.0;		
				}
				else{
					pixel_4[0] =0.0; 									
					pixel_4[2] =0.0; 									
				}
				///////////////////////////image 1- pixel-2 (y+1,x) and (y+1,x+1)/ ///////////////////////////////////
				if( px_y[2]+1>=0 && px_y[2]+1<(int)threadCopy[0].S_y)
				{

					if (px_x[2]>=0 && px_x[2]<(int)threadCopy[0].S_x)
						pixel_3[1] = threadCopy[0].I_n[(px_y[2]+1) * threadCopy[0].S_x + px_x[2]];
					else
						pixel_3[1] = 0.0;
					

					if (px_x[2]+1>=0 && px_x[2]+1<(int)threadCopy[0].S_x)
						pixel_3[3] = threadCopy[0].I_n[(px_y[2]+1) * threadCopy[0].S_x + (px_x[2]+1)];
					else 
						pixel_3[3] = 0.0;
				}
				else{
					pixel_3[1] = 0.0;
					pixel_3[3] = 0.0;
				}
				///////////////////////////image 2- pixel-2 (y+1,x) and (y+1,x+1)/ ///////////////////////////////////
				if( px_y[3]+1>=0 && px_y[3]+1<(int)threadCopy[0].S_y)
				{

					if (px_x[3]>=0 && px_x[3]<(int)threadCopy[0].S_x)
						pixel_4[1] = threadCopy[0].I_n_2[(px_y[3]+1) * threadCopy[0].S_x + px_x[3]];
					else
						pixel_4[1] = 0.0;
					

					if (px_x[3]+1>=0 && px_x[3]+1<(int)threadCopy[0].S_x)
						pixel_4[3] = threadCopy[0].I_n_2[(px_y[3]+1) * threadCopy[0].S_x + (px_x[3]+1)];
					else 
						pixel_4[3] = 0.0;
				}
				else{
					pixel_4[1] = 0.0;
					pixel_4[3] = 0.0;
				}




					pix_1 = (1.0 - alpha[0]) * (1.0 - beta[0]) * pixel_1[0] + (1.0 - alpha[0]) * beta[0]  * pixel_1[1]
					+  alpha[0]  * (1.0 - beta[0]) * pixel_1[2]   +  alpha[0]  *  beta[0]  * pixel_1[3];					
					pix_2 = (1.0 - alpha[1]) * (1.0 - beta[1]) * pixel_2[0] + (1.0 - alpha[1]) * beta[1]  * pixel_2[1]
					+  alpha[1]  * (1.0 - beta[1]) * pixel_2[2]   +  alpha[1]  *  beta[1]  * pixel_2[3];					
					pix_3 = (1.0 - alpha[2]) * (1.0 - beta[2]) * pixel_3[0] + (1.0 - alpha[2]) * beta[2]  * pixel_3[1]
					+  alpha[2]  * (1.0 - beta[2]) * pixel_3[2]   +  alpha[2]  *  beta[2]  * pixel_3[3];
					pix_4 = (1.0 - alpha[3]) * (1.0 - beta[3]) * pixel_4[0] + (1.0 - alpha[3]) * beta[3]  * pixel_4[1]
					+  alpha[3]  * (1.0 - beta[3]) * pixel_4[2]   +  alpha[3]  *  beta[3]  * pixel_4[3];

				f_L[k * L * L + j * L + i] += (float)(1.0 / (w_n[0] * w_n[0]) * pix_1);//image 1 pixel 1 
				f_L[k * L * L + j * L + i] += (float)(1.0 / (w_n[1] * w_n[1]) * pix_2);//image 2 pixel 1 
				f_L[k * L * L + j * L + (i+1)] += (float)(1.0 / (w_n[2] * w_n[2]) * pix_3);//image 1 pixel 2 
				f_L[k * L * L + j * L + (i+1)] += (float)(1.0 / (w_n[3] * w_n[3]) * pix_4);//image 2 pixel 2
			}
		}
	}
    return NULL;

}

