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
        d. cacheStats.cc    <--- active

    2. Header Files
        a. cacheFactory.h
        b. cacheConfig.h
        c. cacheStats.h

About:

This file consists of methods useful to manipulate statistics collected during the simulation.

****************************************************************************************************/

#include "cacheStats.h"

statistics::statistics()
{
    r_hit_count = 0;
    r_miss_count = 0;
    w_hit_count = 0;
    w_miss_count = 0;
    w_bk_count = 0;
    traffic_count = 0;
    miss_rate = 0.00;
    hit_time = 0.00;
    hit_time_offset = 0.00;
    miss_penalty = 0.00;
    miss_penalty_offset = 0.00;
    avg_access_time = 0.00;
}

statistics::statistics(float ht_time_offset, float miss_pen_offset)
{
    r_hit_count = 0;
    r_miss_count = 0;
    w_hit_count = 0;
    w_miss_count = 0;
    w_bk_count = 0;
    traffic_count = 0;
    miss_rate = 0.00;
    hit_time = 0.00;
    hit_time_offset = ht_time_offset;
    miss_penalty = 0.00;
    miss_penalty_offset = miss_pen_offset;
    avg_access_time = 0.00;
}