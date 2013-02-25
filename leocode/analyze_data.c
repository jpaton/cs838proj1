#include <stdio.h>

int main(int argc, char* argv[]){

	int buffersize=0;
	char* filename = (char*) malloc(20);
	FILE* output;
	if(argc==2)
		output = fopen("onefile.txt","w");
	else
		output = fopen("analysis.txt","w");
	if(output == NULL)
	{
		perror("output file could not be opened\n");
		exit(1);
	}
		
	fprintf(output,"BufferSize\tTime\n");

	while(1){
		if(argc ==2 ){
			filename = argv[1];
		}else{
			buffersize+=2;
			sprintf(filename,"%d.txt\0",buffersize);
		}
		FILE* f = fopen(filename,"r");
		if(f == NULL)
		{
			printf("Could not open file %s to process. exiting\n",filename);
			exit(0);
		}else{
			printf("Processing file %s\n",filename);
		}
		char* line = (char*)malloc(200);
		char* pointer;
		//fgets(line,200,f);
		int iteration=0,i=0;
		double time,sum_with_noise=0,average_with_noise=0,average=0,sum=0;
		while(fgets(line,200,f) != NULL ){
			i++;
			iteration = atoi(line);
			pointer = line;
			while(*pointer != '\t') pointer++;
			pointer++;
			time = atol(pointer);
			if(i!=iteration){
				printf("Missing values !!\n");
			}
			sum_with_noise+=time;
		}
		average_with_noise = sum_with_noise/i;
		rewind(f);
		//fgets(line,200,f);
		int ignored = 0;
		i=0;
		while(fgets(line,200,f) != NULL){
			i++;
			iteration = atoi(line);
                        pointer = line;
                        while(*pointer != '\t') pointer++;
                        pointer++;
                        time = atol(pointer);
			//ignore noise which are atleast 1000 ticks away from average
			if(fabs(average_with_noise-time) < 100000)
			{
				sum += time;
			}else{
				ignored++;
				printf("Ignored %f avg w noise %f\n",time,average_with_noise);
			}
		}
		fclose(f);
		average = sum/i;
		double ignored_percentage = (ignored*100)/i;
		if(ignored_percentage >= 5 ){
			//too many spikes values, investigate
			printf("High Noise percentage - %f \n", ignored_percentage);
			fprintf(output,"%d\t%f\t%f * %f\n",buffersize,average_with_noise,average_with_noise/buffersize,ignored_percentage);
		}else{
			fprintf(output,"%d\t%f\t%f\n",buffersize,average,average/buffersize);
		}
		if(argc==2)break;
	}
	fclose(output);
	exit(0);
}
