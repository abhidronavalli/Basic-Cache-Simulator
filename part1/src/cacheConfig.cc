/*************************************************************************************************

Author: Abhiram Dronavalli
Last Modified: Jun 13, 2018
Last Status: Frozen
Version: 3.0
Project: ECE563: L1 Cache Simulator - Part 1
Files:

    1. Source Files
		a. main.cc          				
		b. cacheFactory.cc  
        c. cacheConfig.cc   <--- active
        d. cacheStats.cc    

    2. Header Files
        a. cacheFactory.h
        b. cacheConfig.h
        c. cacheStats.h

About:

This file consists of methods useful to initate the configuration of the cache as per the command
line inputs.

****************************************************************************************************/

#include <math.h>
#include "cacheConfig.h"

cacheConfig::cacheConfig()
{
    blk_size = 0;
    cache_size = 0;
    assoc = 0;
    cache_lines = 0;
    rep_policy = LRU;
    w_policy = WRITE_BACK;
    a_policy = WRITE_ALLOCATE;
}

cacheConfig::~cacheConfig()
{
    delete trace_name;
}

void cacheConfig::errorCheck(int argc, char *argv[])
{
    if (argc != 7)
    {
        fprintf(stderr, "sim_cache <L1_BLOCKSIZE> <L1_SIZE> <L1_ASSOC> <L1_REPLACEMENT_POLICY> <L1_WRITE_POLICY> <trace_file>\n");
        fprintf(stderr, "ERROR: Invalid input parameters");
        abort();
    }
    else
    {
        //checking if the provided block size is power of 2
        if ((log2(atoi(argv[1])) != floor(log2(atoi(argv[1])))))
        {
            fprintf(stderr, "ERROR: Please enter valid block size!");
            abort();
        }

        //checking if the provided cache size is power of 2
        if ((log2(atoi(argv[2])) != floor(log2(atoi(argv[2])))))
        {
            fprintf(stderr, "ERROR: Please enter valid cache size!");
            abort();
        }

        //checking if valid set associativity
        if (atoi(argv[3]) > (atoi(argv[2]) / atoi(argv[1])) || atoi(argv[3]) == 0)
        {
            fprintf(stderr, "ERROR: Please enter valid set associativity!");
            abort();
        }

        //checking if valid replacement policy option
        if (atoi(argv[4]) > 1)
        {
            fprintf(stderr, "Options:\n");
            fprintf(stderr, "0:LRU\n");
            fprintf(stderr, "1:LFU\n");
            fprintf(stderr, "ERROR: Please enter valid replacement option!");
            abort();
        }

        //checking if valid write policy option
        if (atoi(argv[5]) > 1)
        {
            fprintf(stderr, "Options:\n");
            fprintf(stderr, "0:WBWA\n");
            fprintf(stderr, "1:WTNA\n");
            fprintf(stderr, "ERROR: Please enter valid write option!");
            abort();
        }

        // checking for valid trace file
        if (argv[6] == NULL)
        {
            fprintf(stderr, "ERROR: Invalid file name (or) File not present in the directory!");
            abort();
        }
    }
}

void cacheConfig::init(int argc, char *argv[], fstream &trace)
{
    this->errorCheck(argc, argv);
    blk_size = atoi(argv[1]);
    cache_size = atoi(argv[2]);
    assoc = atoi(argv[3]);
    switch (atoi(argv[4]))
    {
    case 0:
    {
        this->set_r_policy(LRU);
        break;
    }
    case 1:
    {
        this->set_r_policy(LFU);
        break;
    }
    }

    switch (atoi(argv[5]))
    {
    case 0:
    {
        this->set_w_policy(WRITE_BACK);
        this->set_a_policy(WRITE_ALLOCATE);
        break;
    }
    case 1:
    {
        this->set_w_policy(WRITE_THROUGH);
        this->set_a_policy(WRITE_NOT_ALLOCATE);
        break;
    }
    }
    trace_name = new char[strlen(argv[6]) + 1];
    strcpy(trace_name, argv[6]);
    trace.open(argv[6]);

    this->set_cache_lines(blk_size, cache_size, assoc);

    blk_offset_size = floor(log2(this->get_blk_size()));
    index_size = floor(log2(this->get_cache_lines()));
    tag_size = 32 - (index_size + blk_offset_size);
}

void cacheConfig::print_config()
{
    fprintf(stdout, "===== Simulator configuration =====\n");
    fprintf(stdout, "L1_BLOCKSIZE:                    %d\n", blk_size);
    fprintf(stdout, "L1_SIZE:                         %d\n", cache_size);
    fprintf(stdout, "L1_ASSOC:                        %d\n", assoc);
    fprintf(stdout, "L1_REPLACEMENT_POLICY:           %d\n", rep_policy);
    fprintf(stdout, "L1_WRITE_POLICY:                 %d\n", w_policy);
    fprintf(stdout, "trace_file:                      %s\n", trace_name);
    fprintf(stdout, "===================================\n");
    fprintf(stdout, "\n");
}