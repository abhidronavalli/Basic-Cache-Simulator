/*************************************************************************************************

Author: Abhiram Dronavalli
Last Modified: Jun 13, 2018
Last Status: Frozen
Version: 3.0
Project: ECE563: L1 Cache Simulator - Part 1
Files:

    1. Source Files
		a. main.cc          				
		b. cacheFactory.cc  <--- active
        c. cacheConfig.cc   
        d. cacheStats.cc    

    2. Header Files
        a. cacheFactory.h
        b. cacheConfig.h
        c. cacheStats.h

About:

The most important file of the entire project. This file has methods which control the working of 
the simulator

****************************************************************************************************/
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include "cacheFactory.h"
#include "cacheConfig.h"
#include "cacheStats.h"

using namespace std;

cacheFactory::cacheFactory(cacheConfig_t *config, stats_t *stats)
{
    cache.clear();
    cpu_address_hex.clear();
    cpu_address_bin.clear();
    incoming_tag.clear();
    incoming_index_bin.clear();
    incoming_index_dec = 0;
    blk_was_dirty = false;
    config_ptr = config;
    stats_ptr = stats;
}

cacheFactory::~cacheFactory()
{
    config_ptr->~cacheConfig();
    config_ptr = NULL;
}

string cacheFactory::hex2bin(string &cpu_address_hex)
{
    cpu_address_bin.clear();
    while (cpu_address_hex.length() != 8)
        cpu_address_hex.insert(cpu_address_hex.begin(), '0');
    for (unsigned int i = 0; i != cpu_address_hex.length(); ++i)
        cpu_address_bin += this->hex2bin_char(cpu_address_hex[i]);

    return cpu_address_bin;
}

string cacheFactory::hex2bin_char(char hex)
{
    switch (toupper(hex))
    {
    case '0':
        return "0000";
    case '1':
        return "0001";
    case '2':
        return "0010";
    case '3':
        return "0011";
    case '4':
        return "0100";
    case '5':
        return "0101";
    case '6':
        return "0110";
    case '7':
        return "0111";
    case '8':
        return "1000";
    case '9':
        return "1001";
    case 'A':
        return "1010";
    case 'B':
        return "1011";
    case 'C':
        return "1100";
    case 'D':
        return "1101";
    case 'E':
        return "1110";
    case 'F':
        return "1111";
    }
    fprintf(stderr, "ERROR: Incorrect hexadecimal value found!\n");
    abort();
}

unsigned int cacheFactory::bin2dec(string &incoming_index_bin)
{
    unsigned int base = 1;
    incoming_index_dec = 0;
    while (incoming_index_bin.length() != 0)
    {
        incoming_index_dec = incoming_index_dec + base * (atoi(&incoming_index_bin.back()) % 10);
        incoming_index_bin.pop_back();
        base = base * 2;
    }
    return incoming_index_dec;
}

void cacheFactory::build(cacheConfig_t *config, int argc, char *argv[], fstream &trace, stats_t *stats)
{
    config->init(argc, argv, trace);
    config->print_config();
    //https://stackoverflow.com/questions/7397768/choice-between-vectorresize-and-vectorreserve
    cache.resize((config->get_cache_lines()) * (config->get_assoc()));
    //initialise LRU count in decreasing order for each cache line
    //initialse LFU count of all sets of a cache line to zero
    for (unsigned int i = 0; i < config->get_cache_lines(); ++i)
    {
        for (unsigned int j = 0; j < config->get_assoc(); ++j)
        {
            cache.push_back(cacheBlk());
            if (config->get_r_policy() == LRU)
                cache[(i * config->get_assoc()) + j].set_lru_count(config->get_assoc() - j - 1);
            else if (config->get_r_policy() == LFU)
                cache[(i * config->get_assoc()) + j].set_lfu_count(0);
            cache[(i * config->get_assoc()) + j].set_state(INVALID);
        }
    }
    stats->set_hit_time(config->get_blk_size(), config->get_cache_size(), config->get_assoc());
    stats->set_miss_penalty(config->get_blk_size());
}

bool cacheFactory::is_request_pending(fstream &trace)
{
    bool req = false;
    char c;
    if (trace.is_open() && trace.get(c))
    {
        if (!trace.eof())
            req = true;
        trace.seekg(-1, trace.cur);
    }
    return req;
}

