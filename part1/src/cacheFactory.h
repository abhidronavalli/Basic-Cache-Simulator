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
        a. cacheFactory.h   <--- active
        b. cacheConfig.h    
        c. cacheStats.h

About:

This file consists of cacheBlk structure and cacheFactory class which model the skeleton of the
simulator.

****************************************************************************************************/

#ifndef CACHEFACTORY_H
#define CACHEFACTORY_H

#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include "cacheConfig.h"
#include "cacheStats.h"

using namespace std;

// models a cache block and it's metadata
typedef struct cacheBlk
{
    string tag;
    unsigned int lru_count;
    unsigned int lfu_count;
    unsigned int age_count;
    cache_block_state_t state;

    cacheBlk() { clear(); }

    void clear()
    {
        tag.clear();
        lru_count = 0;
        lfu_count = 0;
        age_count = 0;
        state = INVALID;
    }

    void print_blk_info()
    {
        fprintf(stdout, "|Tag: %s|LRU: %d|LFU: %d|Age: %d|State: %d| ", tag.c_str(), lru_count, lfu_count, age_count, state);
    }

    void set_lru_count(unsigned int count) { lru_count = count; }
    void set_lfu_count(unsigned int count) { lfu_count = count; }
    void set_age_count(unsigned int count) { age_count = count; }
    void set_state(cache_block_state_t new_state) { state = new_state; }

    string get_tag() { return tag; }
    unsigned int get_lru_count() { return lru_count; }
    unsigned int get_lfu_count() { return lfu_count; }
    unsigned int get_age_count() { return age_count; }
    cache_block_state_t get_state() { return state; }

} cacheBlk_t;

// models the simulator
class cacheFactory
{
  public:
    cacheFactory(cacheConfig_t *config, stats_t *stats);
    ~cacheFactory();
    string hex2bin(string &cpu_address_hex);
    string hex2bin_char(char hex);
    unsigned int bin2dec(string &incoming_index_bin);
    void build(cacheConfig_t *config, int argc, char *argv[], fstream &trace, stats_t *stats);
    bool is_request_pending(fstream &trace);
    void get_request(cacheConfig_t *config, fstream &trace);
    cache_request_status_t request_status(cacheConfig_t *config, unsigned int &hit_index);
    void update_stats(cacheConfig_t *config, cache_request_status_t request_status, stats_t *stats);
    void print_stats(stats_t *stats);

    __wrap_iter<cacheBlk *> update_lru_count(cacheConfig_t *config, cache_request_status_t request_status, unsigned int &hit_index);
    __wrap_iter<cacheBlk *> update_lfu_count(cacheConfig_t *config, cache_request_status_t request_status, unsigned int &hit_index);
    void update_tag(__wrap_iter<cacheBlk *> it, string tag);
    void update_cache_blk(cacheConfig_t *config, cache_request_status_t request_status, unsigned int &hit_index);
    void wrap_up(cacheConfig_t *config, stats_t *stats, fstream &trace);

    cache_request_type_t get_request_type()
    {
        if (req_type == 'r')
            return READ;
        else
            return WRITE;
    }

  private:
    fstream trace;

    vector<cacheBlk_t> cache;
    vector<cacheBlk_t>::iterator it, max_it, min_it;
    char req_type;
    string cpu_address_hex;
    string cpu_address_bin;
    string incoming_tag;
    string incoming_index_bin;
    unsigned int incoming_index_dec;

    bool blk_was_dirty;

    cacheConfig_t *config_ptr;
    stats_t *stats_ptr;
};

#endif