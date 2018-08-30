/*Cache simulator for ECE563*/
//Part A: Includes only L1 Cache hierarchy with LRU, LFU read policies and WBWA and WTWNA policies
//Part B: Includes L2 Cache along with victim Cache


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
    char index_bits_copied[33]="";
    int index_bits_decimal=0;
    int index_bits_div_10=0;
    char block_offset_bits[33]="";
    char victim_tag_bits[33]="";
    char l1_victim_tag_bits[33]="";

    int sets=0;
    char (*set_mapped_cache_tag_arrays)[33]=NULL;
    char (*set_mapped_victim_cache_tag_arrays)[33]=NULL;
    int *set_mapped_victim_cache_valid_arrays=NULL;
    int *set_mapped_cache_valid_arrays=NULL;
    long hex_equ=0;

    long read_hit_count=0;
    long read_miss_count=0;
    long write_hit_count=0;
    long write_miss_count=0;
    long write_back=0;
    long victim_cache_write_back=0;
    //int read_miss_flag=0;
    //int write_miss_flag=0;
    int hit_flag=0;
    int l1_cache_hit_flag=0, victim_cache_hit_flag=0;
    int l1_victim_swaps=0;

    int *lru_counter=NULL;
    int *lfu_counter=NULL;
    int *lfu_counter_set_age=NULL;
    int *set_mapped_dirty_bit=NULL;
    int *set_mapped_victim_dirty_bit=NULL;
    int *victim_cache_lru_counter=NULL;

    float miss_rate = 0.00;
    float l1_hit_time=0;
    float l1_miss_penalty=0;
    //float l1_total_access_time=0;
    //float l1_avg_access_time=0;

    long traffic=0;

    long total_cache_size=0;
    int cache_block_size=0,cache_blocks_number=0;
    int is_victim_cache_enabled=0,victim_cache_blocks_number=0,victim_cache_sets=0;
    int block_offset=0,block_index=0,block_tag=0;
    char rep_policy=NULL,write_policy=NULL;
    int associativity=0;
    int i=0,j=0,k=0;
    long line=1;
    int base=1;
    int temp1=0,temp2=0,temp3=0,temp4=0,temp5=0,temp6=0,temp7=0,temp8=0,temp9=0,temp10=0,temp11=0,temp12=0;
    int temp13=0,temp14=0,temp15=0,temp16=0,temp17=0,temp18=0,temp19=0,temp20=0,temp21=0;
    int victim_swap=0;

    long l2_cache_size=0;
    int associativity_l2=0;
    int l2_cache_blocks_number=0,l2_block_index=0,l2_block_tag=0,l2_sets=0,l2_index_bits_decimal=0,l2_index_bits_div_10=0,l2_temp1=0;

    char (*l2_set_mapped_cache_tag_arrays)[33]=NULL;
    int *l2_set_mapped_cache_valid_arrays=NULL;
    int *l2_lru_counter=NULL;
    int *l2_lfu_counter=NULL;
    int *l2_lfu_counter_set_age=NULL;
    char l2_tag_bits[33]="";
    char l2_index_bits[33]="";
    char l2_index_bits_copied[33]="";
    char l2_block_offset_bits[33]="";
    int *l2_set_mapped_dirty_bit=NULL;

    char evicted_block_tag_index_combined[33]="";
    char evicted_block_tag_bits[33]="";
    char evicted_block_index_bits[33]="";
    int evicted_block_index_bits_div_10=0,evicted_block_index_bits_decimal=0;
    int l2_hit_flag=0,l2_write_hit_count=0,l2_write_miss_count=0;
    long l2_write_back=0;
    long l2_traffic=0;
    int l2_read_hit_count=0,l2_read_miss_count=0;
    char policy[4]="";
    float l2_miss_rate=0.00,l2_hit_time=0.00,l2_miss_penalty=0.00,l2_avg_access_time=0.00;
    float l2_miss_rate_mod=0.00;
    long final_traffic=0;
    //int temp6=INT_MAX;
    //general display of input parameters entered by user

    if(atoi(argv[7]) == 2) strcpy(policy, "LRU");
    else if(atoi(argv[7]) == 3) strcpy(policy, "LFU");

	//printf("\n\nHello World!\n\n");
    printf("===== Simulator configuration =====\n");
    printf("L1_BLOCKSIZE:                    %s\n",argv[1]);
    printf("L1_SIZE:                         %s\n",argv[2]);
    printf("L1_ASSOC:                        %s\n",argv[3]);
    printf("Victim_Cache_Size:               %s\n",argv[4]);
    printf("L2_SIZE:                         %s\n",argv[5]);
    printf("L2_ASSOC:                        %s\n",argv[6]);
    printf("trace_file:                      %s\n",argv[8]);
    printf("Replacement Policy:              %s\n",policy);
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
        printf("Please enter valid L1 Cache size!\n\n");
        error_flag = TRUE;
    }

    //checking if valid set associativity
    if(atoi(argv[3])>(atoi(argv[2])/atoi(argv[1])))
    {
        printf("Please enter valid L1 set associativity!\n\n");
        error_flag = TRUE;
    }
    //checking for invalid victim cache size
    if((log2(atoi(argv[4])) != floor(log2(atoi(argv[4])))) || ((atoi(argv[4]) > (atoi(argv[2])))))
    {
        printf("Invalid Victim size (or) Victim cache size cannot exceed cache size!\n\n");
        error_flag = TRUE;
    }
    //checking if valid l2 option
    if((log2(atoi(argv[5])) != floor(log2(atoi(argv[5])))))

    {
        printf("Please enter valid L2 Cache size!\n\n");
        error_flag = TRUE;
    }

    if(atoi(argv[6])>(atoi(argv[5])/atoi(argv[1])))
    {
        printf("Please enter valid L2 set associativity!\n\n");
        error_flag = TRUE;
    }

    if(atoi(argv[7]) > 3)
    {
      printf("Please enter valid L2 set associativity!\n");
      printf("Options:\n");
      printf("0<= lambda < 1: LRFU\n");
      printf("1 : LRU\n");
      printf("2 : LFU\n");
      error_flag = TRUE;
    }

    if((atof(argv[7])>0) &&(atof(argv[7])<1))
    {
      printf("LRFU policy not supported!\n");
      error_flag = TRUE;
    }
    /*
    if(atoi(argv[5])>1)
    {
        printf("Please enter valid replacement option!\n\n");
        error_flag = TRUE;
        printf("Options:\n");
        printf("0:LRU\n");
        printf("1:LFU\n");
    }

    //checking if valid write policy option
    if(atoi(argv[6])>1)
    {
        printf("Please enter valid write option!\n\n");
        error_flag = TRUE;
        printf("Options:\n");
        printf("0:WBWA\n");
        printf("1:WTNA\n");
    }
    */
    //checking for errors in trace file

    //fseek(trace_file, -1, SEEK_CUR);
    //fflush(trace_file);

    if(argv[8] == NULL)
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
    cache_block_size = atoi(argv[1]);
    total_cache_size = atoi(argv[2]);
    associativity = atoi(argv[3]);
    is_victim_cache_enabled = atoi(argv[4]);
    rep_policy = *argv[7];
    //write_policy = *argv[6];
    write_policy = '0';
    l2_cache_size = atoi(argv[5]);
    associativity_l2 = atoi(argv[6]);

    trace_file = fopen(argv[8],"r");
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
    //printf("%c-way Set Associative Cache detected!\n",associativity);
    cache_blocks_number = total_cache_size/((associativity)*cache_block_size);
    block_offset = floor(log2(cache_block_size));
    block_index = floor(log2(cache_blocks_number));
    block_tag = 32 - (block_offset + block_index);
    sets = associativity;
    //printf("No. of Tag bits = %d\n",block_tag);
    //printf("No. of Index bits = %d\n",block_index);
    //printf("No. of Block Offset bits = %d\n",block_offset);
    //printf("Initialising cache array for tag store, LRU and LFU counter......\n");
    //have to allocate a 2d strings array where both dimensions are known only at run time
    //here the other dimension is set to 33*sizeof 2nd dimension
    //https://stackoverflow.com/questions/14583144/making-2d-array-of-strings-in-c
    //https://stackoverflow.com/questions/917783/how-do-i-work-with-dynamic-multi-dimensional-arrays-in-c

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

    //initalise the valid bit arrays for l1 cache
    set_mapped_cache_valid_arrays = (int *)malloc(sizeof(int)*cache_blocks_number*sets);
    if(set_mapped_cache_valid_arrays == NULL) printf("Cache valid array init failed!\n");
    //else printf("Cache init success!\n");
    //initialising the tag array with zeros
    for(i=0;i<sets;i++)
    {
        for(j=0;j<cache_blocks_number;j++)
        {
            set_mapped_cache_valid_arrays[(i*cache_blocks_number) + j] = 0;

        }
    }
    //printf("Cache valid bit arrays initialised to 0\n");


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

    set_mapped_dirty_bit = (int *)malloc(sizeof(int)*cache_blocks_number*sets);
    if(set_mapped_dirty_bit == NULL) printf("Dirty bits array init failed!\n");
    //else printf("Dirty bit array init success!\n");
    for(i=0;i<sets;i++)
    {
    for(j=0;j<cache_blocks_number;j++)
        {
            set_mapped_dirty_bit[(i*cache_blocks_number) + j]=0;
        }
    }


    //creating victim cache
    if(is_victim_cache_enabled != 0)
    {
        victim_cache_blocks_number = 1; //fully associative victim cache
        victim_cache_sets = (is_victim_cache_enabled/cache_block_size);

        set_mapped_victim_cache_tag_arrays = (char(*)[33])malloc(sizeof(char[33])*victim_cache_blocks_number*victim_cache_sets);
        if(set_mapped_victim_cache_tag_arrays == NULL) printf("Victim Cache init failed!\n");

        set_mapped_victim_cache_valid_arrays = (int *)malloc(sizeof(int)*victim_cache_blocks_number*victim_cache_sets);
        if(set_mapped_victim_cache_valid_arrays == NULL) printf("Victim cache valid bit array init failed!\n");

        set_mapped_victim_dirty_bit = (int *)malloc(sizeof(int)*victim_cache_blocks_number*victim_cache_sets);
        if(set_mapped_victim_dirty_bit == NULL) printf("Victim cache dirty bit init failed!\n");

        //intialise the valid and tag array

        for(i=0;i<victim_cache_sets;i++)
        {
            for(j=0;j<victim_cache_blocks_number;j++)
            {
                memset(set_mapped_victim_cache_tag_arrays[(i*victim_cache_blocks_number) + j], 0, 33*sizeof(char));
                set_mapped_victim_cache_valid_arrays[(i*victim_cache_blocks_number) + j] = 0;
                set_mapped_victim_dirty_bit[(i*victim_cache_blocks_number) + j] = 0;

            }
        }
        //printing tag and valid array's contents
        //printf("Initialised the victim cache tag  and valid arrays with zeros\n");
        /*
        for(i=0;i<victim_cache_sets;i++)
        {
            for(j=0;j<victim_cache_blocks_number;j++)
            {
                printf("Victim cache tag address at set %d of index %d: %s\n",i,j,set_mapped_victim_cache_tag_arrays[(i*victim_cache_blocks_number) + j]);
            }
            printf("\n\n");
        }
        */
        /*

        for(i=0;i<victim_cache_sets;i++)
        {
            for(j=0;j<victim_cache_blocks_number;j++)
            {
                printf("Victim cache valid bit at set %d of index %d: %d\n",i,j,set_mapped_victim_cache_valid_arrays[(i*victim_cache_blocks_number) + j]);
            }
            printf("\n\n");
        }
        */
        victim_cache_lru_counter = (int*)malloc(sizeof(int)*victim_cache_blocks_number*victim_cache_sets);
        if(victim_cache_lru_counter == NULL) printf("Victim cache LRU counter init failed!\n");

        temp1 = victim_cache_sets-1;
        for(i=0;i<victim_cache_sets;i++)
        {
            for(j=0;j<victim_cache_blocks_number;j++)
            {
                victim_cache_lru_counter[(i*victim_cache_blocks_number) + j]=temp1;

            }
            temp1 = temp1 - 1;
        }
        temp1=0;
        //printf("Initialised victim cache LRU counter values\n");

    }
    //else printf("Victim cache is disabled!\n");

    if(l2_cache_size != 0)
    {
      //l2 cache has been enabled
      //printf("L2 cache enabled\n");
      //initalise tag arrays, dirty bit arrays, lfu counters, lfu counter age, lru counter, valid arrays
      //calculate all required bits
      l2_cache_blocks_number = l2_cache_size/((associativity_l2)*cache_block_size);
      l2_block_index = floor(log2(l2_cache_blocks_number));
      l2_block_tag = 32 - (block_offset + l2_block_index);
      l2_sets = associativity_l2;
      //printf("no of L2 tag bits: %d\n",l2_block_tag);
      //printf("no of L2 index bits: %d\n",l2_block_index);

      l2_set_mapped_cache_tag_arrays = (char(*)[33])malloc(sizeof(char[33])*l2_cache_blocks_number*l2_sets);
      if(l2_set_mapped_cache_tag_arrays == NULL) printf("L2 Cache init failed!\n");
      //else printf("L2 Cache init success!\n");
      //initialising the tag array with zeros
      for(i=0;i<l2_sets;i++)
      {
          for(j=0;j<l2_cache_blocks_number;j++)
          {
              memset(l2_set_mapped_cache_tag_arrays[(i*l2_cache_blocks_number) + j], 0, 33*sizeof(char));

          }
      }

      l2_set_mapped_cache_valid_arrays = (int *)malloc(sizeof(int)*l2_cache_blocks_number*l2_sets);
      if(l2_set_mapped_cache_valid_arrays == NULL) printf("L2 Cache valid array init failed!\n");
      //else printf("L2 Cache init success!\n");
      //initialising the tag array with zeros
      for(i=0;i<l2_sets;i++)
      {
          for(j=0;j<l2_cache_blocks_number;j++)
          {
              l2_set_mapped_cache_valid_arrays[(i*l2_cache_blocks_number) + j] = 0;

          }
      }

      l2_lru_counter = (int*)malloc(sizeof(int)*l2_cache_blocks_number*l2_sets);
      if(l2_lru_counter == NULL) printf("L2 LRU counter init failed!\n");
      //else printf("LRU counter init success!\n");
      //adding random values to lru_counter for initial setup
      l2_temp1 = l2_sets - 1;
      for(i=0;i<l2_sets;i++)
      {
          for(j=0;j<l2_cache_blocks_number;j++)
          {
              l2_lru_counter[(i*l2_cache_blocks_number) + j]=l2_temp1;

          }
          l2_temp1 = l2_temp1 - 1;
      }
      l2_temp1=0;

      l2_lfu_counter = (int *)malloc(sizeof(int)*l2_cache_blocks_number*l2_sets);
      if(l2_lfu_counter == NULL) printf("L2 LFU counter init failed!\n");
      //else printf("LFU counter init success!\n");
      //initialising the counters to zero
      for(i=0;i<l2_sets;i++)
      {
          for(j=0;j<l2_cache_blocks_number;j++)
          {
              l2_lfu_counter[(i*l2_cache_blocks_number)+j] = 0;
          }
      }

      l2_lfu_counter_set_age = (int*)malloc(sizeof(int)*l2_cache_blocks_number);
      if(l2_lfu_counter_set_age == NULL) printf("L2 LFU counters age init failed!\n");
      //else printf("LFU counters age init success!\n");
      for(i=0;i<l2_cache_blocks_number;i++)
      {
          l2_lfu_counter_set_age[i] = 0;
      }

      l2_set_mapped_dirty_bit = (int *)malloc(sizeof(int)*l2_cache_blocks_number*l2_sets);
      if(l2_set_mapped_dirty_bit == NULL) printf("L2 Dirty bits array init failed!\n");
      //else printf("Dirty bit array init success!\n");
      for(i=0;i<l2_sets;i++)
      {
      for(j=0;j<l2_cache_blocks_number;j++)
          {
              l2_set_mapped_dirty_bit[(i*l2_cache_blocks_number) + j]=0;
          }
      }

    }
    //else printf("L2 Cache disabled\n");
    //fseek(trace_file, 0, SEEK_SET);
    //fseek(trace_file, -1, SEEK_SET);
