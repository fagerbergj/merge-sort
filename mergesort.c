/**
*@author Jason Fagerberg
*@date December 10, 2017
*@goal Sort file filled with strings using a merge sort
*	threads will be used to sort 16 equal segments
*	threads will be used for 8 merges of the 16 segments
*@input file path as given_path
*@output sorted txt as given_path_s.txt
*@assumptions file consists of a new string on each line
		no holes in data
*@dependencies string length =< 65
**/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <pthread.h>

//Global vars
int num_rec = 0;

//Sort thread args
typedef struct sort_param{
    char** base;
    int len;
}sort_param;

//merge thread args
typedef struct merge_param{
     int s1;
     int s2;
     int len;
     char** dat;
}merge_param;

//print # processors
void printProcessors() {
    printf("This system has %d processors\n", get_nprocs_conf());
}

// qsort C-string comparison function
int rec_cmp(const void *a, const void *b) { 
    const char **ia = (const char **)a;
    const char **ib = (const char **)b;
    return strcmp(*ia, *ib);
}

//Read file as pointer to string array
char ** read_file(int size, char* path){
	FILE *f;
	char line[size];
	char **lines = NULL;
	int i = 0;
		f = fopen(path, "r+");

	if(f == NULL) {
		f = fopen(path, "w");
	}

	while(fgets(line, size, f)) {
		line[size-2] = '\0';              
		i++;
		lines = (char**)realloc(lines, sizeof(char*)*i);
		lines[i-1] = strdup(line);
	}
		num_rec = i;
		fclose(f);
return lines;
}

//sort
void* sort(void* x){
    struct sort_param *s = (struct sort_param*) x;
    qsort(s->base,s->len,sizeof(char*),rec_cmp);
}

//merge
void* merge(void* x){
	
struct merge_param *m = (struct merge_param*) x;
	
	int l = m->s1;
	int r = m->s2;
	int k = 0;
	int tot_len = m->len*2;
	char** temp=NULL;
	
	while( l < m->len + m->s1 && r < m->len + m->s2){
		if(strcmp(m->dat[l],m->dat[r]) < 0){
			k++;
			temp = (char**)realloc(temp, sizeof(char*)*k);
			temp[k-1] = strdup(m->dat[l]);
			l++;
		}else{
			k++;
			temp = (char**)realloc(temp, sizeof(char*)*k);
			temp[k-1] = strdup(m->dat[r]);
			r++;
		}
	}
	while(l < m->len + m->s1){
		k++;
		temp = (char **)realloc(temp, sizeof(char*)*k);
		temp[k-1] = strdup(m->dat[l]);
		l++;
	}
	while(r < m->len + m->s2){
		k++;
		temp = (char**)realloc(temp, sizeof(char*)*k);
		temp[k-1] = strdup(m->dat[r]);
		r++;
	}
	
	for(int i = 0; i < tot_len; i++){
		m->dat[m->s1 + i] = temp[i];
	}

//print_arr(temp,tot_len);
}

int main() {
    // print # processors
    printProcessors();
    
    //map file
	char path[100];
	printf("Enter path to file to sort: ");
     	scanf("%s", path);

	char** map = read_file(65, path);

    //thread and segment size
    int numthreads = 8; //# threads
    int jmp = num_rec/(numthreads*2);
	  
    /**
    * sort segments
    **/
	pthread_t s_threads[numthreads*2];
	sort_param s[numthreads*2];   
	
    //make threads
    for(int i = 0; i < numthreads*2; i++){
	    s[i].base = &map[(jmp*i)]; s[i].len = jmp;
	    pthread_create(&s_threads[i], NULL, sort, &s[i]);
    }
    
    //join threads
    for(int i = 0; i < numthreads*2; i++){
	    //printf("i= %d\nc= %d \n", i, c);
	    pthread_join(s_threads[i],NULL);
     }

    /**
    * merge segments
    **/
    int round = 1;

    while(jmp*2 <= num_rec){	
	    pthread_t m_threads[((numthreads)/round)*2];
	    merge_param m[((numthreads)/round)*2];
	    int c = 0;
	
	    //create threads
	    for(int i = 0; i < ((numthreads)/round)*2; i+=2){
		    m[c].s1 = i*jmp; m[c].s2 = ((i+1)*jmp); m[c].len = jmp; m[c].dat = map;
		    pthread_create(&m_threads[c], NULL, merge, &m[c]);
		    c++;
	    }
	    
	    //join threads
	    for(int i = 0; i < c; i++){
		    //printf("i= %d\nc= %d \n", i, c);
		    pthread_join(m_threads[i],NULL);
	    }

    //modify # threads created next round
	jmp*=2;
	//printf("\njmp = %d\n", jmp);
	round*=2;
}

    /**
    * Print to output file
    **/ 
    char* start = path;
    char* end = "_s.txt";

    //append path with "_s.txt"
    char* s_path;
    s_path = malloc(strlen(start)+1+6);
    strcpy(s_path, start);
    strcat(s_path, end);
    
    printf("Output file: %s\n",s_path);

    //create file, write sorted file to it
    FILE *fOut = fopen (s_path, "w");
    for(int i = 0; i < num_rec; i++){
    	fprintf(fOut,"%s\n", map[i]);
    }
    
    fclose(fOut);
    free(s_path);
    //print_arr(map,num_rec);
}

//for debug purposes
void print_arr(char** s, int len){
    for(int i = 0; i < len; i++){
    	printf("#%d, ", i);
    	printf("%s", s[i]);
    }
}

