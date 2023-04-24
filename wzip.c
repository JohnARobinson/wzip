#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
John Robinson
4/29/22

Sorry code not commented been to busy to clean up further
*/
struct _args
{
	char *charnooffset;
	char *charoffset1;
	char *charoffset2;
	int offset1;
	int offset2;
	int offset3;
	char filename[32];
	int size;


};
struct _args args;



void fwriteprint(int num, char letter)
{

	fwrite(&num, sizeof(int),1,stdout);
        fwrite(&letter, sizeof(char),1,stdout);

}


void *workerthread1(void *arg)
{
	
	int fp2 = open(args.filename,O_RDONLY);

	args.charnooffset = mmap(0,args.size, PROT_READ, MAP_PRIVATE, fp2, args.offset1);

	close(fp2);

	return NULL;
}

void *workerthread2(void *arg)
{
	
	int fp3 = open(args.filename,O_RDONLY);
	
	args.charoffset1 = mmap(0,args.size, PROT_READ, MAP_PRIVATE, fp3, args.offset2);
	
	close(fp3);

        return NULL;
}

void *workerthread3(void *arg)
{
	int fp4 = open(args.filename,O_RDONLY);

	args.charoffset2 = mmap(0,args.size, PROT_READ, MAP_PRIVATE, fp4, args.offset3);

	close(fp4);

        return NULL;
}



int main(int argc, char *argv[]) {
	

	if(argc == 1)
	{
		printf("wzip: file1 [file2 ...]\n");
		return 1;
	}

  

        FILE *fp;
	
	
        char previous_letter;
        char current_letter;
	
        int current_letter_count = 1;
	
	int prevsize = 0;
	
	char *letterarray;
	
        int letterarraycount = 0;
	int c;
        int filelength[7];
	int totalcounter=0;
	int totalfilesize = 0;
	//int largefile = 0;

        for(int m = 0; m < argc-1; m++)

        {	strcpy(args.filename, argv[m+1]);
		fp = fopen(argv[m+1],"r");
		fseek(fp, 0, SEEK_END);
                args.size = ftell(fp);
                fseek(fp, 0, SEEK_SET);

		totalfilesize = args.size + totalfilesize;

		int offsetcalc = args.size / 3;
		args.offset1 = 0;
		int offset2 = args.offset1 + offsetcalc;
		int power = 1;
                while(power < offset2)
                        power*=2;
		args.offset2 = power;
		int offset3 = offset2 + offsetcalc;	
		int power2 = 1;
		while(power2 < offset3)
    			power2*=2;
		args.offset3 = power2;

		if(args.size >= 4096)
		{	
			pthread_t w1, w2, w3;

			pthread_create(&w1, NULL, workerthread1, NULL);

			pthread_create(&w2, NULL, workerthread2, NULL);
			
			pthread_create(&w3, NULL, workerthread3, NULL);

			pthread_join(w1, NULL);
			pthread_join(w2, NULL);
			pthread_join(w3, NULL);
			
			
						
		

			if(m == 0)
                        {
                                letterarray = (char*)malloc(args.size + 1);


				int arg1count = 0;
	                        int arg2count = 0;
        	                while(totalcounter < args.size)
                	        {

                        	        if(totalcounter < args.offset2)
                                	{
                                        	letterarray[totalcounter] = args.charnooffset[totalcounter];
                                	}

                                	if(totalcounter >= args.offset2 && totalcounter < args.offset3)
                                	{
                                        	letterarray[totalcounter] = args.charoffset1[arg1count];
                                        	arg1count++;
                                	}
                                	if(totalcounter >= args.offset3)
                                	{
                                        	letterarray[totalcounter] = args.charoffset2[arg2count];
                                        	arg2count++;
                                	}

                                	totalcounter++;
                        	}
                        	letterarraycount = args.size;



                        }
                        if(m > 0)
                        {
				letterarray = (char*)realloc(letterarray, (totalfilesize)* sizeof(char*));


				int newtotal = 0;
				int arg1count = 0;
                        	int arg2count = 0;
				letterarraycount = args.size + letterarraycount;
                        	while(totalcounter < letterarraycount)
                        	{


                                	if(newtotal < args.offset2)
                                	{
                                	        letterarray[totalcounter] = args.charnooffset[newtotal];
                                	}

                                	if(newtotal >= args.offset2 && newtotal < args.offset3)
                                	{
                                        	letterarray[totalcounter] = args.charoffset1[arg1count];
                                        	arg1count++;
                                	}
                                	if(newtotal >= args.offset3)
                                	{
                                        	letterarray[totalcounter] = args.charoffset2[arg2count];
                                        	arg2count++;
                                	}
					newtotal++;
                                	totalcounter++;
                        	}

                        }
			prevsize = args.size;
			
			
			
		}
		else
		{

			if(m == 0)
			{
				letterarray = (char*)malloc(args.size + 1);
			}
             		if(m > 0)
			{
				letterarray = (char*)realloc(letterarray, (args.size + prevsize)* sizeof(char*));	
			}
			prevsize = args.size;
			
			while(1)
                	{
				if (feof(fp))
				{
					break;
				}
				c = fgetc(fp);	
				if(c != EOF)
				{	
					letterarray[letterarraycount] = c;
                        		letterarraycount++;
				
					filelength[m]++;
				}	
			
                	}
		}

                fclose(fp);
        }

	
        int goodchar = 0;
        for(int v = 0; v <= letterarraycount; v++)
        {
                
                if(letterarray[v] != EOF && letterarray[v] != '\n')
                {
                        current_letter = letterarray[v];
                        goodchar = 1;
                }
		if(letterarray[v] == '\n')
		{
			current_letter = letterarray[v];
			goodchar = 1;

		}

		if(v > 0 && goodchar == 1)
                {	
                        if(current_letter == previous_letter)
                        {
                                current_letter_count++;
                        }
                        if(current_letter != previous_letter || v == letterarraycount)
                        {
				
				fwriteprint(current_letter_count, previous_letter);
                                current_letter_count = 1;
                        }
			

                }
                if(goodchar == 1)
                {	
			previous_letter = current_letter;
		}
		goodchar = 0;
        }
	free(letterarray);	
	
       
}	