void cacheFactory::get_request(cacheConfig_t *config, fstream &trace)
{
    trace.get(req_type);
    trace.seekg(1, trace.cur);
    if (!(req_type == 'r' || req_type == 'w'))
    {
        fprintf(stderr, "ERROR: Incorrect request type\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "r: READ\n");
        fprintf(stderr, "w: WRITE\n");
        abort();
    }
    getline(trace, cpu_address_hex, '\n');
    if (cpu_address_hex.length() > 8)
    {
        fprintf(stderr, "ERROR: Simulator is 32bit compatible only!");
        abort();
    }
    cpu_address_bin = this->hex2bin(cpu_address_hex);
    incoming_tag.assign(cpu_address_bin, 0, config->get_tag_size());
    incoming_index_bin.assign(cpu_address_bin, config->get_tag_size(), config->get_index_size());
    incoming_index_dec = this->bin2dec(incoming_index_bin);
}

cache_request_status_t cacheFactory::request_status(cacheConfig_t *config, unsigned int &index)
{
    for (unsigned int j = 0; j < config->get_assoc(); ++j)
    {
        if (cache[(incoming_index_dec * config->get_assoc()) + j].tag.compare(incoming_tag) == 0 &&
            cache[(incoming_index_dec * config->get_assoc()) + j].state != INVALID)
        {
            index = j;
            return HIT;
        }
    }
    return MISS;
}

bool lru_comp(const cacheBlk_t &lhs, const cacheBlk_t &rhs)
{
    return lhs.lru_count < rhs.lru_count;
}

__wrap_iter<cacheBlk *> cacheFactory::update_lru_count(cacheConfig_t *config, cache_request_status_t request_status, unsigned int &hit_index)
{
    //returns a cache blk in case of miss only, in case of hit returns first cache blk(redundant)
    //does not use hit_index in case of miss, only used for hit scenario
    if (request_status == HIT)
    {
        //increment the lru count of all sets with count value less than the lru count of the hit blk and then set the hit blk's count to zero
        unsigned int old_lru_count = cache[(incoming_index_dec * config->get_assoc()) + hit_index].get_lru_count();
        for (it = cache.begin() + (incoming_index_dec * config->get_assoc()); it != cache.begin() + ((incoming_index_dec + 1) * config->get_assoc()); ++it)
        {
            if (it->get_lru_count() < old_lru_count)
                it->set_lru_count(it->get_lru_count() + 1);
        }
        cache[(incoming_index_dec * config->get_assoc()) + hit_index].set_lru_count(0);
        return cache.begin();
    }
    else if (request_status == MISS)
    {
        //find largest lru cache blk, set it to zero, inc others by one and return the set which was set to zero
        max_it = max_element(cache.begin() + (incoming_index_dec * config->get_assoc()), cache.begin() + ((incoming_index_dec + 1) * config->get_assoc()), lru_comp);
        fprintf(stdout, "Max LRU count is %d of tag %s\n", max_it->get_lru_count(), max_it->get_tag().c_str());
        max_it->set_lru_count(0);
        for (it = cache.begin() + (incoming_index_dec * config->get_assoc()); it != cache.begin() + ((incoming_index_dec + 1) * config->get_assoc()); ++it)
        {
            if (it != max_it)
                it->set_lru_count(it->get_lru_count() + 1);
        }
        return max_it;
    }
    else
    {
        fprintf(stderr, "ERROR: Incorrect cache request passed to update_lru_count function\n");
        abort();
    }
}

bool lfu_comp(const cacheBlk_t &lhs, const cacheBlk_t &rhs)
{
    return lhs.lfu_count < rhs.lfu_count;
}

__wrap_iter<cacheBlk *> cacheFactory::update_lfu_count(cacheConfig_t *config, cache_request_status_t request_status, unsigned int &hit_index)
{
    //returns a cache blk in case of miss only, in case of hit returns first cache blk(redundant)
    //does not use hit_index in case of miss, only used for hit scenario
    if (request_status == HIT)
    {
        //increment the lfu count of the hit blk
        unsigned int old_lfu_count = cache[(incoming_index_dec * config->get_assoc()) + hit_index].get_lfu_count();
        cache[(incoming_index_dec * config->get_assoc()) + hit_index].set_lfu_count(old_lfu_count + 1);
        return cache.begin();
    }
    else if (request_status == MISS)
    {
        //find the blk with lowest lfu count and set it's count to age_count+1(LFU with "Dynamic Aging")
        min_it = min_element(cache.begin() + (incoming_index_dec * config->get_assoc()), cache.begin() + ((incoming_index_dec + 1) * config->get_assoc()), lfu_comp);
        min_it->set_age_count(min_it->get_lfu_count());
        min_it->set_lfu_count(min_it->get_age_count() + 1);

        return min_it;
    }
    else
    {
        fprintf(stderr, "ERROR: Incorrect cache request passed to update_lfu_count function\n");
        abort();
    }
}

void cacheFactory::update_tag(__wrap_iter<cacheBlk *> it, string tag)
{
    it->tag.resize(tag.size());
    it->tag = tag;
    if (it->get_state() == INVALID)
    {
        it->set_state(VALID_CLEAN);
    }
}

void cacheFactory::update_cache_blk(cacheConfig_t *config, cache_request_status_t request_status, unsigned int &hit_index)
{
    blk_was_dirty = false;
    if (config->get_r_policy() == LRU && config->get_w_policy() == WRITE_BACK && config->get_a_policy() == WRITE_ALLOCATE)
    {
        if (request_status == HIT)
        {
            this->update_lru_count(config, HIT, hit_index);
            if (this->get_request_type() == WRITE)
                cache[(incoming_index_dec * config->get_assoc()) + hit_index].state = VALID_DIRTY;
        }
        else if (request_status == MISS)
        {
            it = this->update_lru_count(config, MISS, hit_index);
            this->update_tag(it, incoming_tag);
            if (it->get_state() == VALID_DIRTY)
            {
                it->set_state(VALID_CLEAN);
                blk_was_dirty = true;
            }
            if (this->get_request_type() == WRITE)
                it->set_state(VALID_DIRTY);
        }
    }
    else if (config->get_r_policy() == LRU && config->get_w_policy() == WRITE_THROUGH && config->get_a_policy() == WRITE_NOT_ALLOCATE)
    {
        if (request_status == HIT)
        {
            this->update_lru_count(config, HIT, hit_index);
        }
        else if (request_status == MISS)
        {
            if (this->get_request_type() == READ)
            {
                it = this->update_lru_count(config, MISS, hit_index);
                this->update_tag(it, incoming_tag);
            }
        }
    }
    else if (config->get_r_policy() == LFU && config->get_w_policy() == WRITE_BACK && config->get_a_policy() == WRITE_ALLOCATE)
    {
        if (request_status == HIT)
        {
            this->update_lfu_count(config, HIT, hit_index);
            if (this->get_request_type() == WRITE)
                cache[(incoming_index_dec * config->get_assoc()) + hit_index].state = VALID_DIRTY;
        }
        else if (request_status == MISS)
        {
            it = this->update_lfu_count(config, MISS, hit_index);
            this->update_tag(it, incoming_tag);
            if (it->get_state() == VALID_DIRTY)
            {
                it->set_state(VALID_CLEAN);
                blk_was_dirty = true;
            }
            if (this->get_request_type() == WRITE)
                it->set_state(VALID_DIRTY);
        }
    }
    else if (config->get_r_policy() == LFU && config->get_w_policy() == WRITE_THROUGH && config->get_a_policy() == WRITE_NOT_ALLOCATE)
    {
        if (request_status == HIT)
        {
            this->update_lfu_count(config, HIT, hit_index);
        }
        else if (request_status == MISS)
        {
            if (this->get_request_type() == READ)
            {
                it = this->update_lfu_count(config, MISS, hit_index);
                this->update_tag(it, incoming_tag);
            }
        }
    }
    else
    {
        fprintf(stderr, "ERROR: Invalid simulator configuration!\n");
        abort();
    }
}

void cacheFactory::update_stats(cacheConfig_t *config, cache_request_status_t request_status, stats_t *stats)
{
    if (config->get_r_policy() == LRU && config->get_w_policy() == WRITE_BACK && config->get_a_policy() == WRITE_ALLOCATE)
    {
        if (request_status == HIT && this->get_request_type() == READ)
            stats->inc_r_hit_count();
        else if (request_status == HIT && this->get_request_type() == WRITE)
            stats->inc_w_hit_count();

        if (request_status == MISS && blk_was_dirty == true)
        {
            stats->inc_w_bk_count();
            stats->inc_traffic_count();
            blk_was_dirty = false;
        }

        if (request_status == MISS && this->get_request_type() == READ)
        {
            stats->inc_r_miss_count();
            stats->inc_traffic_count();
        }
        else if (request_status == MISS && this->get_request_type() == WRITE)
        {
            stats->inc_w_miss_count();
            stats->inc_traffic_count();
        }
        stats->set_miss_rate();
    }
    else if (config->get_r_policy() == LRU && config->get_w_policy() == WRITE_THROUGH && config->get_a_policy() == WRITE_NOT_ALLOCATE)
    {
        if (request_status == HIT && this->get_request_type() == READ)
            stats->inc_r_hit_count();
        else if (request_status == HIT && this->get_request_type() == WRITE)
        {
            stats->inc_w_hit_count();
            stats->inc_traffic_count();
        }
        if (request_status == MISS && this->get_request_type() == READ)
        {
            stats->inc_r_miss_count();
            stats->inc_traffic_count();
        }
        else if (request_status == MISS && this->get_request_type() == WRITE)
        {
            stats->inc_w_miss_count();
            stats->inc_traffic_count();
        }
    }
    else if (config->get_r_policy() == LFU && config->get_w_policy() == WRITE_BACK && config->get_a_policy() == WRITE_ALLOCATE)
    {
        if (request_status == HIT && this->get_request_type() == READ)
            stats->inc_r_hit_count();
        else if (request_status == HIT && this->get_request_type() == WRITE)
            stats->inc_w_hit_count();

        if (request_status == MISS && blk_was_dirty == true)
        {
            stats->inc_w_bk_count();
            stats->inc_traffic_count();
            blk_was_dirty = false;
        }

        if (request_status == MISS && this->get_request_type() == READ)
        {
            stats->inc_r_miss_count();
            stats->inc_traffic_count();
        }
        else if (request_status == MISS && this->get_request_type() == WRITE)
        {
            stats->inc_w_miss_count();
            stats->inc_traffic_count();
        }
    }
    else if (config->get_r_policy() == LFU && config->get_w_policy() == WRITE_THROUGH && config->get_a_policy() == WRITE_NOT_ALLOCATE)
    {
        if (request_status == HIT && this->get_request_type() == READ)
            stats->inc_r_hit_count();
        else if (request_status == HIT && this->get_request_type() == WRITE)
        {
            stats->inc_w_hit_count();
            stats->inc_traffic_count();
        }
        if (request_status == MISS && this->get_request_type() == READ)
        {
            stats->inc_r_miss_count();
            stats->inc_traffic_count();
        }
        else if (request_status == MISS && this->get_request_type() == WRITE)
        {
            stats->inc_w_miss_count();
            stats->inc_traffic_count();
        }
    }
    else
    {
        fprintf(stderr, "ERROR: Invalid simulator configuration!\n");
        abort();
    }
    stats->set_miss_rate();
}

void cacheFactory::wrap_up(cacheConfig_t *config, stats_t *stats, fstream &trace)
{
    fprintf(stdout, "===== L1 contents =====\n");
    for (unsigned int i = 0; i < config->get_cache_lines(); ++i)
    {
        fprintf(stdout, "Set %d: ", i);
        for (unsigned int j = 0; j < config->get_assoc(); ++j)
        {
            if (cache[(i * config->get_assoc()) + j].get_state() != INVALID)
            {
                fprintf(stdout, "%8x ", stoi(cache[(i * config->get_assoc()) + j].get_tag(), nullptr, 2));
                if (cache[(i * config->get_assoc()) + j].get_state() == VALID_DIRTY)
                    fprintf(stdout, "D ");
            }
            else
                fprintf(stdout, "- ");
        }
        fprintf(stdout, "\n");
    }
    stats->set_aat();
    trace.close();
}

void cacheFactory::print_stats(stats_t *stats)
{
    fprintf(stdout, "\n====== Simulation results (raw) ======\n");
    fprintf(stdout, "a. number of L1 reads:             %d\n", stats->get_r_miss_count() + stats->get_r_hit_count());
    fprintf(stdout, "b. number of L1 read misses:       %d\n", stats->get_r_miss_count());
    fprintf(stdout, "c. Number of L1 writes:            %d\n", stats->get_w_miss_count() + stats->get_w_hit_count());
    fprintf(stdout, "d. number of L1 write misses:      %d\n", stats->get_w_miss_count());
    fprintf(stdout, "e. L1 miss rate:                   %0.4f\n", stats->get_miss_rate());
    fprintf(stdout, "f. number of writebacks from L1:   %d\n", stats->get_w_bk_count());
    fprintf(stdout, "g. total memory traffic:           %d\n", stats->get_traffic_count());
    fprintf(stdout, "\n");
    fprintf(stdout, "==== Simulation results (performance) ====\n");
    fprintf(stdout, "1. average access time:            %0.4f ns\n", stats->get_aat());
}