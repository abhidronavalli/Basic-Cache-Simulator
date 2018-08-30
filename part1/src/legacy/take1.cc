
//#include "world.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//#include <stdbool.h>
#include <limits.h>
//#define BLOCK_SIZE 256
#define FALSE 0
#define TRUE 1

int main(int argc, char *argv[]) 
{
	//hello_world(argc, argv);
    FILE *trace_file; //pointer for the trace file
    

           
    int cursor=0; //for surfing through the data in the trace file
    bool error_flag=FALSE;
    char read_write;
    char cpu_address_hex[9]="";
    
    char cpu_address_binary[33]="";
    char cpu_address_binary_reversed[33]="";
    char cpu_address_binary_temp_nibble[5]="";
    char one_zero[9] = "0";
    char two_zero[9] = "00";
    char three_zero[9] = "000";
    char four_zero[9] = "0000";
    char five_zero[9] = "00000";
    char six_zero[9] = "000000";
    char seven_zero[9] = "0000000";
    
    char tag_bits[33]="";
    char index_bits[33]="";
    int index_bits_decimal=0;
    int index_bits_div_10=0;
    char block_offset_bits[33]="";
    
    int sets=0;
    //char (*direct_mapped_cache_index_array)[33];
    char (*direct_mapped_cache_tag_array)[33]=NULL;
    char (*set_mapped_cache_tag_arrays)[33]=NULL;
    long hex_equ=0;
    
    long read_hit_count=0;
    long read_miss_count=0;
    long write_hit_count=0;
    long write_miss_count=0;
    long write_back=0;
    int read_miss_flag=0;
    int write_miss_flag=0;
    
    int *lru_counter=NULL;
    int *lfu_counter=NULL;
    int *lfu_counter_set_age=NULL;
    int *dm_dirty_bit=NULL;
    int *sm_dirty_bit=NULL;
    
    float miss_rate = 0.00;
    float l1_hit_time=0;
    float l1_miss_penalty=0;
    float l1_total_access_time=0;
    float l1_avg_access_time=0;
    
    long traffic=0;
    
    int total_cache_size=0,cache_block_size=0,cache_blocks_number=0;
    int block_offset=0,block_index=0,block_tag=0;
    char associativity=NULL,rep_policy=NULL,write_policy=NULL;
    int i=0,j=0;
    long line=1;
    int base=1;
    int temp1=0,temp2=0,temp3=0,temp4=0,temp5=0,temp6=0,temp7=0;
    //int temp6=INT_MAX;
    
    //general display of input parameters entered by user
    
	//printf("\n\nHello World!\n\n");
    printf("===== Simulator configuration =====\n");
    printf("L1_BLOCKSIZE:                    %s\n",argv[1]);
    printf("L1_SIZE:                         %s\n",argv[2]);
    printf("L1_ASSOC:                        %s\n",argv[3]);
    printf("L1_REPLACEMENT_POLICY:           %s\n",argv[4]);
    printf("L1_WRITE_POLICY:                 %s\n",argv[5]);
    printf("trace_file:                      %s\n",argv[6]);
    printf("===================================\n");
    printf("\n");
    
    
    
    
    
    //error check routine begins
    
    //checking if the provided block size is power of 2
    if((log2(atoi(argv[1])) != floor(log2(atoi(argv[1]))))) 
        
    {
        printf("Please enter valid block size!\n\n");
        error_flag = TRUE;
        
    }
    
    //checking if the provided cache size is power of 2
    if((log2(atoi(argv[2])) != floor(log2(atoi(argv[2]))))) 
        
    {
        printf("Please enter valid cache size!\n\n");
        error_flag = TRUE;
    }
    
    //checking if valid set associativity
    if(atoi(argv[3])>(atoi(argv[2])/atoi(argv[1])))
    {
        printf("Please enter valid set associativity!\n\n");
        error_flag = TRUE;
    }
    
    //checking if valid replacement policy option
    if(atoi(argv[4])>1)
    {
        printf("Please enter valid replacement option!\n\n");
        error_flag = TRUE;
        printf("Options:\n");
        printf("0:LRU\n");
        printf("1:LFU\n");      
    }
    
    //checking if valid write policy option
    if(atoi(argv[5])>1)
    {
        printf("Please enter valid write option!\n\n");
        error_flag = TRUE;
        printf("Options:\n");
        printf("0:WBWA\n");
        printf("1:WTNA\n");      
    }
    
    //checking for errors in trace file
    
    //fseek(trace_file, -1, SEEK_CUR);
    //fflush(trace_file);
    
    if(argv[6] == NULL)
    {
        printf("Invalid file name (or) File not present in the directory!");
        error_flag = TRUE;
    }
    
    if(error_flag==TRUE)
    {
        printf("Exiting........");
        exit(EXIT_FAILURE);
    }
    
    //error check routine ends
    
    //initialising the L1 cache block
    total_cache_size = atoi(argv[2]);
    cache_block_size = atoi(argv[1]);
    associativity = *argv[3];
    rep_policy = *argv[4];
    write_policy = *argv[5];
    
    
    trace_file = fopen(argv[6],"r");
    //fclose(trace_file_new);

    //remove(trace_file);
    //rename("output.txt", "source.txt");
   //reading trace file begins
   /*
   while ((cursor=fgetc(trace_file))!=EOF)
    {
            putchar(cursor);
    }
    */
    //printf("\nFinished reading the Trace file\n");
    //reading trace file ends
    //fseek(trace_file, 0, SEEK_SET);
    //printf("Now begining to parse the input data\n");
    //parse routine begins
    
    if(associativity=='1')
    {
        
        //printf("Direct Mapped Cache detected!\n");
        cache_blocks_number = total_cache_size/cache_block_size;
        block_offset = floor(log2(cache_block_size));
        block_index = floor(log2(cache_blocks_number));
        block_tag = 32 - (block_offset + block_index);
        //printf("No. of Tag bits = %d\n",block_tag);
        //printf("No. of Index bits = %d\n",block_index);
        //printf("No. of Block Offset bits = %d\n",block_offset);
        //printf("Initialising cache array for data store......\n");
        
        //have to allocate a 2d strings array where one dimensions are known only at run time
        //the other dimension is set to 33
        direct_mapped_cache_tag_array = (char(*)[33])malloc(cache_blocks_number*sizeof(char [33]));
        if(direct_mapped_cache_tag_array == NULL) printf("Cache init failed!!\n");
        for(i=0;i<cache_blocks_number;i++)
        {
            strcpy(direct_mapped_cache_tag_array[i],"");
        }
        dm_dirty_bit = (int *)malloc(cache_blocks_number*sizeof(int));
        if(dm_dirty_bit == NULL) printf("Dirty bits array init failed!\n");
        	for(j=0;j<cache_blocks_number;j++)
        	{
            	dm_dirty_bit[j]=0;
        	}
        //else printf("Initialisation complete!\n");
        //fseek(trace_file, -1, SEEK_CUR);
        //fseek(trace_file, -1, SEEK_SET);
        
    }
    
    else
    {
        //printf("%c-way Set Associative Cache detected!\n",associativity);
        cache_blocks_number = total_cache_size/((atoi(&associativity))*cache_block_size);
        block_offset = floor(log2(cache_block_size));
        block_index = floor(log2(cache_blocks_number));
        block_tag = 32 - (block_offset + block_index);
        //printf("No. of Tag bits = %d\n",block_tag);
        //printf("No. of Index bits = %d\n",block_index);
        //printf("No. of Block Offset bits = %d\n",block_offset);
        //printf("Initialising cache array for tag store, LRU and LFU counter......\n");
        //have to allocate a 2d strings array where both dimensions are known only at run time
        //here the other dimension is set to 33*sizeof 2nd dimension
        //https://stackoverflow.com/questions/14583144/making-2d-array-of-strings-in-c
        //https://stackoverflow.com/questions/917783/how-do-i-work-with-dynamic-multi-dimensional-arrays-in-c
        
        sets = atoi(&associativity);
        set_mapped_cache_tag_arrays = (char(*)[33])malloc(sizeof(char[33])*cache_blocks_number*sets);
        if(set_mapped_cache_tag_arrays == NULL) printf("Cache init failed!\n");
        //else printf("Cache init success!\n");
        //initialising the tag array with zeros
        for(i=0;i<sets;i++)
        {
            for(j=0;j<cache_blocks_number;j++)
            {
                memset(set_mapped_cache_tag_arrays[(i*cache_blocks_number) + j], 0, 33*sizeof(char));
                
            }
        }
        //printing tag array's contents
        /*for(i=0;i<sets;i++)
        {
            for(j=0;j<cache_blocks_number;j++)
            {
                printf("Tag address at set %d of index %d: %s\n",i,j,set_mapped_cache_tag_arrays[(i*cache_blocks_number) + j]); 
            }
            printf("\n\n");
        }*/
        //printf("Initialised the tag arrays with zeros\n");
        //printf("No.of Sets: %d\n",sets);
        //printf("No.of Blocks: %d\n",cache_blocks_number);
        //evict the data with high lru_counter value
        lru_counter = (int*)malloc(sizeof(int)*cache_blocks_number*sets);
        if(lru_counter == NULL) printf("LRU counter init failed!\n");
        //else printf("LRU counter init success!\n");
        //adding random values to lru_counter for initial setup
        temp1 = sets-1;
        for(i=0;i<sets;i++)
        {
            for(j=0;j<cache_blocks_number;j++)
            {
                lru_counter[(i*cache_blocks_number) + j]=temp1;
                
            }
            temp1 = temp1 - 1;
        }
        temp1=0;
        /*printf("Initial values of LRU counters\n");
        for(i=0;i<sets;i++)
        {
            for(j=0;j<cache_blocks_number;j++)
            {
                printf("%d\t",lru_counter[(i*cache_blocks_number) + j]);
                
            }
            printf("\n");
        }*/
        //evict the data with lowest lfu_counter value
        lfu_counter = (int *)malloc(sizeof(int)*cache_blocks_number*sets);
        if(lfu_counter == NULL) printf("LFU counter init failed!\n");
        //else printf("LFU counter init success!\n");
        //initialising the counters to zero
        for(i=0;i<sets;i++)
        {
            for(j=0;j<cache_blocks_number;j++)
            {
                lfu_counter[(i*cache_blocks_number)+j] = 0;
            }
        }
       /* printf("Initial values of LFU counters\n");
        for(i=0;i<sets;i++)
        {
            for(j=0;j<cache_blocks_number;j++)
            {
                printf("%d\t",lfu_counter[(i*cache_blocks_number) + j]);
                
            }
            printf("\n");
        }*/
        lfu_counter_set_age = (int*)malloc(sizeof(int)*cache_blocks_number);
        if(lfu_counter_set_age == NULL) printf("LFU counters age init failed!\n");
        //else printf("LFU counters age init success!\n");
        for(i=0;i<cache_blocks_number;i++)
        {
            lfu_counter_set_age[i] = 0;
        }
        
        sm_dirty_bit = (int *)malloc(sizeof(int)*cache_blocks_number*sets);
        if(sm_dirty_bit == NULL) printf("Dirty bits array init failed!\n");
        //else printf("Dirty bit array init success!\n");
        for(i=0;i<sets;i++)
        {
        for(j=0;j<cache_blocks_number;j++)
        	{
            	sm_dirty_bit[(i*cache_blocks_number) + j]=0;
        	}
        }
        //fseek(trace_file, 0, SEEK_SET);
        //fseek(trace_file, -1, SEEK_SET);
    }
    
    
    
    fseek(trace_file, 0, SEEK_SET);

    while ((cursor=fgetc(trace_file)) != EOF)
        {
            fseek(trace_file, -1, SEEK_CUR);
            fscanf(trace_file, "%c", &read_write);
            //printf("Read/Write %ld = %c\n",line,read_write);
            fscanf(trace_file, "%s\n", &cpu_address_hex);
            //printf("CPU Address in Hex (without zero tagging) %ld = %s\n",line,cpu_address_hex);
            if(strlen(cpu_address_hex)==7)
                { 
                    strcat(one_zero,cpu_address_hex);
                    strcpy(cpu_address_hex,one_zero);
                    strcpy(one_zero,"0");
                }
            else if(strlen(cpu_address_hex)==6)
                { 
                    strcat(two_zero,cpu_address_hex);
                    strcpy(cpu_address_hex,two_zero);
                    strcpy(two_zero,"00");
                }
            else if(strlen(cpu_address_hex)==5)
                { 
                    strcat(three_zero,cpu_address_hex);
                    strcpy(cpu_address_hex,three_zero);
                    strcpy(three_zero,"000");
                }
            else if(strlen(cpu_address_hex)==4)
                { 
                    strcat(four_zero,cpu_address_hex);
                    strcpy(cpu_address_hex,four_zero);
                    strcpy(four_zero,"0000");
                }
            else if(strlen(cpu_address_hex)==3)
                { 
                    strcat(five_zero,cpu_address_hex);
                    strcpy(cpu_address_hex,five_zero);
                    strcpy(five_zero,"00000");
                }
            else if(strlen(cpu_address_hex)==2)
                { 
                    strcat(six_zero,cpu_address_hex);
                    strcpy(cpu_address_hex,six_zero);
                    strcpy(six_zero,"000000");
                }
            else if(strlen(cpu_address_hex)==1)
                { 
                    strcat(seven_zero,cpu_address_hex);
                    strcpy(cpu_address_hex,seven_zero);
                    strcpy(seven_zero,"0000000");
                }
            else if(strlen(cpu_address_hex)==0)
                {
                    printf("No address present at location %ld of the input file",line);
                    printf("Exiting...........");
                    exit(EXIT_FAILURE);
                }
            //printf("CPU Address in Hex (with zero tagging) %ld = %s\n",line,cpu_address_hex);
            for(i=0;i<=strlen(cpu_address_hex)-1;i++)
                {
                    if(cpu_address_hex[i]=='0') strcpy(cpu_address_binary_temp_nibble,"0000");
                    else if(cpu_address_hex[i]=='1') strcpy(cpu_address_binary_temp_nibble,"0001");
                    else if(cpu_address_hex[i]=='2') strcpy(cpu_address_binary_temp_nibble,"0010");
                    else if(cpu_address_hex[i]=='3') strcpy(cpu_address_binary_temp_nibble,"0011");
                    else if(cpu_address_hex[i]=='4') strcpy(cpu_address_binary_temp_nibble,"0100");
                    else if(cpu_address_hex[i]=='5') strcpy(cpu_address_binary_temp_nibble,"0101");
                    else if(cpu_address_hex[i]=='6') strcpy(cpu_address_binary_temp_nibble,"0110");
                    else if(cpu_address_hex[i]=='7') strcpy(cpu_address_binary_temp_nibble,"0111");
                    else if(cpu_address_hex[i]=='8') strcpy(cpu_address_binary_temp_nibble,"1000");
                    else if(cpu_address_hex[i]=='9') strcpy(cpu_address_binary_temp_nibble,"1001");
                    else if(cpu_address_hex[i]=='a') strcpy(cpu_address_binary_temp_nibble,"1010");
                    else if(cpu_address_hex[i]=='b') strcpy(cpu_address_binary_temp_nibble,"1011");
                    else if(cpu_address_hex[i]=='c') strcpy(cpu_address_binary_temp_nibble,"1100");
                    else if(cpu_address_hex[i]=='d') strcpy(cpu_address_binary_temp_nibble,"1101");
                    else if(cpu_address_hex[i]=='e') strcpy(cpu_address_binary_temp_nibble,"1110");
                    else if(cpu_address_hex[i]=='f') strcpy(cpu_address_binary_temp_nibble,"1111");
                    else 
                        {
                            printf("Invalid memory address at line %ld of input file\n",line);
                            printf("Exiting.........\n");
                            exit(EXIT_FAILURE);
                        }
                    //puts(cpu_address_binary_temp_nibble);
                    strcat(cpu_address_binary,cpu_address_binary_temp_nibble);
                }
           /* //printf("CPU Address in Binary(Reversed) %ld = %s\n",line,cpu_address_binary_reversed);
            for(i=strlen(cpu_address_binary_reversed)-1;i>=0;i--)
            cpu_address_binary[strlen(cpu_address_binary_reversed)-1-i] = cpu_address_binary_reversed[i];*/
            //printf("CPU Address in Binary %ld = %s\n",line,cpu_address_binary);
            //printf("String len of CPU address: %lu",strlen(cpu_address_binary));
            //hex to binary coversion finished

            //identify the tag, index and BO bits
            //create array
            //check if read/write and proceed accordingly

            strncpy(tag_bits,cpu_address_binary,block_tag);
            strncpy(index_bits,cpu_address_binary+block_tag,block_index);
            strncpy(block_offset_bits,cpu_address_binary+block_tag+block_index,block_offset);
            //printf("The tag field includes: %s\n",tag_bits);
            //printf("The index field includes: %s\n",index_bits);
            //printf("The block offset field includes: %s\n",block_offset_bits);
            //printf("String length of tag field: %lu\n",strlen(tag_bits));
            //printf("String length of index field: %lu\n",strlen(index_bits));
            //printf("String length of block offset field: %lu\n",strlen(block_offset_bits));
            //converting index to decimal
            while(atoi(index_bits) != 0)
            {
                index_bits_decimal = index_bits_decimal + base*(atoi(index_bits)%10);
                index_bits_div_10 = atoi(index_bits) / 10;
                base=base*2;
                snprintf(index_bits, block_index, "%d", index_bits_div_10);
            }
            base=1;
          // printf("Index in decimal : %d\n",index_bits_decimal);
            
            if(read_write=='r')
            {
                //check type of cache, begin read policy and count no of hits and misses
                //printf("found read operation\n");
                
                //add code for DM cache
                if(associativity=='1')
                {
                    //printf("starting DM cache read\n");
                    //printf("Current address in tag store of index %d: %s \t%d\n",index_bits_decimal,direct_mapped_cache_tag_array[index_bits_decimal],dm_dirty_bit[index_bits_decimal]);
                   //printf("Incoming Tag bits: %s\n",tag_bits);
                    if(strcmp(direct_mapped_cache_tag_array[index_bits_decimal],tag_bits)==0)
                    {
                        //printf("L1 Cache Read Hit!\n");
                        read_hit_count++;
                    }
                    else
                    {
                        //check replacement policy
                        //since DM cache, no need to check 
                        //printf("L1 Cache Read Miss!\n");
                        read_miss_count++;
                        traffic++;
                        strcpy(direct_mapped_cache_tag_array[index_bits_decimal],tag_bits);
                        if(dm_dirty_bit[index_bits_decimal] == 0)
                        {
                                               
                        }
                        else if(dm_dirty_bit[index_bits_decimal] == 1)
                        {
                            
                            write_back = write_back + 1;
                            dm_dirty_bit[index_bits_decimal] = 0;
                            traffic++;
                            //printf("Dirty removed\n");
                            
                        }
                        
                        
                        //request data from next level
                        //valid_bit=1;
                    }
                }
                
                //add code for n-way associativity
                else
                {
                    read_miss_flag=0;
                    temp2=0;
                    temp1=0;
                    //printf("starting set associative cache read\n");
                    //printf("Incoming Tag bits: %s\n",tag_bits);
                    //printf("Current addresses in the tag stores of index %d:\n",index_bits_decimal);
                    //for(j=0;j<sets;j++)
                       // printf("Tag address at set %d: %s\n",j,set_mapped_cache_tag_arrays[(j*(cache_blocks_number)) + index_bits_decimal]);
                    for(j=0;j<sets;j++)
                    {
                        
                        if(strcmp(set_mapped_cache_tag_arrays[(j*cache_blocks_number) + index_bits_decimal],tag_bits)==0)
                        {
                            //printf("L1 Cache Read Hit!\n");
                            read_hit_count = read_hit_count + 1;
                            
                            temp1 = 0;
                            if(rep_policy == '0')
                            {
                                //LRU policy begins
                                temp2 = j;
                                //printf("LRU Read hit at set %d\n",temp2);
                                temp1 = lru_counter[(j*cache_blocks_number) + index_bits_decimal];
                                lru_counter[(j*cache_blocks_number) + index_bits_decimal] = 0;
                                //printf("LRU Counter value at set %d of index %d: %d\n",j,index_bits_decimal,lru_counter[(j*cache_blocks_number) + index_bits_decimal]);
                                read_miss_flag = 3;//doesnt indicate a miss, just used to prevent execution of miss_flag = 2 and miss_flag = 1
                                //check for dirty bits
                                break;
                            }
                            else if(rep_policy == '1')
                            {
				                
                                //LFU policy begins
                                temp2 = j;
                               // printf("LFU Read hit at set %d\n",temp2);
                                temp1 = lfu_counter[(temp2*cache_blocks_number) + index_bits_decimal];
                                lfu_counter[(j*cache_blocks_number) + index_bits_decimal] = temp1 + 1;
                                //printf("LFU Counter value at set %d of index %d now is: %d\n",j,index_bits_decimal,lfu_counter[(j*cache_blocks_number) + index_bits_decimal]);
                                read_miss_flag = 2;//doesnt indicate a miss, just used to prevent execution of miss_flag = 0 and miss_flag = 1
                                break;
                            }
                        }
                        
                        else
                        {
                            
                            read_miss_flag=1;//cache miss
                        }
                    }
                    if (read_miss_flag == 1)
                    {
                        //printf("L1 Cache Read Miss!\n");
                        //check the replacement policy
                        //0:LRU
                        //1:LFU
                        read_miss_count = read_miss_count + 1;
                        traffic++;
                        if(rep_policy == '0')
                            {
                                    
                                    //lru bits=log2(no.of sets)
                                    //check for lru value
                                    //strcpy to the highest value lru flag
                                    //set that lru to zero
                                    //inc all other lru counter values by 1
                                    //request data from lower level caches
                                    temp3 = 0;
                                    temp4 = 0;
                                    //searching for the highest LRU counter value and setting it to zero
                                    for(j=0;j<sets;j++)
                                    {
                                        if(lru_counter[(j*cache_blocks_number) + index_bits_decimal] > temp3)
                                        {
                                            temp3 = lru_counter[(j*cache_blocks_number) + index_bits_decimal];
                                            temp4 = j;
                                        }
                                    }
                                
                                    if(sm_dirty_bit[(temp4*cache_blocks_number) + index_bits_decimal] == 1)
                                    {
                                        sm_dirty_bit[(temp4*cache_blocks_number) + index_bits_decimal] = 0;
                                        write_back = write_back + 1;
                                        traffic++;
                                        //pass data to lower cache
                                    }
                                    

                                    
                                    strcpy(set_mapped_cache_tag_arrays[(temp4*cache_blocks_number) + index_bits_decimal],tag_bits);
                                
                                    if(strcpy(set_mapped_cache_tag_arrays[(temp4*cache_blocks_number) + index_bits_decimal],tag_bits) ==NULL) exit(EXIT_FAILURE);
                                    
                                    sm_dirty_bit[(temp4*cache_blocks_number) + index_bits_decimal] = 0;
                                
                                    lru_counter[(temp4*cache_blocks_number) + index_bits_decimal] = 0;
                                    for(j=0;j<sets;j++)
                                    {
                                        //printf("Updated Tag address at set %d: %s\n",j,set_mapped_cache_tag_arrays[(j*cache_blocks_number) + index_bits_decimal]);
                                    }

                                    //except for the recently modified lru counter, rest all counters are incremented
                                    //printf("LRU Counter value at set %d of index %d: %d\n",temp4,index_bits_decimal,lru_counter[(temp4*cache_blocks_number) + index_bits_decimal]);

                                    for(j=0;j<sets;j++)
                                    {
                                        if(j!=temp4)
                                        lru_counter[(j*cache_blocks_number) + index_bits_decimal]++;  

                                    }
                                
                                //printf("Updated values of LRU counters\n");
                                for(j=0;j<sets;j++)
                                {
                                    //printf("Set %d: %d\n",j,lru_counter[(j*cache_blocks_number) + index_bits_decimal]);
                                }
                            }
                        else if(rep_policy == '1')
                            {

                                    
                                    //find first lowest counter value
                                    //copy that to lfu counter age
                                    //increment that counter by age+1
                                    //update tag bits

                                    //request data from lower level cache

                                    //searching for the lowest lfu counter value
                                    //setting counter to zero and updating tag bits for future access
                                    temp6 = INT_MAX;
                                    //printf("INT_MAX = %d\n",temp6);
                                    temp7 = 0;
                                    //searching for the lowest LFU counter value and setting it to zero
                                    for(j=0;j<sets;j++)
                                    {
                                        if(lfu_counter[(j*cache_blocks_number) + index_bits_decimal] <= temp6)
                                        {
                                            temp6 = lfu_counter[(j*cache_blocks_number) + index_bits_decimal];
                                        }
                                    }

                                    for(j=0;j<sets;j++)
                                    {
                                        if(lfu_counter[(j*cache_blocks_number) + index_bits_decimal] == temp6)
                                        {
                                            temp7 = j;
                                            break;
                                        }
                                    }
                                    //printf("Lowest value of counter found is: %d\n",temp6);
                                    //printf("First lowest value found at set %d\n",temp7);
                                    //lfu_counter_set_age[index_bits_decimal] = lfu_counter[(temp7*cache_blocks_number) + index_bits_decimal];
                                    lfu_counter_set_age[index_bits_decimal] = temp6;
                                    lfu_counter[(temp7*cache_blocks_number) + index_bits_decimal] = lfu_counter_set_age[index_bits_decimal] + 1;
                                    //printf("Age has been set and counter has been incremented\n");

                                    if(sm_dirty_bit[(temp7*cache_blocks_number) + index_bits_decimal] == 1)
                                    {
                                        write_back++;
                                        sm_dirty_bit[(temp7*cache_blocks_number) + index_bits_decimal] = 0;
                                        traffic++;
                                    }
                                    
                                
                                    strcpy(set_mapped_cache_tag_arrays[(temp7*cache_blocks_number) + index_bits_decimal],tag_bits);
                                
                                    if(strcpy(set_mapped_cache_tag_arrays[(temp7*cache_blocks_number) + index_bits_decimal],tag_bits) == NULL) exit(EXIT_FAILURE);

                                    sm_dirty_bit[(temp7*cache_blocks_number) + index_bits_decimal] = 0;
                                
                                    for(j=0;j<sets;j++)
                                    {
                                        //printf("Updated Tag address at set %d: %s\n",j,set_mapped_cache_tag_arrays[(j*cache_blocks_number) + index_bits_decimal]);
                                    }

                                    //printf("LFU Counter value at set %d of index %d: %d\n",temp7,index_bits_decimal,lfu_counter[(temp7*cache_blocks_number) + index_bits_decimal]);
                                
                                //printf("Counter Age value of index %d: %d\n",index_bits_decimal,lfu_counter_set_age[index_bits_decimal]);
                                //printf("Updated values of LFU counters\n");
                                for(j=0;j<sets;j++)
                                {
                                    //printf("Set %d: %d\n",j,lfu_counter[(j*cache_blocks_number) + index_bits_decimal]);
                                }
                            }
                        
                        read_miss_flag=0;
                    }
                    
                    
                    else if(read_miss_flag == 3)
                    {
                        //read hit;now modify the LRU counter value according to the replacement policy
                        for(j=0;j<sets;j++)
                        {
                            if(j!=temp2)
                            {
                                if((lru_counter[(j*cache_blocks_number) + index_bits_decimal]) < temp1) 
                                {
                                    lru_counter[(j*cache_blocks_number) + index_bits_decimal]++;  
                                    //printf("incremented LRU counter of set %d\n",j);
                                }
                            }
                            
                        }
                        //printf("Updated values of counters at index %d\n",index_bits_decimal);
                        for(j=0;j<sets;j++)
                        {
                            //printf("Set %d: %d\n",j,lru_counter[(j*cache_blocks_number) + index_bits_decimal]);
                        }
                           
                    }
                    
                    
                    else if(read_miss_flag == 2)
                    {
                        for(j=0;j<sets;j++)
                        {
                            //printf("Updated Tag address at set %d: %s\n",j,set_mapped_cache_tag_arrays[(j*cache_blocks_number) + index_bits_decimal]);
                        }
                        //printf("No changes made to counter age, it is currently %d\n",lfu_counter_set_age[index_bits_decimal]);
                        //printf("Updated values of LFU counters\n");
                        for(j=0;j<sets;j++)
                        {
                            //printf("Set %d: %d\n",j,lfu_counter[(j*cache_blocks_number) + index_bits_decimal]);
                        }
                    }
                    
                    
                }
                
            }
            

            
            
            
            
            
            else if(read_write=='w')    
            {
                //check type of cache, begin write policy and count no of hits and misses
                //printf("found write operation\n");
                
                
                //add code for DM cache
                if(associativity=='1')
                {
                    //printf("starting DM cache write\n");
                    //printf("Current address in tag store of index %d: %s\t%d\n",index_bits_decimal,direct_mapped_cache_tag_array[index_bits_decimal],dm_dirty_bit[index_bits_decimal]);
                    //printf("Incoming Tag bits: %s\n",tag_bits);
                    if(write_policy == '0')
                    {
                        if(strcmp(direct_mapped_cache_tag_array[index_bits_decimal],tag_bits)==0)
                        {
                            //printf("L1 Cache Write Hit!\n");
                            write_hit_count++;
                            dm_dirty_bit[index_bits_decimal] = 1;
                            //printf("Dirty set\n");
                        }
                        else
                        {
                            //printf("L1 Cache Write Miss!\n");
                            write_miss_count++;
                            traffic++;
                            strcpy(direct_mapped_cache_tag_array[index_bits_decimal],tag_bits);
                            if(dm_dirty_bit[index_bits_decimal] == 0)
                            {
                                dm_dirty_bit[index_bits_decimal] = 1;
                                //printf("Dirty set\n");
                                
                            }
                            else if(dm_dirty_bit[index_bits_decimal] == 1)
                            {
                                write_back = write_back +1;
                                dm_dirty_bit[index_bits_decimal] = 1;
                                traffic++;
                                //printf("Dirty set\n");
                            }
                        }
                    }
                    else if(write_policy == '1')
                    {
                        if(strcmp(direct_mapped_cache_tag_array[index_bits_decimal],tag_bits)==0)
                        {
                            write_hit_count++;
                        }
                        else
                        {
                            write_miss_count++;
                            traffic++;
                        }
                    }
                        
                    

                }
                
                
                
                //set associative cache write code
                else
                {
                    write_miss_flag=0;
                    //printf("starting set associative cache write\n");
                    //printf("Incoming Tag bits: %s\n",tag_bits);
                    //printf("Current addresses in the tag stores of index %d:\n",index_bits_decimal);
                    //for(j=0;j<sets;j++)
                    //    printf("Tag address at set %d: %s\n",j,set_mapped_cache_tag_arrays[(j*(cache_blocks_number)) + index_bits_decimal]);
                    for(j=0;j<sets;j++)
                    {
                        if(strcmp(set_mapped_cache_tag_arrays[(j*cache_blocks_number) + index_bits_decimal],tag_bits)==0)
                        {   
                            //l1 cache write hit
                            //printf("L1 Cache Write Hit!\n");
                            write_hit_count++;
                        
                            //check write policy
                            //0:WBWA
                            //1:WTNA
                            if(write_policy == '0')
                            {
                                sm_dirty_bit[(j*cache_blocks_number) + index_bits_decimal] = 1;
                                //printf("Dirty bit set to 1\n");
                                write_miss_flag = 0;//nothing more needs to be done
                                break;
                            }
                        
                            else if(write_policy == '1')
                            {
                                
                                write_miss_flag = 0;//nothing more needs to be done
                                //pass data to lower cache
                                break;
                                
                            }
                        }
                        else write_miss_flag = 1;    
                    }
                    if (write_miss_flag == 1)
                    {
			            write_miss_flag = 0;
                        //printf("L1 Cache Write Miss!\n");
                        write_miss_count = write_miss_count + 1;
                        traffic++;
                        //check write policy and then rep policy
                        //check write policy
                        //0:WBWA
                        //1:WTNA
                        if(write_policy == '0')
                        {
				
                            //check the rep policy
                            //0:lru
                            //1:lfu
                            //check what rep policy to follow
                            //0:lru
                            //1:lfu
                            if(rep_policy == '0')
                            {
                                //lru bits=log2(no.of sets)
                                //check for lru value
                                //strcpy to the highest value lru flag
                                //set that lru to zero
                                //inc all other lru counter values by 1
                                //request data from lower level caches
                                temp3 = 0;
                                temp4 = 0;
                                
                                //searching for the highest LRU counter value and setting it to zero
                                for(j=0;j<sets;j++)
                                {
                                    if(lru_counter[(j*cache_blocks_number) + index_bits_decimal] > temp3)
                                    {
                                        temp3 = lru_counter[(j*cache_blocks_number) + index_bits_decimal];
                                        temp4 = j;
                                    }
                                }
                                
                                if(sm_dirty_bit[(temp4*cache_blocks_number) + index_bits_decimal] == 1)
                                {
                                	write_back++;
                                    traffic++;
                                    
                                }
                                
                                strcpy(set_mapped_cache_tag_arrays[(temp4*cache_blocks_number) + index_bits_decimal],tag_bits); 
                                sm_dirty_bit[(temp4*cache_blocks_number) + index_bits_decimal] = 1;
                                if(strcpy(set_mapped_cache_tag_arrays[(temp4*cache_blocks_number) + index_bits_decimal],tag_bits) ==NULL) exit(EXIT_FAILURE);
                                
                                
                                for(j=0;j<sets;j++)
                                {
                                    //printf("Updated Tag address at set %d: %s\n",j,set_mapped_cache_tag_arrays[(j*cache_blocks_number) + index_bits_decimal]);
                                }
                                
                                lru_counter[(temp4*cache_blocks_number) + index_bits_decimal] = 0;
                                //except for the recently modified lru counter, rest all counters are incremented
                                //printf("LRU Counter value at set %d of index %d: %d\n",temp4,index_bits_decimal,lru_counter[(temp4*cache_blocks_number) + index_bits_decimal]);
                                for(j=0;j<sets;j++)
                                {
                                    if(j!=temp4)
                                    lru_counter[(j*cache_blocks_number) + index_bits_decimal]++;  
                                    
                                }
                                //printf("Updated values of LRU counters\n");
                                for(j=0;j<sets;j++)
                                {
                                   // printf("Set %d: %d\n",j,lru_counter[(j*cache_blocks_number) + index_bits_decimal]);
                                }
                            }
                            else if(rep_policy == '1')
                            {
                                //find first lowest counter value
                                //copy that to lfu counter age
                                //increment that counter by age+1
                                //update tag bits
                                
                                //request data from lower level cache
                                
                                //searching for the lowest lfu counter value
                                //setting counter to zero and updating tag bits for future access
                                
                                temp6 = INT_MAX;
                                //printf("INT_MAX = %d\n",temp6);
                                temp7 = 0;
                                //searching for the lowest LFU counter value and setting it to zero
                                for(j=0;j<sets;j++)
                                {
                                    if(lfu_counter[(j*cache_blocks_number) + index_bits_decimal] <= temp6)
                                    {
                                        temp6 = lfu_counter[(j*cache_blocks_number) + index_bits_decimal];
                                    }
                                }
                                
                                for(j=0;j<sets;j++)
                                {
                                    if(lfu_counter[(j*cache_blocks_number) + index_bits_decimal] == temp6)
                                    {
                                        temp7 = j;
                                        break;
                                    }
                                }
                                //printf("Lowest value of counter found is: %d\n",temp6);
                                //printf("First lowest value found at set %d\n",temp7);
                                //lfu_counter_set_age[index_bits_decimal] = lfu_counter[(temp7*cache_blocks_number) + index_bits_decimal];
                                lfu_counter_set_age[index_bits_decimal] = temp6;
                                lfu_counter[(temp7*cache_blocks_number) + index_bits_decimal] = lfu_counter_set_age[index_bits_decimal] + 1;
                                //printf("Age has been set and counter has been incremented\n");
                                
                                if(sm_dirty_bit[(temp7*cache_blocks_number) + index_bits_decimal] == 1)
                                {
                                	write_back++;
                                    traffic++;
                                    
                                	
                                }
                                
                                strcpy(set_mapped_cache_tag_arrays[(temp7*cache_blocks_number) + index_bits_decimal],tag_bits);
                                if(strcpy(set_mapped_cache_tag_arrays[(temp7*cache_blocks_number) + index_bits_decimal],tag_bits) == NULL) exit(EXIT_FAILURE);
                                sm_dirty_bit[(temp7*cache_blocks_number) + index_bits_decimal] = 1;
                                
                                for(j=0;j<sets;j++)
                                {
                                    //printf("Updated Tag address at set %d: %s\n",j,set_mapped_cache_tag_arrays[(j*cache_blocks_number) + index_bits_decimal]);
                                }
                                
                                //printf("LFU Counter value at set %d of index %d: %d\n",temp7,index_bits_decimal,lfu_counter[(temp7*cache_blocks_number) + index_bits_decimal]);
                                //printf("Counter Age value of index %d: %d\n",index_bits_decimal,lfu_counter_set_age[index_bits_decimal]);
                                //printf("Updated values of LFU counters\n");
                                for(j=0;j<sets;j++)
                                {
                                    //printf("Set %d: %d\n",j,lfu_counter[(j*cache_blocks_number) + index_bits_decimal]);
                                }
                                                           
                        }
                            
                        }
                        else if(write_policy == '1')
                        {
                            
                        }
                        write_miss_flag = 0;
                    }
                    
                }
            }
            
            
            
            else
            {
                printf("Invalid action found on line %ld. Please add a new line in trace file at this location\n",line);
                printf("Exiting.........\n");
                exit(EXIT_FAILURE);
                
            }
            
            
            index_bits_decimal=0;
            memset(cpu_address_binary_reversed,0,sizeof(cpu_address_binary_reversed));
            memset(cpu_address_binary_temp_nibble,0,sizeof(cpu_address_binary_temp_nibble));
            memset(cpu_address_binary,0,sizeof(cpu_address_binary));
            memset(tag_bits,0,33*sizeof(char));
            memset(index_bits,0,33*sizeof(char));
            memset(block_offset_bits,0,33*sizeof(char));  
            //printf("\n");
            line++;
        }


            //parse routine ends


            //converting di
            //de allocating the index and tag array from Direct mapped cache                                            
            //free(direct_mapped_cache_index_array);

            //printf("fail1");
            if(associativity=='1')
            {

                //converting direct mapped cache tag arrays to hex from binary
                printf("===== L1 contents =====\n");
                for(j=0;j<cache_blocks_number;j++)
                {
                    hex_equ = strtol(direct_mapped_cache_tag_array[j],NULL,2);
                    printf("Set %d: %lx ",j,hex_equ);
                    if (dm_dirty_bit[j]==1) printf("D\n"); else printf("\n");
                }
                free(direct_mapped_cache_tag_array);
                free(dm_dirty_bit);
            }

            else
            {
                printf("===== L1 contents =====\n");
                for(j=0;j<cache_blocks_number;j++)
                {
                    printf("Set %d: ",j);
                    for(i=0;i<sets;i++)
                    {
                        hex_equ = strtol(set_mapped_cache_tag_arrays[(i*cache_blocks_number) + j],NULL,2);
                        printf("%lx ",hex_equ);
                        if (sm_dirty_bit[(i*cache_blocks_number) + j]==1) printf("D "); else printf(" ");
                    }
                    printf("\n");

                }
                free(set_mapped_cache_tag_arrays);
                free(lru_counter);
                free(lfu_counter);
                free(lfu_counter_set_age);
                free(sm_dirty_bit);
            }
        
    //printf("fail3");
    
    //printf("fail4");
    
    //printf("fail5");
    
    //printf("fail6");
    
    miss_rate = (((float)read_miss_count+write_miss_count)/(float)(read_miss_count+read_hit_count+write_hit_count+write_miss_count));

    l1_hit_time = 0.25 + (2.5*(total_cache_size/(512.0000*1024.0000))) + (0.025*(cache_block_size/16)) + (0.025*atoi(&associativity));
    //printf("hit time %f\n",l1_hit_time);
    
    l1_miss_penalty = 20+ (0.5*(cache_block_size/16));
    //printf("miss penalty %f\n",l1_miss_penalty);

    //l1_total_access_time = ((read_miss_count+read_hit_count+write_hit_count+write_miss_count)*l1_hit_time) +((read_miss_count+write_miss_count)*l1_miss_penalty);

    l1_avg_access_time = l1_hit_time + (miss_rate*l1_miss_penalty);

    printf("\n====== Simulation results (raw) ======\n");
    printf("a. number of L1 reads:             %ld\n",read_miss_count+read_hit_count);
    printf("b. number of L1 read misses:       %ld\n",read_miss_count);
    printf("c. Number of L1 writes:            %ld\n",write_hit_count+write_miss_count);
    printf("d. number of L1 write misses:      %ld\n",write_miss_count);
    printf("e. L1 miss rate:                   %0.4f\n",miss_rate);
    printf("f. number of writebacks from L1:   %ld\n",write_back);
    printf("g. total memory traffic:           %ld\n",traffic);       
	printf("\n");
    //L1 miss penalty (in ns) = 20 ns + 0.5*(L1_BLOCKSIZE / 16 B/ns))
    //L1 Cache Hit Time (in ns) = 0.25ns + 2.5ns * (L1_Cache Size / 512kB) + 0.025ns * (L1_BLOCKSIZE / 16B) + 0.025ns * L1_SET_ASSOCIATIVITY
    printf("==== Simulation results (performance) ====\n");
    printf("1. average access time:                  %0.4f ns\n",l1_avg_access_time);       
    //fflush(trace_file);
    //printf("Closing the trace file\n");

    fclose(trace_file);
    return 0;
    
   // else printf("unexpected break\n");
}

