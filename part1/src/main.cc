/*************************************************************************************************

Author: Abhiram Dronavalli
Last Modified: Jun 13, 2018
Last Status: Frozen
Version: 3.0
Project: ECE563: L1 Cache Simulator - Part 1
Files:

    1. Source Files
		a. main.cc          <--- active
		b. cacheFactory.cc
        c. cacheConfig.cc
        d. cacheStats.cc

    2. Header Files
        a. cacheFactory.h
        b. cacheConfig.h
        c. cacheStats.h

About:

This wrapper file for the cache simulator instantiates classes from the header files. This file
consists of basic functionality of the simulator.

****************************************************************************************************/

#define _GNU_SOURCE
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

int main(int argc, char *argv[])
{
    cacheConfig_t *l1_config = new cacheConfig();
    stats_t *l1_stats = new statistics(0.25, 20);
    fstream trace;
    unsigned int hit_at_index;
    cacheFactory l1_cache(l1_config, l1_stats);

    l1_cache.build(l1_config, argc, argv, trace, l1_stats);

    while (l1_cache.is_request_pending(trace))
    {
        l1_cache.get_request(l1_config, trace);
        if (l1_cache.request_status(l1_config, hit_at_index) == HIT)
        {
            l1_cache.update_cache_blk(l1_config, HIT, hit_at_index);
            l1_cache.update_stats(l1_config, HIT, l1_stats);
        }
        else if (l1_cache.request_status(l1_config, hit_at_index) == MISS)
        {
            l1_cache.update_cache_blk(l1_config, MISS, hit_at_index);
            l1_cache.update_stats(l1_config, MISS, l1_stats);
        }
    }
    l1_cache.wrap_up(l1_config, l1_stats, trace);
    l1_cache.print_stats(l1_stats);

    return 0;
}