//}



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
            /*
            //printf("CPU Address in Binary(Reversed) %ld = %s\n",line,cpu_address_binary_reversed);
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
            //printf("The L1 tag field includes: %s\n",tag_bits);
            //printf("The L1 index field includes: %s\n",index_bits);
            //printf("The L1 block offset field includes: %s\n",block_offset_bits);
            //printf("String length of tag field: %lu\n",strlen(tag_bits));
            //printf("String length of index field: %lu\n",strlen(index_bits));
            //printf("String length of block offset field: %lu\n",strlen(block_offset_bits));
            if(is_victim_cache_enabled != 0)
            {
                //printf("Incoming tag bits for L1 cache: %s\n",tag_bits);
                //printf("Incoming index bits for L1 cache: %s\n",index_bits);
                strcat(victim_tag_bits,tag_bits);
                strcat(victim_tag_bits,index_bits);
                //printf("Victim tag bits are: %s\n",victim_tag_bits);
            }

            //initialising the tag, index bits for L2
            if(l2_cache_size != 0)
            {
              strncpy(l2_tag_bits,cpu_address_binary,l2_block_tag);
              strncpy(l2_index_bits,cpu_address_binary+l2_block_tag,l2_block_index);
              strncpy(l2_block_offset_bits,cpu_address_binary+l2_block_tag+l2_block_index,block_offset);
              //printf("The L2 tag field includes: %s\n",l2_tag_bits);
              //printf("The L2 index field includes: %s\n",l2_index_bits);
              //printf("The L2 block offset field includes: %s\n",l2_block_offset_bits);
              strcpy(l2_index_bits_copied,l2_index_bits);//a copy of it is saved for future uses

              while(atoi(l2_index_bits) != 0)
              {
                  l2_index_bits_decimal = l2_index_bits_decimal + base*(atoi(l2_index_bits)%10);
                  l2_index_bits_div_10 = atoi(l2_index_bits) / 10;
                  base=base*2;
                  snprintf(l2_index_bits, l2_block_index, "%d", l2_index_bits_div_10);
              }
              base=1;
              //printf("L2 Index in decimal : %d\n",l2_index_bits_decimal);
            }

            strcpy(index_bits_copied,index_bits);//since index bits is used up below, a copy of it is saved for future uses
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

            //if(rep_policy == '0')
            if(rep_policy == '2')
            {
                //lru detected
                if(write_policy == '0')
                {
                    //wbwa detected
                    if(is_victim_cache_enabled != 0)
                    {
                      temp1=0,temp2=0,temp3=0,temp4=0,temp5=0,temp6=0,temp7=0,temp8=0,temp9=0,temp10=0;
                      l1_cache_hit_flag = 0;
                      victim_cache_hit_flag = 0;
                      //printf("Now searching for tag bits in L1 Cache\n");

                      for(j=0;j<sets;j++)
                        {
                            if(strcmp(set_mapped_cache_tag_arrays[(j*cache_blocks_number) + index_bits_decimal],tag_bits)==0)
                            {
                                l1_cache_hit_flag = 1;
                                victim_cache_hit_flag = 0;
                                temp1 = j;
                                temp2 = lru_counter[(j*cache_blocks_number) + index_bits_decimal];
                                //printf("L1 cache hit!\n");
                                break;
                            }
                        }

                      if((l1_cache_hit_flag == 0)&&(victim_cache_hit_flag == 0))
                        {
                            //printf("Now searching for tag bits in victim Cache\n");
                            for(j=0;j<victim_cache_sets;j++)
                            {
                                if(strcmp(set_mapped_victim_cache_tag_arrays[j*victim_cache_blocks_number],victim_tag_bits)==0)
                                {
                                    victim_cache_hit_flag = 1;
                                    l1_cache_hit_flag = 0;
                                    temp7 = j;
                                    temp8 = victim_cache_lru_counter[(j*victim_cache_blocks_number)];
                                    //printf("Victim cache hit!\n");
                                    break;
                                }
                            }
                        }

                        //l1 cache miss and victim cache hit
                      if((l1_cache_hit_flag == 0)&&(victim_cache_hit_flag == 1))
                      {
                            //printf("L1 Cache miss and victim cache hit!\n");
                            l1_victim_swaps++;
                            //update counter in victim cache
                            //update counter in l1 and choose a spot
                            //copy tag from that spot to victim cache along with dirty status
                            //now copy the incoming tag from proc to the just emptied spot and set dirty bit to 1
                            victim_cache_lru_counter[(temp7*victim_cache_blocks_number)] = 0;
                            for(j=0;j<victim_cache_sets;j++)
                            {
                                if(j!=temp7)
                                {
                                    if((victim_cache_lru_counter[j*victim_cache_blocks_number]) < temp8)
                                    {
                                        victim_cache_lru_counter[j*victim_cache_blocks_number]++;
                                        //printf("Incremented the victim cache lru counter\n");
                                    }
                                }
                            }
                            //printf("Updated victim cache LRU counters\n");
                            //now find a spot in L1
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
                            //temp4 is the spot
                            lru_counter[(temp4*cache_blocks_number) + index_bits_decimal] = 0;
                            for(j=0;j<sets;j++)
                            {
                                if(j!=temp4)
                                lru_counter[(j*cache_blocks_number) + index_bits_decimal]++;

                            }
                            //printf("Updated L1 cache LRU counters!\n");
                            //now transferring tag from L1 to victim
                            strcat(l1_victim_tag_bits,set_mapped_cache_tag_arrays[(temp4*cache_blocks_number) + index_bits_decimal]);
                            strcat(l1_victim_tag_bits,index_bits_copied);
                            //printf("victim tag bits before appending index to it: %s\n",set_mapped_cache_tag_arrays[(temp4*cache_blocks_number) + index_bits_decimal]);
                            //printf("victim tag bits after appending index to it: %s\n",l1_victim_tag_bits);
                            strcpy(set_mapped_victim_cache_tag_arrays[(temp7*victim_cache_blocks_number)],l1_victim_tag_bits);
                            //printf("Copied tag from L1 to victim!\n");

                            for(j=0;j<victim_cache_sets;j++)
                            {
                                //printf("Updated tag addresses in victim cache: %s\n",set_mapped_victim_cache_tag_arrays[(j*victim_cache_blocks_number)]);

                            }

                            if(set_mapped_victim_dirty_bit[temp7*victim_cache_blocks_number] == 1) victim_swap =1; else victim_swap =0;
                            if(set_mapped_dirty_bit[(temp4*cache_blocks_number) + index_bits_decimal] == 1) set_mapped_victim_dirty_bit[temp7*victim_cache_blocks_number] = 1; else set_mapped_victim_dirty_bit[temp7*victim_cache_blocks_number] = 0;
                            if(victim_swap == 1) set_mapped_dirty_bit[(temp4*cache_blocks_number) + index_bits_decimal] = 1; else set_mapped_dirty_bit[(temp4*cache_blocks_number) + index_bits_decimal] = 0;

                            strcpy(set_mapped_cache_tag_arrays[(temp4*cache_blocks_number) + index_bits_decimal],tag_bits);

                            for(j=0;j<sets;j++)
                            {
                                //printf("Updated tag addresses in L1 cache at index %d: %s\n",index_bits_decimal,set_mapped_cache_tag_arrays[(j*cache_blocks_number) + index_bits_decimal]);
                            }

                            if(read_write == 'r')
                            {
                                read_hit_count++;
                                //set_mapped_dirty_bit[(temp4*cache_blocks_number) + index_bits_decimal] = 0;
                                //here dirty bit status is taken care by the swap between L1 and victim
                            }

                            if(read_write == 'w')
                            {
                                write_hit_count++;
                                set_mapped_dirty_bit[(temp4*cache_blocks_number) + index_bits_decimal] = 1;
                            }
                        }

                        //L1 cache hit and victim cache miss
                      if((l1_cache_hit_flag == 1)&&(victim_cache_hit_flag == 0))
                      {
                            //printf("L1 cache hit and Victim cache miss!\n");
                            //update l1 lru counter
                            //dirty bit is set in l1 if write
                            //in victim cache nothing needs to be done
                            lru_counter[(temp1*cache_blocks_number) + index_bits_decimal] = 0;
                            for(j=0;j<sets;j++)
                            {
                                if(j!=temp1)
                                {
                                    if((lru_counter[(j*cache_blocks_number) + index_bits_decimal]) < temp2)
                                    {
                                        lru_counter[(j*cache_blocks_number) + index_bits_decimal]++;
                                        //printf("incremented LRU counter of set %d\n",j);
                                    }
                                }

                            }
                            for(j=0;j<sets;j++)
                            {
                                //printf("Updated tag addresses in L1 cache at index %d: %s\n",index_bits_decimal,set_mapped_cache_tag_arrays[(j*cache_blocks_number) + index_bits_decimal]);
                            }
                            //printf("Updated L1 cache LRU counter!\n");
                            set_mapped_cache_valid_arrays[(temp1*cache_blocks_number) + index_bits_decimal] = 1;
                            if(read_write == 'r')
                            {
                                read_hit_count++;
                            }
                            if(read_write == 'w')
                            {
                                write_hit_count++;
                                set_mapped_dirty_bit[(temp1*cache_blocks_number) + index_bits_decimal] = 1;

                            }
                        }


                      //L1 and victim cache miss
                      if((l1_cache_hit_flag == 0)&&(victim_cache_hit_flag == 0))
                      {
                          //printf("Miss in both L1 and victim cache!\n");
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
                          lru_counter[(temp4*cache_blocks_number) + index_bits_decimal] = 0;
                          for(j=0;j<sets;j++)
                          {
                              if(j!=temp4)
                              lru_counter[(j*cache_blocks_number) + index_bits_decimal]++;

                          }
                          //printf("Updated L1 cache LRU counter!\n");
                          if(set_mapped_cache_valid_arrays[(temp4*cache_blocks_number) + index_bits_decimal] == 1)
                          {
                              //printf("Valid bit detected!\n");
                              //finding the LRU value in victim cache, appending tag bits with index bits and copying and transferring dirty status
                              //also set dirty bit for the new incoming tag entry
                              temp5 = 0;
                              temp6 = 0;
                              //searching for the highest LRU counter value and setting it to zero
                              for(j=0;j<victim_cache_sets;j++)
                              {
                                  if(victim_cache_lru_counter[(j*victim_cache_blocks_number)] > temp5)
                                  {
                                      temp5 = victim_cache_lru_counter[(j*victim_cache_blocks_number)];
                                      temp6 = j;
                                  }
                              }
                              victim_cache_lru_counter[(temp6*victim_cache_blocks_number)] = 0;
                              for(j=0;j<victim_cache_sets;j++)
                              {
                                  if(j!=temp6)
                                  victim_cache_lru_counter[(j*victim_cache_blocks_number)]++;

                              }
                              //printf("Updated LRU counters for victim cache\n");
                              strcat(l1_victim_tag_bits,set_mapped_cache_tag_arrays[(temp4*cache_blocks_number) + index_bits_decimal]);
                              strcat(l1_victim_tag_bits,index_bits_copied);
                              //printf("victim tag bits before appending index to it: %s\n",set_mapped_cache_tag_arrays[(temp4*cache_blocks_number) + index_bits_decimal]);
                              //printf("victim tag bits after appending index to it: %s\n",l1_victim_tag_bits);
                              //printf("Copied tag from L1 to victim!\n");
                              if(set_mapped_victim_dirty_bit[temp6*victim_cache_blocks_number] == 1)
                              {
                                  victim_cache_write_back++;
                                  traffic++;
                                  set_mapped_victim_dirty_bit[temp6*victim_cache_blocks_number] = 0;
                                  //send data to L2
                                  //check if L2 is enabled and send write request to l2 followed by read request
                                  //modify the tag bits to fit into L2
                                  if(l2_cache_size != 0)
                                  {
                                    //printf("Dirty bit detected so writing to L2\n");
                                    strncpy(evicted_block_tag_bits,set_mapped_victim_cache_tag_arrays[(temp6*victim_cache_blocks_number)],l2_block_tag);
                                    strncpy(evicted_block_index_bits,set_mapped_victim_cache_tag_arrays[(temp6*victim_cache_blocks_number)]+l2_block_tag,l2_block_index);
                                    //printf("Converted victim tag bit for L2\n");
                                    //printf("Tag bit going to L2: %s",evicted_block_tag_bits);
                                    //printf("Index bits going to L2: %s",evicted_block_index_bits);
                                    //convert the index bits to decimal
                                    while(atoi(evicted_block_index_bits) != 0)
                                    {
                                        evicted_block_index_bits_decimal = evicted_block_index_bits_decimal + base*(atoi(evicted_block_index_bits)%10);
                                        evicted_block_index_bits_div_10 = atoi(evicted_block_index_bits) / 10;
                                        base = base*2;
                                        snprintf(evicted_block_index_bits, l2_block_index, "%d", evicted_block_index_bits_div_10);
                                    }
                                    base=1;
                                    //printf("the evicted_block_index_bits in decimal is: %d\n",evicted_block_index_bits_decimal);

                                    //printf("Sending write request to L2\n");
                                    for(j=0;j<l2_sets;j++)
                                    {
                                      if(strcmp(l2_set_mapped_cache_tag_arrays[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal],evicted_block_tag_bits)==0)
                                      {
                                        l2_hit_flag = 1;
                                        temp9 = j;
                                        temp10 = l2_lru_counter[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal];
                                        break;
                                      }
                                      else l2_hit_flag = 0;
                                    }

                                    if(l2_hit_flag == 1)
                                    {
                                      //printf("write hit in L2\n");
                                      l2_write_hit_count++;
                                      l2_set_mapped_dirty_bit[(temp9*l2_cache_blocks_number) + evicted_block_index_bits_decimal] = 1;
                                      l2_lru_counter[(temp9*l2_cache_blocks_number) + evicted_block_index_bits_decimal] = 0;

                                      for(j=0;j<l2_sets;j++)
                                      {
                                          if(j!=temp9)
                                          {
                                              if((l2_lru_counter[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal]) < temp10)
                                              {
                                                  l2_lru_counter[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal]++;
                                                  //printf("incremented LRU counter of set %d\n",j);
                                              }
                                          }

                                      }
                                    }

                                    else if(l2_hit_flag == 0)
                                    {
                                      //printf("write miss in L2\n");
                                      l2_write_miss_count++;
                                      l2_traffic++;
                                      temp11 = 0;
                                      temp12 = 0;
                                      //searching for the highest LRU counter value and setting it to zero
                                      for(j=0;j<l2_sets;j++)
                                      {
                                          if(l2_lru_counter[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal] > temp11)
                                          {
                                              temp11 = l2_lru_counter[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal];
                                              temp12 = j;
                                          }
                                      }

                                      if(l2_set_mapped_dirty_bit[(temp12*l2_cache_blocks_number) + evicted_block_index_bits_decimal] == 1)
                                      {
                                          l2_write_back++;
                                          l2_set_mapped_dirty_bit[(temp12*l2_cache_blocks_number) + evicted_block_index_bits_decimal] = 0;
                                          l2_traffic++;
                                      }



                                      l2_set_mapped_dirty_bit[(temp12*l2_cache_blocks_number) + evicted_block_index_bits_decimal] = 1;

                                      strcpy(l2_set_mapped_cache_tag_arrays[(temp12*l2_cache_blocks_number) + evicted_block_index_bits_decimal],evicted_block_tag_bits);

                                      if(strcpy(l2_set_mapped_cache_tag_arrays[(temp12*l2_cache_blocks_number) + evicted_block_index_bits_decimal],evicted_block_tag_bits) ==NULL) exit(EXIT_FAILURE);

                                      l2_lru_counter[(temp12*l2_cache_blocks_number) + evicted_block_index_bits_decimal] = 0;

                                      for(j=0;j<l2_sets;j++)
                                      {
                                          //printf("Updated Tag address at set %d of L2: %s\n",j,l2_set_mapped_cache_tag_arrays[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal]);
                                      }

                                      //except for the recently modified lru counter, rest all counters are incremented
                                      //printf("LRU Counter value at set %d of index %d of L2: %d\n",temp12,evicted_block_index_bits_decimal,l2_lru_counter[(temp12*l2_cache_blocks_number) + evicted_block_index_bits_decimal]);

                                      for(j=0;j<l2_sets;j++)
                                      {
                                          if(j!=temp12)
                                          l2_lru_counter[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal]++;

                                      }

                                      //printf("Updated values of LRU counters\n");
                                      for(j=0;j<l2_sets;j++)
                                      {
                                          //printf("Set %d of L2: %d\n",j,l2_lru_counter[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal]);
                                      }
                                    }


                                  }


                              }

                              strcpy(set_mapped_victim_cache_tag_arrays[(temp6*victim_cache_blocks_number)],l1_victim_tag_bits);
                              if(set_mapped_dirty_bit[(temp4*cache_blocks_number) + index_bits_decimal] == 1)
                              {
                                  set_mapped_victim_dirty_bit[temp6*victim_cache_blocks_number] = 1;
                              }
                              else set_mapped_victim_dirty_bit[temp6*victim_cache_blocks_number] = 0;

                              //set victim cache valid bit
                              set_mapped_victim_cache_valid_arrays[temp6*victim_cache_blocks_number] = 1;
                              //printf("Victim valid bit set\n");
                              for(j=0;j<victim_cache_sets;j++)
                              {
                                  //printf("Updated tag addresses in victim cache: %s\n",set_mapped_victim_cache_tag_arrays[(j*victim_cache_blocks_number)]);

                              }

                          }
                          //set l1 cache valid bit
                          //printf("Setting valid bit!\n");
                          set_mapped_cache_valid_arrays[(temp4*cache_blocks_number) + index_bits_decimal] = 1;

                          strcpy(set_mapped_cache_tag_arrays[(temp4*cache_blocks_number) + index_bits_decimal],tag_bits);
                          //printf("Copied incoming tag bits to L1 cache and VC updated if valid bit was found!\n");
                          for(j=0;j<sets;j++)
                          {
                              //printf("Updated tag addresses in L1 cache at index %d: %s\n",index_bits_decimal,set_mapped_cache_tag_arrays[(j*cache_blocks_number) + index_bits_decimal]);
                          }

                          if(l2_cache_size != 0)
                          {
                            //printf("L2 Cache is enabled and sending read request now\n");
                            for(j=0;j<l2_sets;j++)
                            {
                              //printf("tag add present in L2 at set %d: %s\n",j,l2_set_mapped_cache_tag_arrays[(j*l2_cache_blocks_number) + l2_index_bits_decimal]);
                            }
                            //printf("incoming tag add is: %s\n",l2_tag_bits);
                            for(j=0;j<l2_sets;j++)
                            {
                              //printf("Current LRU counter in set %d of L2: %d\n",j,l2_lru_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal]);
                            }

                            //now send a read request to L2 Cache
                            for(j=0;j<l2_sets;j++)
                            {
                              if(strcmp(l2_set_mapped_cache_tag_arrays[(j*l2_cache_blocks_number) + l2_index_bits_decimal],l2_tag_bits)==0)
                              {
                                //printf("read hit at set %d\n",j);
                                l2_hit_flag = 1;
                                temp9 = j;
                                temp10 = l2_lru_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal];
                                break;
                              }
                              else l2_hit_flag = 0;
                            }

                            if(l2_hit_flag == 1)
                            {
                                //printf("Read hit in L2 at set %d\n",j);
                                l2_read_hit_count++;
                                l2_lru_counter[(temp9*l2_cache_blocks_number) + l2_index_bits_decimal] = 0;
                                for(j=0;j<l2_sets;j++)
                                {
                                    if(j!=temp9)
                                    {
                                        if((l2_lru_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal]) < temp10)
                                        {
                                            l2_lru_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal]++;
                                            //printf("incremented LRU counter of set %d\n",j);
                                        }
                                    }

                                }

                            }

                            else if(l2_hit_flag == 0)
                            {
                              //printf("L2 Miss at line %ld\n",line);
                              temp11 = 0;
                              temp12 = 0;
                              //searching for the highest LRU counter value and setting it to zero
                              for(j=0;j<l2_sets;j++)
                              {
                                  if(l2_lru_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal] > temp11)
                                  {
                                      temp11 = l2_lru_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal];
                                      temp12 = j;
                                  }
                              }
                              //printf("highest LRU of l2 fouund is %d at set %d\n",temp11,temp12);
                              if(l2_set_mapped_dirty_bit[(temp12*l2_cache_blocks_number) + l2_index_bits_decimal] == 1)
                              {
                                  l2_write_back++;
                                  l2_set_mapped_dirty_bit[(temp12*l2_cache_blocks_number) + l2_index_bits_decimal] = 0;
                                  l2_traffic++;
                              }

                              l2_read_miss_count++;
                              l2_traffic++;

                              l2_set_mapped_dirty_bit[(temp12*l2_cache_blocks_number) + l2_index_bits_decimal] = 0;

                              strcpy(l2_set_mapped_cache_tag_arrays[(temp12*l2_cache_blocks_number) + l2_index_bits_decimal],l2_tag_bits);

                              if(strcpy(l2_set_mapped_cache_tag_arrays[(temp12*l2_cache_blocks_number) + l2_index_bits_decimal],l2_tag_bits) ==NULL) exit(EXIT_FAILURE);

                              l2_lru_counter[(temp12*l2_cache_blocks_number) + l2_index_bits_decimal] = 0;

                              for(j=0;j<l2_sets;j++)
                              {
                                  //printf("Updated Tag address at set %d of L2: %s\n",j,l2_set_mapped_cache_tag_arrays[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal]);
                              }

                              //except for the recently modified lru counter, rest all counters are incremented
                              //printf("LRU Counter value at set %d of index %d of L2: %d\n",temp12,evicted_block_index_bits_decimal,l2_lru_counter[(temp12*l2_cache_blocks_number) + evicted_block_index_bits_decimal]);

                              for(j=0;j<l2_sets;j++)
                              {
                                  if(j!=temp12)
                                  l2_lru_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal]++;

                              }

                              //printf("Updated values of LRU counters\n");
                              for(j=0;j<l2_sets;j++)
                              {
                                  //printf("Set %d of L2: %d\n",j,l2_lru_counter[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal]);
                              }
                            }



                          }

                          if(read_write == 'r')
                          {
                              read_miss_count++;
                              traffic++;
                              set_mapped_dirty_bit[(temp4*cache_blocks_number) + index_bits_decimal] = 0;//since data has been brought from lower level to L1

                          }

                          if(read_write == 'w')
                          {
                              write_miss_count++;
                              traffic++;
                              set_mapped_dirty_bit[(temp4*cache_blocks_number) + index_bits_decimal] = 1;
                          }
                          //now if l2 is enabled send a read request to it

                      }


                      //print updated l1 and victim counter values
                      for(j=0;j<sets;j++)
                      {
                          //printf("Updated L1 LRU counter value at set %d: %d\n",j,lru_counter[(j*cache_blocks_number) + index_bits_decimal]);
                      }
                      for(j=0;j<victim_cache_sets;j++)
                      {
                          //printf("Updated Victim cache LRU counter value at set %d: %d\n",j,victim_cache_lru_counter[(j*victim_cache_blocks_number)]);
                      }
                    }

                    else if(is_victim_cache_enabled == 0)
                    {

                            for(j=0;j<sets;j++)
                            {
                                if(strcmp(set_mapped_cache_tag_arrays[(j*cache_blocks_number) + index_bits_decimal],tag_bits)==0)
                                {
                                    hit_flag = 1;
                                    temp1 = j;
                                    temp2 = lru_counter[(j*cache_blocks_number) + index_bits_decimal];
                                    break;
                                }
                                else hit_flag =0;
                            }
                            //cache hit
                            if(hit_flag == 1)
                            {
                                if(read_write == 'r')
                                {
                                    read_hit_count++;
                                }
                                else if(read_write =='w')
                                {
                                    write_hit_count++;
                                    set_mapped_dirty_bit[(temp1*cache_blocks_number) + index_bits_decimal] = 1;

                                }

                                lru_counter[(temp1*cache_blocks_number) + index_bits_decimal] = 0;
                                for(j=0;j<sets;j++)
                                {
                                    if(j!=temp1)
                                    {
                                        if((lru_counter[(j*cache_blocks_number) + index_bits_decimal]) < temp2)
                                        {
                                            lru_counter[(j*cache_blocks_number) + index_bits_decimal]++;
                                            //printf("incremented LRU counter of set %d\n",j);
                                        }
                                    }

                                }

                            }
                            //cache miss
                            else if(hit_flag == 0)
                            {

                                //update counter
                                //copy tag
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

                                if(set_mapped_dirty_bit[(temp4*cache_blocks_number) + index_bits_decimal] == 1)
                                {
                                    write_back = write_back + 1;
                                    set_mapped_dirty_bit[(temp4*cache_blocks_number) + index_bits_decimal] = 0;
                                    traffic++;
                                      if(l2_cache_size != 0)
                                      {
                                        //printf("L2 is enabled and now sending the evicted block to L2\n");
                                        //remodel the tag and index bits to fit the L2 configuration
                                        strcat(evicted_block_tag_index_combined,set_mapped_cache_tag_arrays[(temp4*cache_blocks_number) + index_bits_decimal]);
                                        strcat(evicted_block_tag_index_combined,index_bits_copied);
                                        //printf("the combined tag and index address is: %s\n",evicted_block_tag_index_combined);
                                        strncpy(evicted_block_tag_bits,evicted_block_tag_index_combined,l2_block_tag);
                                        strncpy(evicted_block_index_bits,evicted_block_tag_index_combined + l2_block_tag,l2_block_index);
                                        //printf("the new tag bit for L2 is: %s\n",evicted_block_tag_bits);
                                        //printf("the new index bit for L2 is: %s\n",evicted_block_index_bits);

                                        //now send write request to L2 cache with the generated tag and index bits
                                        //also convert the index bits to decimal
                                        while(atoi(evicted_block_index_bits) != 0)
                                        {
                                            evicted_block_index_bits_decimal = evicted_block_index_bits_decimal + base*(atoi(evicted_block_index_bits)%10);
                                            evicted_block_index_bits_div_10 = atoi(evicted_block_index_bits) / 10;
                                            base = base*2;
                                            snprintf(evicted_block_index_bits, l2_block_index, "%d", evicted_block_index_bits_div_10);
                                        }
                                        base=1;
                                        //printf("the evicted_block_index_bits in decimal is: %d\n",evicted_block_index_bits_decimal);
                                        //now sending write request to L2
                                        //printf("Sending write request to L2\n");
                                        for(j=0;j<l2_sets;j++)
                                        {
                                          if(strcmp(l2_set_mapped_cache_tag_arrays[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal],evicted_block_tag_bits)==0)
                                          {
                                            l2_hit_flag = 1;
                                            temp9 = j;
                                            temp10 = l2_lru_counter[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal];
                                            break;
                                          }
                                          else l2_hit_flag = 0;
                                        }

                                        if(l2_hit_flag == 1)
                                        {
                                          //printf("write hit in L2\n");
                                          l2_write_hit_count++;
                                          l2_set_mapped_dirty_bit[(temp9*l2_cache_blocks_number) + evicted_block_index_bits_decimal] = 1;
                                          l2_lru_counter[(temp9*l2_cache_blocks_number) + evicted_block_index_bits_decimal] = 0;

                                          for(j=0;j<l2_sets;j++)
                                          {
                                              if(j!=temp9)
                                              {
                                                  if((l2_lru_counter[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal]) < temp10)
                                                  {
                                                      l2_lru_counter[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal]++;
                                                      //printf("incremented LRU counter of set %d\n",j);
                                                  }
                                              }

                                          }
                                        }

                                        else if(l2_hit_flag == 0)
                                        {
                                          //printf("write miss in L2\n");
                                          l2_write_miss_count++;
                                          l2_traffic++;
                                          temp11 = 0;
                                          temp12 = 0;
                                          //searching for the highest LRU counter value and setting it to zero
                                          for(j=0;j<l2_sets;j++)
                                          {
                                              if(l2_lru_counter[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal] > temp11)
                                              {
                                                  temp11 = l2_lru_counter[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal];
                                                  temp12 = j;
                                              }
                                          }

                                          if(l2_set_mapped_dirty_bit[(temp12*l2_cache_blocks_number) + evicted_block_index_bits_decimal] == 1)
                                          {
                                              l2_write_back++;
                                              l2_set_mapped_dirty_bit[(temp12*l2_cache_blocks_number) + evicted_block_index_bits_decimal] = 0;
                                              l2_traffic++;
                                          }



                                          l2_set_mapped_dirty_bit[(temp12*l2_cache_blocks_number) + evicted_block_index_bits_decimal] = 1;

                                          strcpy(l2_set_mapped_cache_tag_arrays[(temp12*l2_cache_blocks_number) + evicted_block_index_bits_decimal],evicted_block_tag_bits);

                                          if(strcpy(l2_set_mapped_cache_tag_arrays[(temp12*l2_cache_blocks_number) + evicted_block_index_bits_decimal],evicted_block_tag_bits) ==NULL) exit(EXIT_FAILURE);

                                          l2_lru_counter[(temp12*l2_cache_blocks_number) + evicted_block_index_bits_decimal] = 0;

                                          for(j=0;j<l2_sets;j++)
                                          {
                                              //printf("Updated Tag address at set %d of L2: %s\n",j,l2_set_mapped_cache_tag_arrays[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal]);
                                          }

                                          //except for the recently modified lru counter, rest all counters are incremented
                                          //printf("LRU Counter value at set %d of index %d of L2: %d\n",temp12,evicted_block_index_bits_decimal,l2_lru_counter[(temp12*l2_cache_blocks_number) + evicted_block_index_bits_decimal]);

                                          for(j=0;j<l2_sets;j++)
                                          {
                                              if(j!=temp12)
                                              l2_lru_counter[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal]++;

                                          }

                                          //printf("Updated values of LRU counters\n");
                                          for(j=0;j<l2_sets;j++)
                                          {
                                              //printf("Set %d of L2: %d\n",j,l2_lru_counter[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal]);
                                          }
                                        }

                                      }
                                }
                                if(read_write == 'r')
                                {
                                    read_miss_count++;
                                    set_mapped_dirty_bit[(temp4*cache_blocks_number) + index_bits_decimal] = 0;
                                    //might have to set dirty bit to zero
                                    traffic++;
                                }
                                else if(read_write == 'w')
                                {
                                    write_miss_count++;
                                    traffic++;
                                    set_mapped_dirty_bit[(temp4*cache_blocks_number) + index_bits_decimal] = 1;
                                }
                                strcpy(set_mapped_cache_tag_arrays[(temp4*cache_blocks_number) + index_bits_decimal],tag_bits);

                                if(strcpy(set_mapped_cache_tag_arrays[(temp4*cache_blocks_number) + index_bits_decimal],tag_bits) ==NULL) exit(EXIT_FAILURE);

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

                                //now check if L2 is enabled and start L2 routine
                                if(l2_cache_size != 0)
                                {
                                  //printf("L2 Cache is enabled and sending read request now\n");
                                  //now send a read request to L2 Cache
                                  for(j=0;j<l2_sets;j++)
                                  {
                                    if(strcmp(l2_set_mapped_cache_tag_arrays[(j*l2_cache_blocks_number) + l2_index_bits_decimal],l2_tag_bits)==0)
                                    {
                                      l2_hit_flag = 1;
                                      temp9 = j;
                                      temp10 = l2_lru_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal];
                                      break;
                                    }
                                    else l2_hit_flag = 0;
                                  }

                                  if(l2_hit_flag == 1)
                                  {
                                      //printf("Read hit in L2\n");
                                      l2_read_hit_count++;
                                      l2_lru_counter[(temp9*l2_cache_blocks_number) + l2_index_bits_decimal] = 0;
                                      for(j=0;j<l2_sets;j++)
                                      {
                                          if(j!=temp9)
                                          {
                                              if((l2_lru_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal]) < temp10)
                                              {
                                                  l2_lru_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal]++;
                                                  //printf("incremented LRU counter of set %d\n",j);
                                              }
                                          }

                                      }

                                  }

                                  else if(l2_hit_flag == 0)
                                  {
                                    //printf("read miss in L2\n");
                                    temp11 = 0;
                                    temp12 = 0;
                                    //searching for the highest LRU counter value and setting it to zero
                                    for(j=0;j<l2_sets;j++)
                                    {
                                        if(l2_lru_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal] > temp11)
                                        {
                                            temp11 = l2_lru_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal];
                                            temp12 = j;
                                        }
                                    }

                                    if(l2_set_mapped_dirty_bit[(temp12*l2_cache_blocks_number) + l2_index_bits_decimal] == 1)
                                    {
                                        l2_write_back++;
                                        l2_set_mapped_dirty_bit[(temp12*l2_cache_blocks_number) + l2_index_bits_decimal] = 0;
                                        l2_traffic++;
                                    }

                                  l2_read_miss_count++;
                                  l2_traffic++;

                                    l2_set_mapped_dirty_bit[(temp12*l2_cache_blocks_number) + l2_index_bits_decimal] = 0;

                                    strcpy(l2_set_mapped_cache_tag_arrays[(temp12*l2_cache_blocks_number) + l2_index_bits_decimal],l2_tag_bits);

                                    if(strcpy(l2_set_mapped_cache_tag_arrays[(temp12*l2_cache_blocks_number) + l2_index_bits_decimal],l2_tag_bits) ==NULL) exit(EXIT_FAILURE);

                                    l2_lru_counter[(temp12*l2_cache_blocks_number) + l2_index_bits_decimal] = 0;

                                    for(j=0;j<l2_sets;j++)
                                    {
                                        //printf("Updated Tag address at set %d of L2: %s\n",j,l2_set_mapped_cache_tag_arrays[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal]);
                                    }

                                    //except for the recently modified lru counter, rest all counters are incremented
                                    //printf("LRU Counter value at set %d of index %d of L2: %d\n",temp12,evicted_block_index_bits_decimal,l2_lru_counter[(temp12*l2_cache_blocks_number) + evicted_block_index_bits_decimal]);

                                    for(j=0;j<l2_sets;j++)
                                    {
                                        if(j!=temp12)
                                        l2_lru_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal]++;

                                    }

                                    //printf("Updated values of LRU counters\n");
                                    for(j=0;j<l2_sets;j++)
                                    {
                                        //printf("Set %d of L2: %d\n",j,l2_lru_counter[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal]);
                                    }
                                  }



                                }

                            }


                    }

                }


                else if(write_policy == '1')
                {
                    for(j=0;j<sets;j++)
                        {
                            if(strcmp(set_mapped_cache_tag_arrays[(j*cache_blocks_number) + index_bits_decimal],tag_bits)==0)
                            {
                                //printf("hit at set %d\n",j);
                                hit_flag =1;
                                temp1 = j;
                                temp2 = lru_counter[(j*cache_blocks_number) + index_bits_decimal];
                                lru_counter[(j*cache_blocks_number) + index_bits_decimal] = 0;

                                for(j=0;j<sets;j++)
                                {
                                    if(j!=temp1)
                                    {
                                        if((lru_counter[(j*cache_blocks_number) + index_bits_decimal]) < temp2)
                                        {
                                            lru_counter[(j*cache_blocks_number) + index_bits_decimal]++;
                                            //printf("incremented LRU counter of set %d\n",j);
                                        }
                                    }

                                }
                                //printf("Updated values of LRU counters\n");
                                //for(j=0;j<sets;j++)
                                //{
                                    //printf("Set %d: %d\n",j,lru_counter[(j*cache_blocks_number) + index_bits_decimal]);
                                //}

                                break;
                            }
                            else hit_flag =0;
                        }
                            //cache hit
                        if(hit_flag ==1)
                        {


                            if(read_write == 'r')
                            {
                                read_hit_count++;
                            }
                            else if(read_write =='w')
                            {
                                write_hit_count++;
                                traffic++;
                            }




                        }
                        else if(hit_flag == 0)
                        {
                            //printf("miss\n");
                            //update counter
                            //copy tag
                            //lru bits=log2(no.of sets)
                            //check for lru value
                            //strcpy to the highest value lru flag
                            //set that lru to zero
                            //inc all other lru counter values by 1
                            //request data from lower level caches
                            if(read_write == 'r')
                            {

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
                            for(j=0;j<sets;j++)
                            {
                                lru_counter[(j*cache_blocks_number) + index_bits_decimal]++;

                            }

                            lru_counter[(temp4*cache_blocks_number) + index_bits_decimal] = 0;
                            //printf("Updated values of LRU counters\n");
                            //for(j=0;j<sets;j++)
                            //{
                                //printf("Set %d: %d\n",j,lru_counter[(j*cache_blocks_number) + index_bits_decimal]);
                            //}


                                strcpy(set_mapped_cache_tag_arrays[(temp4*cache_blocks_number) + index_bits_decimal],tag_bits);
                                //printf("tag copied\n");
                                if(strcpy(set_mapped_cache_tag_arrays[(temp4*cache_blocks_number) + index_bits_decimal],tag_bits) == NULL) exit(EXIT_FAILURE);
                                read_miss_count++;
                                traffic++;

                            }
                            else if(read_write == 'w')
                            {
                                write_miss_count++;
                                //printf("tag not copied\n");
                                traffic++;
                            }
                            //for(j=0;j<sets;j++)
                            //{
                                //printf("Updated Tag address at set %d: %s\n",j,set_mapped_cache_tag_arrays[(j*cache_blocks_number) + index_bits_decimal]);
                            //}

                            //except for the recently modified lru counter, rest all counters are incremented
                            //printf("LRU Counter value at set %d of index %d: %d\n",temp4,index_bits_decimal,lru_counter[(temp4*cache_blocks_number) + index_bits_decimal]);



                            //printf("Updated values of LRU counters\n");
                            //for(j=0;j<sets;j++)
                            //{
                                //printf("Set %d: %d\n",j,lru_counter[(j*cache_blocks_number) + index_bits_decimal]);
                            //}
                        }
                }

            }
            //else if(rep_policy == '1')
            else if(rep_policy == '3')
            {
                //lfu detected
                if(write_policy == '0')
                {
                    //wbwa detected
                    if(is_victim_cache_enabled != 0)
                    {
                      for(j=0;j<sets;j++)
                      {
                        //printf("(before)LFU counter value at set %d: %d\n",j,lfu_counter[(j*cache_blocks_number) + index_bits_decimal]);
                      }
                      temp1=0,temp2=0,temp3=0,temp4=0,temp5=0,temp6=0,temp7=0,temp8=0,temp9=0,temp10=0;
                      //NOTE: acc to spec sheet, even though this is LFU and WBWA routine, replacement for victim cache would still be LRU and L1,L2 will folow LFU
                      //printf("Now searching for tag bits in L1 Cache\n");
                      l1_cache_hit_flag = 0;
                      victim_cache_hit_flag = 0;
                      for(j=0;j<sets;j++)
                        {
                            if(strcmp(set_mapped_cache_tag_arrays[(j*cache_blocks_number) + index_bits_decimal],tag_bits)==0)
                            {
                                l1_cache_hit_flag = 1;
                                victim_cache_hit_flag = 0;
                                temp1 = j;
                                temp2 = lfu_counter[(j*cache_blocks_number) + index_bits_decimal];
                                //printf("L1 cache hit!\n");
                                break;
                            }
                        }

                        if((l1_cache_hit_flag == 0)&&(victim_cache_hit_flag == 0))
                          {
                              //printf("Now searching for tag bits in victim Cache\n");
                              for(j=0;j<victim_cache_sets;j++)
                              {
                                  if(strcmp(set_mapped_victim_cache_tag_arrays[j*victim_cache_blocks_number],victim_tag_bits)==0)
                                  {
                                      victim_cache_hit_flag = 1;
                                      l1_cache_hit_flag = 0;
                                      temp7 = j;
                                      temp8 = victim_cache_lru_counter[(j*victim_cache_blocks_number)];
                                      //printf("Victim cache hit!\n");
                                      break;
                                  }
                              }
                          }

                          if((l1_cache_hit_flag == 0)&&(victim_cache_hit_flag == 1))
                          {
                                //printf("L1 Cache miss and victim cache hit!\n");
                                l1_victim_swaps++;
                                //update counter in victim cache
                                //update counter in l1 and choose a spot
                                //copy tag from that spot to victim cache along with dirty status
                                //now copy the incoming tag from proc to the just emptied spot and set dirty bit to 1
                                victim_cache_lru_counter[(temp7*victim_cache_blocks_number)] = 0;
                                for(j=0;j<victim_cache_sets;j++)
                                {
                                    if(j!=temp7)
                                    {
                                        if((victim_cache_lru_counter[j*victim_cache_blocks_number]) < temp8)
                                        {
                                            victim_cache_lru_counter[j*victim_cache_blocks_number]++;
                                            //printf("Incremented the victim cache lru counter\n");
                                        }
                                    }
                                }
                                //printf("Updated victim cache LRU counters\n");
                                //now find a spot in L1
                                temp3 = INT_MAX;
                                temp4 = 0;
                                //searching for the lowest LFU counter value and incrementing to age+1
                                for(j=0;j<sets;j++)
                                {
                                    if(lfu_counter[(j*cache_blocks_number) + index_bits_decimal] < temp3)
                                    {
                                        temp3 = lfu_counter[(j*cache_blocks_number) + index_bits_decimal];

                                    }
                                }
                                for(j=0;j<sets;j++)
                                {
                                    if(lfu_counter[(j*cache_blocks_number) + index_bits_decimal] == temp3)
                                    {
                                        temp4 = j;
                                        break;
                                    }
                                }
                                //temp4 is the spot
                                lfu_counter_set_age[index_bits_decimal] = lfu_counter[(temp4*cache_blocks_number) + index_bits_decimal];
                                lfu_counter[(temp4*cache_blocks_number) + index_bits_decimal] = lfu_counter_set_age[index_bits_decimal] + 1;

                                //printf("Updated L1 cache LFU counters!\n");
                                //now transferring tag from L1 to victim
                                strcat(l1_victim_tag_bits,set_mapped_cache_tag_arrays[(temp4*cache_blocks_number) + index_bits_decimal]);
                                strcat(l1_victim_tag_bits,index_bits_copied);
                                //printf("victim tag bits before appending index to it: %s\n",set_mapped_cache_tag_arrays[(temp4*cache_blocks_number) + index_bits_decimal]);
                                //printf("victim tag bits after appending index to it: %s\n",l1_victim_tag_bits);
                                strcpy(set_mapped_victim_cache_tag_arrays[(temp7*victim_cache_blocks_number)],l1_victim_tag_bits);
                                //printf("Copied tag from L1 to victim!\n");

                                for(j=0;j<victim_cache_sets;j++)
                                {
                                    //printf("Updated tag addresses in victim cache: %s\n",set_mapped_victim_cache_tag_arrays[(j*victim_cache_blocks_number)]);

                                }

                                if(set_mapped_victim_dirty_bit[temp7*victim_cache_blocks_number] == 1) victim_swap =1; else victim_swap =0;
                                if(set_mapped_dirty_bit[(temp4*cache_blocks_number) + index_bits_decimal] == 1) set_mapped_victim_dirty_bit[temp7*victim_cache_blocks_number] = 1; else set_mapped_victim_dirty_bit[temp7*victim_cache_blocks_number] = 0;
                                if(victim_swap == 1) set_mapped_dirty_bit[(temp4*cache_blocks_number) + index_bits_decimal] = 1; else set_mapped_dirty_bit[(temp4*cache_blocks_number) + index_bits_decimal] = 0;
                                //printf("Swapped dirty bits\n");

                                strcpy(set_mapped_cache_tag_arrays[(temp4*cache_blocks_number) + index_bits_decimal],tag_bits);
                                //printf("Copied tag bits VC proc to L1\n");

                                for(j=0;j<sets;j++)
                                {
                                    ///printf("Updated tag addresses in L1 cache at index %d: %s\n",index_bits_decimal,set_mapped_cache_tag_arrays[(j*cache_blocks_number) + index_bits_decimal]);
                                }

                                if(read_write == 'r')
                                {
                                    read_hit_count++;
                                    //set_mapped_dirty_bit[(temp4*cache_blocks_number) + index_bits_decimal] = 0;
                                    //here dirty bit status is taken care by the swap between L1 and victim
                                }

                                if(read_write == 'w')
                                {
                                    write_hit_count++;
                                    set_mapped_dirty_bit[(temp4*cache_blocks_number) + index_bits_decimal] = 1;
                                }
                          }

                            //L1 cache hit and victim cache miss
                          if((l1_cache_hit_flag == 1)&&(victim_cache_hit_flag == 0))
                          {
                                  //printf("L1 cache hit and Victim cache miss!\n");
                                  //update l1 lfu counter
                                  //dirty bit is set in l1 if write
                                  //in victim cache nothing needs to be done
                                  lfu_counter[(temp1*cache_blocks_number) + index_bits_decimal]++;

                                  for(j=0;j<sets;j++)
                                  {
                                      //printf("Updated tag addresses in L1 cache at index %d: %s\n",index_bits_decimal,set_mapped_cache_tag_arrays[(j*cache_blocks_number) + index_bits_decimal]);
                                  }
                                  //printf("Updated L1 cache LFU counter!\n");
                                  //printf("nothing modified in victim cache\n");
                                  set_mapped_cache_valid_arrays[(temp1*cache_blocks_number) + index_bits_decimal] = 1;
                                  if(read_write == 'r')
                                  {
                                      read_hit_count++;
                                  }
                                  if(read_write == 'w')
                                  {
                                      write_hit_count++;
                                      set_mapped_dirty_bit[(temp1*cache_blocks_number) + index_bits_decimal] = 1;

                                  }
                          }

                              //L1 and victim cache miss
                          if((l1_cache_hit_flag == 0)&&(victim_cache_hit_flag == 0))
                          {
                              //printf("Miss in both L1 and victim cache!\n");
                              temp3 = INT_MAX;
                              temp4 = 0;
                              //searching for the lowest LFU counter value and incrementing it to age+1
                              for(j=0;j<sets;j++)
                              {
                                  if(lfu_counter[(j*cache_blocks_number) + index_bits_decimal] < temp3)
                                  {
                                      temp3 = lfu_counter[(j*cache_blocks_number) + index_bits_decimal];

                                  }
                              }
                              for(j=0;j<sets;j++)
                              {
                                  if(lfu_counter[(j*cache_blocks_number) + index_bits_decimal] == temp3)
                                  {
                                      temp4 = j;
                                      break;
                                  }
                              }

                              lfu_counter_set_age[index_bits_decimal] = lfu_counter[(temp4*cache_blocks_number) + index_bits_decimal];
                              lfu_counter[(temp4*cache_blocks_number) + index_bits_decimal] = lfu_counter_set_age[index_bits_decimal] + 1;
                              //printf("Updated L1 cache LFU counter!\n");
                              if(set_mapped_cache_valid_arrays[(temp4*cache_blocks_number) + index_bits_decimal] == 1)
                              {
                                  //printf("Valid bit detected in L1!\n");
                                  //finding the LRU value in victim cache, appending tag bits with index bits and copying and transferring dirty status
                                  //also set dirty bit for the new incoming tag entry
                                  temp5 = 0;
                                  temp6 = 0;
                                  //searching for the highest LRU counter value and setting it to zero
                                  for(j=0;j<victim_cache_sets;j++)
                                  {
                                      if(victim_cache_lru_counter[(j*victim_cache_blocks_number)] > temp5)
                                      {
                                          temp5 = victim_cache_lru_counter[(j*victim_cache_blocks_number)];
                                          temp6 = j;
                                      }
                                  }
                                  victim_cache_lru_counter[(temp6*victim_cache_blocks_number)] = 0;
                                  for(j=0;j<victim_cache_sets;j++)
                                  {
                                      if(j!=temp6)
                                      victim_cache_lru_counter[(j*victim_cache_blocks_number)]++;

                                  }
                                  //printf("Updated 'LRU' counters for victim cache\n");
                                  strcat(l1_victim_tag_bits,set_mapped_cache_tag_arrays[(temp4*cache_blocks_number) + index_bits_decimal]);
                                  strcat(l1_victim_tag_bits,index_bits_copied);
                                  //printf("tag bits of evcited block from L1: %s\n",set_mapped_cache_tag_arrays[(temp4*cache_blocks_number) + index_bits_decimal]);
                                  //printf("tag bits from L1 modified for Victim cache: %s\n",l1_victim_tag_bits);

                                  if(set_mapped_victim_dirty_bit[temp6*victim_cache_blocks_number] == 1)
                                  {
                                      //printf("block to be evicted from victim found dirty\n");
                                      victim_cache_write_back++;
                                      traffic++;
                                      set_mapped_victim_dirty_bit[temp6*victim_cache_blocks_number] = 0;
                                      //send data to L2
                                      //check if L2 is enabled and send write request to l2 followed by read request
                                      //modify the tag bits to fit into L2
                                      if(l2_cache_size != 0)
                                      {
                                        //printf("Dirty bit detected in VC so writing to L2\n");
                                        strncpy(evicted_block_tag_bits,set_mapped_victim_cache_tag_arrays[(temp6*victim_cache_blocks_number)],l2_block_tag);
                                        strncpy(evicted_block_index_bits,set_mapped_victim_cache_tag_arrays[(temp6*victim_cache_blocks_number)]+l2_block_tag,l2_block_index);
                                        //printf("Converted victim tag bit for L2\n");
                                        //printf("Tag bit going to L2: %s",evicted_block_tag_bits);
                                        //printf("Index bits going to L2: %s",evicted_block_index_bits);
                                        //convert the index bits to decimal
                                        while(atoi(evicted_block_index_bits) != 0)
                                        {
                                            evicted_block_index_bits_decimal = evicted_block_index_bits_decimal + base*(atoi(evicted_block_index_bits)%10);
                                            evicted_block_index_bits_div_10 = atoi(evicted_block_index_bits) / 10;
                                            base = base*2;
                                            snprintf(evicted_block_index_bits, l2_block_index, "%d", evicted_block_index_bits_div_10);
                                        }
                                        base=1;
                                        //printf("the evicted_block_index_bits in decimal is: %d\n",evicted_block_index_bits_decimal);

                                        //printf("Sending write request to L2\n");
                                        for(j=0;j<l2_sets;j++)
                                        {
                                          temp13=0;temp14=0;l2_hit_flag=0;
                                            if(strcmp(l2_set_mapped_cache_tag_arrays[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal],evicted_block_tag_bits)==0)
                                            {
                                                l2_hit_flag = 1;
                                                temp13 = j;
                                                temp14 = l2_lfu_counter[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal];


                                                break;
                                            }
                                            else l2_hit_flag = 0;
                                        }

                                        if(l2_hit_flag == 1)
                                        {
                                            //printf("write hit in L2 at set %d\n",temp13);
                                            //cache hit
                                            //update counter
                                            //copy tag
                                            //printf("Hit at Set %d\n",temp1);

                                            //printf("inc lfu counter\n");
                                            //printf("Updated values of LFU counters\n");
                                            //for(j=0;j<sets;j++)
                                            //{
                                                //printf("Set %d: %d\n",j,lfu_counter[(j*cache_blocks_number) + index_bits_decimal]);
                                           // }
                                            l2_lfu_counter[(temp13*l2_cache_blocks_number) + evicted_block_index_bits_decimal]++;
                                            l2_write_hit_count++;
                                            l2_set_mapped_dirty_bit[(temp13*l2_cache_blocks_number) + evicted_block_index_bits_decimal] = 1;
                                            //printf("updated LFU counter of L2 and set Dirty\n");
                                                //printf("Dirty set\n");

                                        }

                                        else if(l2_hit_flag == 0)
                                        {
                                          //printf("write miss in L2\n");
                                          l2_write_miss_count++;
                                          l2_traffic++;
                                          temp15 = INT_MAX;
                                          //printf("INT_MAX = %d\n",temp6);
                                          temp16 = 0;
                                          //searching for the lowest LFU counter value and setting it to zero
                                          for(j=0;j<l2_sets;j++)
                                          {
                                              if(l2_lfu_counter[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal] < temp15)
                                              {
                                                  temp15 = l2_lfu_counter[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal];
                                              }
                                          }

                                          for(j=0;j<l2_sets;j++)
                                          {
                                              if(l2_lfu_counter[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal] == temp15)
                                              {
                                                  temp16 = j;
                                                  break;
                                              }
                                          }

                                          if(l2_set_mapped_dirty_bit[(temp16*l2_cache_blocks_number) + evicted_block_index_bits_decimal] == 1)
                                          {
                                            l2_write_back++;
                                            l2_set_mapped_dirty_bit[(temp16*l2_cache_blocks_number) + evicted_block_index_bits_decimal] = 0;
                                            l2_traffic++;
                                          }

                                          l2_set_mapped_dirty_bit[(temp16*l2_cache_blocks_number) + evicted_block_index_bits_decimal] = 1;

                                          l2_lfu_counter_set_age[evicted_block_index_bits_decimal] = l2_lfu_counter[(temp16*l2_cache_blocks_number) + evicted_block_index_bits_decimal];
                                          l2_lfu_counter_set_age[evicted_block_index_bits_decimal] = temp15;
                                          l2_lfu_counter[(temp16*l2_cache_blocks_number) + evicted_block_index_bits_decimal] = l2_lfu_counter_set_age[evicted_block_index_bits_decimal] + 1;

                                          strcpy(l2_set_mapped_cache_tag_arrays[(temp16*l2_cache_blocks_number) + evicted_block_index_bits_decimal],evicted_block_tag_bits);
                                          //printf("Updated LFU counter of L2\n");
                                          if(strcpy(l2_set_mapped_cache_tag_arrays[(temp16*l2_cache_blocks_number) + evicted_block_index_bits_decimal],evicted_block_tag_bits) == NULL) exit(EXIT_FAILURE);
                                          //printf("Copied tag from victim cache to L2\n");


                                        }


                                      }


                                  }

                                  //printf("Copied tag from L1 to victim!\n");
                                  strcpy(set_mapped_victim_cache_tag_arrays[(temp6*victim_cache_blocks_number)],l1_victim_tag_bits);
                                  if(set_mapped_dirty_bit[(temp4*cache_blocks_number) + index_bits_decimal] == 1)
                                  {
                                      set_mapped_victim_dirty_bit[temp6*victim_cache_blocks_number] = 1;
                                  }
                                  else set_mapped_victim_dirty_bit[temp6*victim_cache_blocks_number] = 0;
                                  //printf("set victim dirty if L1 block was dirty\n");
                                  //set victim cache valid bit
                                  set_mapped_victim_cache_valid_arrays[temp6*victim_cache_blocks_number] = 1;
                                  //printf("Victim valid bit set\n");
                                  for(j=0;j<victim_cache_sets;j++)
                                  {
                                      //printf("Updated tag addresses in victim cache: %s\n",set_mapped_victim_cache_tag_arrays[(j*victim_cache_blocks_number)]);

                                  }

                              }
                              //set l1 cache valid bit
                              //printf("Setting valid bit!\n");
                              set_mapped_cache_valid_arrays[(temp4*cache_blocks_number) + index_bits_decimal] = 1;

                              strcpy(set_mapped_cache_tag_arrays[(temp4*cache_blocks_number) + index_bits_decimal],tag_bits);
                              //printf("Copied incoming tag bits to L1 cache and VC updated if valid bit was found!\n");
                              for(j=0;j<sets;j++)
                              {
                                  //printf("Updated tag addresses in L1 cache at index %d: %s\n",index_bits_decimal,set_mapped_cache_tag_arrays[(j*cache_blocks_number) + index_bits_decimal]);
                              }

                              //now if l2 is enabled send a read request to it
                              if(l2_cache_size != 0)
                              {
                              //  printf("L2 Cache is enabled and sending read request now\n");

                                for(j=0;j<l2_sets;j++)
                                {
                                    if(strcmp(l2_set_mapped_cache_tag_arrays[(j*l2_cache_blocks_number) + l2_index_bits_decimal],l2_tag_bits)==0)
                                    {
                                        l2_hit_flag = 1;
                                        temp17 = j;
                                        temp18 = l2_lfu_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal];

                                        break;
                                    }
                                    else l2_hit_flag = 0;
                                }

                                if(l2_hit_flag == 1)
                                {
                                    //printf("Read hit at set %d in L2\n", temp17);
                                    //cache hit
                                    //update counter
                                    //copy tag
                                    //printf("Hit at Set %d\n",temp1);

                                    //printf("inc lfu counter\n");
                                    //printf("Updated values of LFU counters\n");
                                    //for(j=0;j<sets;j++)
                                    //{
                                        //printf("Set %d: %d\n",j,lfu_counter[(j*cache_blocks_number) + index_bits_decimal]);
                                   // }
                                   l2_lfu_counter[(temp17*l2_cache_blocks_number) + l2_index_bits_decimal]++;
                                    l2_read_hit_count++;
                                    //printf("updated LFU counter of L2\n");
                                }
                                else if(l2_hit_flag == 0)
                                {
                                  //printf("read miss in L2\n");
                                  temp19 = INT_MAX;
                                  //printf("INT_MAX = %d\n",temp6);
                                  temp20 = 0;
                                  //searching for the lowest LFU counter value and setting it to zero
                                  for(j=0;j<l2_sets;j++)
                                  {
                                      if(l2_lfu_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal] < temp19)
                                      {
                                          temp19 = l2_lfu_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal];
                                      }
                                  }

                                  for(j=0;j<l2_sets;j++)
                                  {
                                      if(l2_lfu_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal] == temp19)
                                      {
                                          temp20 = j;
                                          break;
                                      }
                                  }
                                  //printf("updated LFU counter of L2\n");
                                  if(l2_set_mapped_dirty_bit[(temp20*l2_cache_blocks_number) + l2_index_bits_decimal] == 1)
                                  {
                                    l2_write_back++;
                                    l2_set_mapped_dirty_bit[(temp20*l2_cache_blocks_number) + l2_index_bits_decimal] = 0;
                                    l2_traffic++;
                                  }

                                  l2_read_miss_count++;
                                  l2_traffic++;
                                  l2_set_mapped_dirty_bit[(temp20*l2_cache_blocks_number) + l2_index_bits_decimal] = 0;
                                  //printf("cleared dirty since read miss\n");
                                  strcpy(l2_set_mapped_cache_tag_arrays[(temp20*l2_cache_blocks_number) + l2_index_bits_decimal],l2_tag_bits);

                                  if(strcpy(l2_set_mapped_cache_tag_arrays[(temp20*l2_cache_blocks_number) + l2_index_bits_decimal],l2_tag_bits) == NULL) exit(EXIT_FAILURE);
                                  //printf("copied tag bits fro proc to L2\n");
                                  l2_lfu_counter_set_age[l2_index_bits_decimal] = l2_lfu_counter[(temp20*l2_cache_blocks_number) + l2_index_bits_decimal];
                                  l2_lfu_counter_set_age[l2_index_bits_decimal] = temp19;
                                  l2_lfu_counter[(temp20*l2_cache_blocks_number) + l2_index_bits_decimal] = l2_lfu_counter_set_age[l2_index_bits_decimal] + 1;
                                  //printf("LFU counter of L2 updated\n");




                                }

                              }


                              if(read_write == 'r')
                              {
                                  read_miss_count++;
                                  traffic++;
                                  set_mapped_dirty_bit[(temp4*cache_blocks_number) + index_bits_decimal] = 0;//since data has been brought from lower level to L1

                              }

                              if(read_write == 'w')
                              {
                                  write_miss_count++;
                                  traffic++;
                                  set_mapped_dirty_bit[(temp4*cache_blocks_number) + index_bits_decimal] = 1;
                              }


                          }

                          for(j=0;j<sets;j++)
                          {
                            //printf("(after)LFU counter value at set %d: %d\n",j,lfu_counter[(j*cache_blocks_number) + index_bits_decimal]);
                          }

                    }

                    else if(is_victim_cache_enabled == 0)
                    {
                      for(j=0;j<sets;j++)
                      {
                          if(strcmp(set_mapped_cache_tag_arrays[(j*cache_blocks_number) + index_bits_decimal],tag_bits)==0)
                          {
                              hit_flag = 1;
                              temp1 = j;
                              temp2 = lfu_counter[(j*cache_blocks_number) + index_bits_decimal];
                              break;
                          }
                          else hit_flag = 0;
                      }
                          if(hit_flag == 1)
                          {
                              //cache hit
                              //update counter
                              //copy tag
                              //printf("Hit at Set %d\n",temp1);

                              //printf("inc lfu counter\n");
                              //printf("Updated values of LFU counters\n");
                              //for(j=0;j<sets;j++)
                              //{
                                  //printf("Set %d: %d\n",j,lfu_counter[(j*cache_blocks_number) + index_bits_decimal]);
                             // }
                             lfu_counter[(temp1*cache_blocks_number) + index_bits_decimal]++;
                             if(read_write == 'r')
                              {
                                  read_hit_count++;
                              }
                              else if(read_write =='w')
                              {
                                  write_hit_count++;
                                  set_mapped_dirty_bit[(temp1*cache_blocks_number) + index_bits_decimal] = 1;
                                  //printf("Dirty set\n");

                              }

                          }
                          //cache miss
                          else if(hit_flag == 0)
                          {
                              //printf("Miss\n");
                              //update counter
                              //copy tag
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
                                  if(lfu_counter[(j*cache_blocks_number) + index_bits_decimal] < temp6)
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
                              if(set_mapped_dirty_bit[(temp7*cache_blocks_number) + index_bits_decimal] == 1)
                              {
                                  write_back = write_back + 1;
                                  set_mapped_dirty_bit[(temp7*cache_blocks_number) + index_bits_decimal] = 0;
                                  traffic++;
                                  if(l2_cache_size != 0)
                                  {
                                    //printf("L2 is enabled and now sending the evicted block to L2\n");
                                    //remodel the tag and index bits to fit the L2 configuration
                                    strcat(evicted_block_tag_index_combined,set_mapped_cache_tag_arrays[(temp7*cache_blocks_number) + index_bits_decimal]);
                                    strcat(evicted_block_tag_index_combined,index_bits_copied);
                                    //printf("the combined tag and index address is: %s\n",evicted_block_tag_index_combined);
                                    strncpy(evicted_block_tag_bits,evicted_block_tag_index_combined,l2_block_tag);
                                    strncpy(evicted_block_index_bits,evicted_block_tag_index_combined + l2_block_tag,l2_block_index);
                                    //printf("the new tag bit for L2 is: %s\n",evicted_block_tag_bits);
                                    //printf("the new index bit for L2 is: %s\n",evicted_block_index_bits);

                                    //now send write request to L2 cache with the generated tag and index bits
                                    //also convert the index bits to decimal
                                    while(atoi(evicted_block_index_bits) != 0)
                                    {
                                        evicted_block_index_bits_decimal = evicted_block_index_bits_decimal + base*(atoi(evicted_block_index_bits)%10);
                                        evicted_block_index_bits_div_10 = atoi(evicted_block_index_bits) / 10;
                                        base = base*2;
                                        snprintf(evicted_block_index_bits, l2_block_index, "%d", evicted_block_index_bits_div_10);
                                    }
                                    base=1;

                                    for(j=0;j<l2_sets;j++)
                                    {
                                        if(strcmp(l2_set_mapped_cache_tag_arrays[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal],evicted_block_tag_bits)==0)
                                        {
                                            l2_hit_flag = 1;
                                            temp13 = j;
                                            temp14 = l2_lfu_counter[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal];


                                            break;
                                        }
                                        else l2_hit_flag = 0;
                                    }

                                    if(l2_hit_flag == 1)
                                    {
                                        //printf("write hit in L2\n");
                                        //cache hit
                                        //update counter
                                        //copy tag
                                        //printf("Hit at Set %d\n",temp1);

                                        //printf("inc lfu counter\n");
                                        //printf("Updated values of LFU counters\n");
                                        //for(j=0;j<sets;j++)
                                        //{
                                            //printf("Set %d: %d\n",j,lfu_counter[(j*cache_blocks_number) + index_bits_decimal]);
                                       // }
                                        l2_lfu_counter[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal]++;
                                        l2_write_hit_count++;
                                        l2_set_mapped_dirty_bit[(temp13*l2_cache_blocks_number) + evicted_block_index_bits_decimal] = 1;
                                            //printf("Dirty set\n");

                                    }
                                    else if(l2_hit_flag == 0)
                                    {
                                      //printf("write miss in L2\n");
                                      l2_write_miss_count++;
                                      l2_traffic++;
                                      temp15 = INT_MAX;
                                      //printf("INT_MAX = %d\n",temp6);
                                      temp16 = 0;
                                      //searching for the lowest LFU counter value and setting it to zero
                                      for(j=0;j<l2_sets;j++)
                                      {
                                          if(l2_lfu_counter[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal] < temp15)
                                          {
                                              temp15 = l2_lfu_counter[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal];
                                          }
                                      }

                                      for(j=0;j<l2_sets;j++)
                                      {
                                          if(l2_lfu_counter[(j*l2_cache_blocks_number) + evicted_block_index_bits_decimal] == temp15)
                                          {
                                              temp16 = j;
                                              break;
                                          }
                                      }

                                      if(l2_set_mapped_dirty_bit[(temp16*l2_cache_blocks_number) + evicted_block_index_bits_decimal] == 1)
                                      {
                                        l2_write_back++;
                                        l2_set_mapped_dirty_bit[(temp16*l2_cache_blocks_number) + evicted_block_index_bits_decimal] = 0;
                                        l2_traffic++;
                                      }

                                      l2_set_mapped_dirty_bit[(temp16*l2_cache_blocks_number) + evicted_block_index_bits_decimal] = 1;

                                      l2_lfu_counter_set_age[evicted_block_index_bits_decimal] = l2_lfu_counter[(temp16*l2_cache_blocks_number) + evicted_block_index_bits_decimal];
                                      l2_lfu_counter_set_age[evicted_block_index_bits_decimal] = temp15;
                                      l2_lfu_counter[(temp16*l2_cache_blocks_number) + evicted_block_index_bits_decimal] = l2_lfu_counter_set_age[evicted_block_index_bits_decimal] + 1;

                                      strcpy(l2_set_mapped_cache_tag_arrays[(temp16*l2_cache_blocks_number) + evicted_block_index_bits_decimal],evicted_block_tag_bits);

                                      if(strcpy(l2_set_mapped_cache_tag_arrays[(temp16*l2_cache_blocks_number) + evicted_block_index_bits_decimal],evicted_block_tag_bits) == NULL) exit(EXIT_FAILURE);



                                    }



                                  }
                              }

                              if(read_write == 'r')
                              {
                                  read_miss_count++;
                                  set_mapped_dirty_bit[(temp7*cache_blocks_number) + index_bits_decimal] = 0;
                                  traffic++;
                              }

                              else if(read_write == 'w')
                              {
                                  write_miss_count++;
                                  traffic++;
                                  //printf("Dirty set\n");
                                  set_mapped_dirty_bit[(temp7*cache_blocks_number) + index_bits_decimal] = 1;
                              }

                              strcpy(set_mapped_cache_tag_arrays[(temp7*cache_blocks_number) + index_bits_decimal],tag_bits);

                              if(strcpy(set_mapped_cache_tag_arrays[(temp7*cache_blocks_number) + index_bits_decimal],tag_bits) == NULL) exit(EXIT_FAILURE);
                              //printf("Lowest value of counter found is: %d\n",temp6);
                              //printf("First lowest value found at set %d\n",temp7);
                              lfu_counter_set_age[index_bits_decimal] = lfu_counter[(temp7*cache_blocks_number) + index_bits_decimal];
                              //lfu_counter_set_age[index_bits_decimal] = temp6;
                              lfu_counter[(temp7*cache_blocks_number) + index_bits_decimal] = lfu_counter_set_age[index_bits_decimal] + 1;
                              //printf("Age has been set and counter has been incremented\n");


                              //for(j=0;j<sets;j++)
                              //{
                                  //printf("Updated Tag address at set %d: %s\n",j,set_mapped_cache_tag_arrays[(j*cache_blocks_number) + index_bits_decimal]);
                              //}

                              //printf("LFU Counter value at set %d of index %d: %d\n",temp7,index_bits_decimal,lfu_counter[(temp7*cache_blocks_number) + index_bits_decimal]);

                              //printf("Counter Age value of index %d: %d\n",index_bits_decimal,lfu_counter_set_age[index_bits_decimal]);
                              //printf("Updated values of LFU counters\n");
                              //for(j=0;j<sets;j++)
                              //{
                                  //printf("Set %d: %d\n",j,lfu_counter[(j*cache_blocks_number) + index_bits_decimal]);
                              //}

                              if(l2_cache_size != 0)
                              {
                                //printf("L2 Cache is enabled and sending read request now\n");

                                for(j=0;j<l2_sets;j++)
                                {
                                    if(strcmp(l2_set_mapped_cache_tag_arrays[(j*l2_cache_blocks_number) + l2_index_bits_decimal],l2_tag_bits)==0)
                                    {
                                        l2_hit_flag = 1;
                                        temp17 = j;
                                        temp18 = l2_lfu_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal];

                                        break;
                                    }
                                    else l2_hit_flag = 0;
                                }

                                if(l2_hit_flag == 1)
                                {
                                    //printf("Read hit at set %d in L2\n", temp17);
                                    //cache hit
                                    //update counter
                                    //copy tag
                                    //printf("Hit at Set %d\n",temp1);

                                    //printf("inc lfu counter\n");
                                    //printf("Updated values of LFU counters\n");
                                    //for(j=0;j<sets;j++)
                                    //{
                                        //printf("Set %d: %d\n",j,lfu_counter[(j*cache_blocks_number) + index_bits_decimal]);
                                   // }
                                   l2_lfu_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal]++;
                                    l2_read_hit_count++;
                                }
                                else if(l2_hit_flag == 0)
                                {
                                  //printf("read miss in L2\n");
                                  temp19 = INT_MAX;
                                  //printf("INT_MAX = %d\n",temp6);
                                  temp20 = 0;
                                  //searching for the lowest LFU counter value and setting it to zero
                                  for(j=0;j<l2_sets;j++)
                                  {
                                      if(l2_lfu_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal] < temp19)
                                      {
                                          temp19 = l2_lfu_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal];
                                      }
                                  }

                                  for(j=0;j<l2_sets;j++)
                                  {
                                      if(l2_lfu_counter[(j*l2_cache_blocks_number) + l2_index_bits_decimal] == temp19)
                                      {
                                          temp20 = j;
                                          break;
                                      }
                                  }

                                  if(l2_set_mapped_dirty_bit[(temp20*l2_cache_blocks_number) + l2_index_bits_decimal] == 1)
                                  {
                                    l2_write_back++;
                                    l2_set_mapped_dirty_bit[(temp20*l2_cache_blocks_number) + l2_index_bits_decimal] = 0;
                                    l2_traffic++;
                                  }

                                  l2_read_miss_count++;
                                  l2_traffic++;
                                  l2_set_mapped_dirty_bit[(temp20*l2_cache_blocks_number) + l2_index_bits_decimal] = 0;

                                  strcpy(l2_set_mapped_cache_tag_arrays[(temp20*l2_cache_blocks_number) + l2_index_bits_decimal],l2_tag_bits);

                                  if(strcpy(l2_set_mapped_cache_tag_arrays[(temp20*l2_cache_blocks_number) + l2_index_bits_decimal],l2_tag_bits) == NULL) exit(EXIT_FAILURE);

                                  l2_lfu_counter_set_age[l2_index_bits_decimal] = l2_lfu_counter[(temp20*l2_cache_blocks_number) + l2_index_bits_decimal];
                                  l2_lfu_counter_set_age[l2_index_bits_decimal] = temp19;
                                  l2_lfu_counter[(temp20*l2_cache_blocks_number) + l2_index_bits_decimal] = l2_lfu_counter_set_age[l2_index_bits_decimal] + 1;





                                }

                              }


                          }
                    }


                }


                else if(write_policy == '1')
                {
                    //wtwna detected
                    for(j=0;j<sets;j++)
                    {
                        if(strcmp(set_mapped_cache_tag_arrays[(j*cache_blocks_number) + index_bits_decimal],tag_bits)==0)
                        {
                            hit_flag =1;
                            temp1 = j;
                            temp2 = lfu_counter[(j*cache_blocks_number) + index_bits_decimal];

                            break;
                        }
                        else hit_flag =0;
                    }
                        if(hit_flag == 1)
                        {
                            //cache hit
                            //update counter
                            //copy tag
                            //printf("Hit at Set %d\n",temp1);
                            lfu_counter[(temp1*cache_blocks_number) + index_bits_decimal]++;
                            //printf("inc lfu counter\n");
                            //printf("Updated values of LFU counters\n");
                            //for(j=0;j<sets;j++)
                            //{
                                //printf("Set %d: %d\n",j,lfu_counter[(j*cache_blocks_number) + index_bits_decimal]);
                            //}

                            if(read_write == 'r')
                            {
                                read_hit_count++;
                            }
                            else if(read_write =='w')
                            {
                                write_hit_count++;
                                traffic++;

                            }

                        }
                        //cache miss
                        else if(hit_flag == 0)
                        {
                            //printf("Miss\n");
                            if(read_write == 'r')
                            {


                            //update counter
                            //copy tag
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

                            strcpy(set_mapped_cache_tag_arrays[(temp7*cache_blocks_number) + index_bits_decimal],tag_bits);

                            if(strcpy(set_mapped_cache_tag_arrays[(temp7*cache_blocks_number) + index_bits_decimal],tag_bits) == NULL) exit(EXIT_FAILURE);

                            //for(j=0;j<sets;j++)
                            //{
                                //printf("Updated Tag address at set %d: %s\n",j,set_mapped_cache_tag_arrays[(j*cache_blocks_number) + index_bits_decimal]);
                            //}

                            //printf("LFU Counter value at set %d of index %d: %d\n",temp7,index_bits_decimal,lfu_counter[(temp7*cache_blocks_number) + index_bits_decimal]);

                            //printf("Counter Age value of index %d: %d\n",index_bits_decimal,lfu_counter_set_age[index_bits_decimal]);
                            //printf("Updated values of LFU counters\n");
                            //for(j=0;j<sets;j++)
                            //{
                                //printf("Set %d: %d\n",j,lfu_counter[(j*cache_blocks_number) + index_bits_decimal]);
                            //}



                                read_miss_count++;
                                traffic++;

                            }
                            else if(read_write == 'w')
                            {
                                write_miss_count++;
                                traffic++;

                            }
                        }
                }
            }

            else
            {
                printf("Invalid action found on line %ld. Please specify r/w at this location\n",line);
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
            if(is_victim_cache_enabled != 0)
            {
              memset(victim_tag_bits,0,33*sizeof(char));
              memset(l1_victim_tag_bits,0,33*sizeof(char));
            }
            if(l2_cache_size != 0)
            {
              memset(l2_block_offset_bits,0,33*sizeof(char));
              memset(evicted_block_tag_index_combined,0,33*sizeof(char));
              memset(l2_tag_bits,0,33*sizeof(char));
              memset(l2_index_bits,0,33*sizeof(char));
              memset(evicted_block_tag_bits,0,33*sizeof(char));
              memset(evicted_block_index_bits,0,33*sizeof(char));
              evicted_block_index_bits_decimal=0;
              l2_index_bits_decimal=0;
            }



            //printf("\n");
            line++;
    }


    //parse routine ends


    //converting di
    //de allocating the index and tag array from Direct mapped cache
    //free(direct_mapped_cache_index_array);

    //if(rep_policy == '0')
    if(rep_policy == '2')
    {

      //temp21 = sets - 1;
      printf("===== L1 contents =====\n");
      for(j=0;j<cache_blocks_number;j++)
      {
          printf("Set %d: ",j);
          //temp21 = sets - 1;
          for(i=0;i<sets;i++)
          {
            //for(k=0;k<sets;k++)
            //{
              //if(lru_counter[(k*cache_blocks_number) + j] == temp21)
              //{
                hex_equ = strtol(set_mapped_cache_tag_arrays[(i*cache_blocks_number) + j],NULL,2);
                printf("%lx ",hex_equ);
                if (set_mapped_dirty_bit[(i*cache_blocks_number) + j]==1) printf("D "); else printf(" ");
              //}
            //}
            //temp21 = temp21 - 1;
          }
          printf("\n");

      }
      if(is_victim_cache_enabled != 0)
      {
          printf("===== Victim Cache contents =====\n");
          for(j=0;j<victim_cache_blocks_number;j++)
          {
              printf("Set %d: ",j);
              for(i=0;i<victim_cache_sets;i++)
              {
                  hex_equ = strtol(set_mapped_victim_cache_tag_arrays[(i*victim_cache_blocks_number) + j],NULL,2);
                  printf("%lx ",hex_equ);
                  if (set_mapped_victim_dirty_bit[(i*victim_cache_blocks_number) + j]==1) printf("D "); else printf(" ");
              }
              printf("\n");

          }
      }
      if(l2_cache_size != 0)
      {
        //temp21 = l2_sets - 1;
        printf("===== L2 contents =====\n");
        for(j=0;j<l2_cache_blocks_number;j++)
        {
            printf("Set %d: ",j);
            //temp21 = l2_sets - 1;
            for(i=0;i<l2_sets;i++)
            {
              //for(k=0;k<l2_sets;k++)
              //{
                //if(l2_lru_counter[(k*l2_cache_blocks_number) + j] == temp21)
                //{
                  hex_equ = strtol(l2_set_mapped_cache_tag_arrays[(i*l2_cache_blocks_number) + j],NULL,2);
                  printf("%lx ",hex_equ);
                  if (l2_set_mapped_dirty_bit[(i*l2_cache_blocks_number) + j]==1) printf("D "); else printf(" ");
                //}
              //}
              //temp21 = temp21 - 1;
            }
            printf("\n");

        }
      }
    }

    //else if(rep_policy == '1')
    else if(rep_policy == '3')
    {

      temp21 = sets - 1;
      printf("===== L1 contents =====\n");
      for(j=0;j<cache_blocks_number;j++)
      {
          printf("Set %d: ",j);
          temp21 = sets - 1;
          for(i=0;i<sets;i++)
          {
            for(k=0;k<sets;k++)
            {
              if(lru_counter[(k*cache_blocks_number) + j] == temp21)
              {
                hex_equ = strtol(set_mapped_cache_tag_arrays[(k*cache_blocks_number) + j],NULL,2);
                printf("%lx ",hex_equ);
                if (set_mapped_dirty_bit[(k*cache_blocks_number) + j]==1) printf("D "); else printf(" ");
              }
            }
            temp21 = temp21 - 1;
          }
          printf("\n");

      }

      if(is_victim_cache_enabled != 0)
      {
          printf("===== Victim Cache contents =====\n");
          for(j=0;j<victim_cache_blocks_number;j++)
          {
              printf("Set %d: ",j);
              for(i=0;i<victim_cache_sets;i++)
              {
                  hex_equ = strtol(set_mapped_victim_cache_tag_arrays[(i*victim_cache_blocks_number) + j],NULL,2);
                  printf("%lx ",hex_equ);
                  if (set_mapped_victim_dirty_bit[(i*victim_cache_blocks_number) + j]==1) printf("D "); else printf(" ");
              }
              printf("\n");

          }
      }

      if(l2_cache_size != 0)
      {
        temp21 = l2_sets - 1;
        printf("===== L2 contents =====\n");
        for(j=0;j<l2_cache_blocks_number;j++)
        {
            printf("Set %d: ",j);
            temp21 = l2_sets - 1;
            for(i=0;i<l2_sets;i++)
            {
              for(k=0;k<l2_sets;k++)
              {
                if(l2_lru_counter[(k*l2_cache_blocks_number) + j] == temp21)
                {
                  hex_equ = strtol(l2_set_mapped_cache_tag_arrays[(k*l2_cache_blocks_number) + j],NULL,2);
                  printf("%lx ",hex_equ);
                  if (l2_set_mapped_dirty_bit[(k*l2_cache_blocks_number) + j]==1) printf("D "); else printf(" ");
                }
              }
              temp21 = temp21 - 1;
            }
            printf("\n");

        }
      }
    }

    free(set_mapped_cache_tag_arrays);
    free(set_mapped_cache_valid_arrays);
    free(lru_counter);
    free(lfu_counter);
    free(lfu_counter_set_age);
    free(set_mapped_dirty_bit);
    if(is_victim_cache_enabled != 0)
    {
        free(set_mapped_victim_cache_tag_arrays);
        free(set_mapped_victim_cache_valid_arrays);
        free(set_mapped_victim_dirty_bit);
        free(victim_cache_lru_counter);
    }
    if(l2_cache_size != 0)
    {
      free(l2_set_mapped_cache_tag_arrays);
      free(l2_lru_counter);
      free(l2_lfu_counter);
      free(l2_lfu_counter_set_age);
      free(l2_set_mapped_dirty_bit);
    }


    miss_rate = (((float)read_miss_count+write_miss_count)/(float)(read_miss_count+read_hit_count+write_hit_count+write_miss_count));
    l2_miss_rate = (((float)l2_read_miss_count+l2_write_miss_count)/(float)(l2_read_miss_count+l2_read_hit_count+l2_write_hit_count+l2_write_miss_count));

    l1_hit_time = 0.25 + (2.5*(total_cache_size/(512.0000*1024.0000))) + (0.025*(cache_block_size/16)) + (0.025*sets);
    l2_hit_time = 2.5 + (2.5*(l2_cache_size/(512.0000*1024.0000))) + (0.025*(cache_block_size/16)) + (0.025*l2_sets);
    //printf("hit time %f\n",l1_hit_time);

    l1_miss_penalty = 20 + (0.5*(cache_block_size/16));
    l2_miss_penalty = 20 + (0.5*(cache_block_size/16));

    //printf("miss penalty %f\n",l1_miss_penalty);

    //l1_total_access_time = ((read_miss_count+read_hit_count+write_hit_count+write_miss_count)*l1_hit_time) +((read_miss_count+write_miss_count)*l1_miss_penalty);
    if(l2_cache_size != 0)
    {
      l2_avg_access_time = l1_hit_time + (miss_rate*(l2_hit_time + (l2_miss_rate*l2_miss_penalty)));
      l2_miss_rate_mod = l2_miss_rate;
      final_traffic = l2_traffic;
    }
    else
    {
      l2_avg_access_time = l1_hit_time + (miss_rate*l1_miss_penalty);
      l2_miss_rate_mod = 0;
      final_traffic = traffic;
    }


    printf("\n====== Simulation results (raw) ======\n");
    printf("a. number of L1 reads:              %ld\n",read_miss_count+read_hit_count);
    printf("b. number of L1 read misses:        %ld\n",read_miss_count);
    printf("c. Number of L1 writes:             %ld\n",write_hit_count+write_miss_count);
    printf("d. number of L1 write misses:       %ld\n",write_miss_count);
    printf("e. L1 miss rate:                    %0.4f\n",miss_rate);
    printf("f. number of swaps:                 %d\n",l1_victim_swaps);
    printf("g. number of victim cache writeback:%ld\n",victim_cache_write_back);
    printf("h. number of L2 reads:              %d\n",l2_read_miss_count+l2_read_hit_count);
    printf("i. number of L2 read misses:        %d\n",l2_read_miss_count);
    printf("j. Number of L2 writes:             %d\n",l2_write_hit_count+l2_write_miss_count);
    printf("k. number of L2 write misses:       %d\n",l2_write_miss_count);
    if(l2_cache_size != 0)
    printf("l. L2 miss rate:                    %0.4f\n",l2_miss_rate_mod);
    else
    printf("l. L2 miss rate:                    0\n");
    printf("m. number of L2 writeback:          %ld\n",l2_write_back);
    printf("n. total memory traffic:            %ld\n",final_traffic);
	printf("\n");
    //L1 miss penalty (in ns) = 20 ns + 0.5*(L1_BLOCKSIZE / 16 B/ns))
    //L1 Cache Hit Time (in ns) = 0.25ns + 2.5ns * (L1_Cache Size / 512kB) + 0.025ns * (L1_BLOCKSIZE / 16B) + 0.025ns * L1_SET_ASSOCIATIVITY
    printf("==== Simulation results (performance) ====\n");
    printf("1. average access time:             %0.4f ns\n",l2_avg_access_time);
    //fflush(trace_file);
    //printf("Closing the trace file\n");

    fclose(trace_file);
    return 0;
}
