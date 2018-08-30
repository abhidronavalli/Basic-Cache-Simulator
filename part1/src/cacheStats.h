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
        b. cacheConfig.h    
        c. cacheStats.h     <--- active

About:

This file consists of statistics class useful the model the stat of the simulator.

****************************************************************************************************/

#ifndef STATISTICS_H
#define STATISTICS_H

// contains methods for stat collection
typedef class statistics
{
  public:
    statistics();
    statistics(float hit_time, float miss_penalty);

    void inc_r_hit_count() { r_hit_count++; }
    void inc_r_miss_count() { r_miss_count++; }
    void inc_w_hit_count() { w_hit_count++; }
    void inc_w_miss_count() { w_miss_count++; }
    void inc_w_bk_count() { w_bk_count++; }
    void inc_traffic_count() { traffic_count++; }

    void set_hit_time(unsigned int blk_size, unsigned int cache_size, unsigned int assoc)
    {
        hit_time = hit_time_offset + (2.5 * (cache_size / (512.0000 * 1024.0000))) + (0.025 * (blk_size / 16)) + (0.025 * assoc);
    }
    void set_miss_penalty(unsigned int blk_size) { miss_penalty = miss_penalty_offset + (0.5 * (blk_size / 16)); }
    void set_miss_rate() { miss_rate = (((float)r_miss_count + w_miss_count) / (float)(r_miss_count + r_hit_count + w_hit_count + w_miss_count)); }
    void set_aat() { avg_access_time = hit_time + (miss_rate * miss_penalty); }

    unsigned int get_r_hit_count() { return r_hit_count; }
    unsigned int get_r_miss_count() { return r_miss_count; }
    unsigned int get_w_hit_count() { return w_hit_count; }
    unsigned int get_w_miss_count() { return w_miss_count; }
    unsigned int get_w_bk_count() { return w_bk_count; }
    unsigned int get_traffic_count() { return traffic_count; }
    float get_miss_rate() { return miss_rate; }
    float get_aat() { return avg_access_time; }

  private:
    unsigned int r_hit_count;
    unsigned int r_miss_count;

    unsigned int w_hit_count;
    unsigned int w_miss_count;

    unsigned int w_bk_count;
    unsigned int traffic_count;

    float miss_rate;
    float hit_time;
    float hit_time_offset;
    float miss_penalty;
    float miss_penalty_offset;
    float avg_access_time;

} stats_t;

#endif