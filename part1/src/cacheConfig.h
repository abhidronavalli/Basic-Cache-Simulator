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
        c. cacheConfig.cc   
        d. cacheStats.cc    

    2. Header Files
        a. cacheFactory.h
        b. cacheConfig.h    <--- active
        c. cacheStats.h

About:

This file consists of config class useful to initate the configuration of the cache as per the command
line inputs along with few useful enum states for easy understanding of the code.

****************************************************************************************************/

#ifndef CACHECONFIG_H
#define CACHECONFIG_H

#include <fstream>

using namespace std;

typedef enum
{
    INVALID,
    VALID_CLEAN,
    VALID_DIRTY
} cache_block_state_t;

typedef enum
{
    MISS,
    HIT
} cache_request_status_t;

typedef enum
{
    READ,
    WRITE
} cache_request_type_t;

typedef enum
{
    WRITE_BACK,
    WRITE_THROUGH
} write_policy_t;

typedef enum
{
    WRITE_ALLOCATE,
    WRITE_NOT_ALLOCATE
} allocation_policy_t;

typedef enum
{
    LRU,
    LFU,
    LRFU
} replacement_policy_t;

// contains method to initiate a cache from the trace file
typedef class cacheConfig
{
  public:
    cacheConfig();
    ~cacheConfig();
    void init(int argc, char *argv[], fstream &trace);
    void errorCheck(int argc, char *argv[]);
    void set_blk_size(unsigned int blk_size);
    void set_cache_size(unsigned int cache_size);
    void set_assoc(unsigned int assoc);
    void set_cache_lines(unsigned int blk_size, unsigned int cache_size, unsigned int assoc) { cache_lines = cache_size / (assoc * blk_size); }
    void set_r_policy(replacement_policy_t policy) { rep_policy = policy; }
    void set_w_policy(write_policy_t policy) { w_policy = policy; }
    void set_a_policy(allocation_policy_t policy) { a_policy = policy; }

    unsigned int get_blk_size() { return blk_size; }
    unsigned int get_cache_size() { return cache_size; }
    unsigned int get_assoc() { return assoc; }
    unsigned int get_cache_lines() { return cache_lines; }
    unsigned int get_blk_offset_size() { return blk_offset_size; }
    unsigned int get_index_size() { return index_size; }
    unsigned int get_tag_size() { return tag_size; }
    replacement_policy_t get_r_policy() { return rep_policy; }
    write_policy_t get_w_policy() { return w_policy; }
    allocation_policy_t get_a_policy() { return a_policy; }

    void print_config();

  private:
    fstream trace;

    unsigned int blk_size;
    unsigned int cache_size;
    unsigned int assoc;

    unsigned int cache_lines;

    unsigned int blk_offset_size;
    unsigned int index_size;
    unsigned int tag_size;

    replacement_policy_t rep_policy;
    write_policy_t w_policy;
    allocation_policy_t a_policy;

    char *trace_name;

} cacheConfig_t;

#endif