/* adRRlist.h - A generic Round Robin implementation of lists and numbers
 *
 * Copyright (c) 2006-2009, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <time.h>
#ifndef __ADRRLIST_H__
#define __ADRRLIST_H__

#include "adlist.h"
/* Node, List, and Iterator are the only data structures used currently. */


typedef struct listRRDNode {
    int periodvalue;
    list *value;
} listRRDNode;

typedef struct RRDlist {
    int period;
    int numperiods;
    void (*free)(void *ptr);
    listRRDNode **lists;
} RRDlist;

typedef struct listRRSNode {
    int periodvalue;
    long value;
    long count;
    long min;
    long max;
    // other  info here for variance etc
} listRRSNode;


typedef struct RRSlist {
    int period;
    int numperiods;
    void (*free)(void *ptr);
    listRRSNode **values;
} RRSlist;

/* Functions implemented as macros */

/* Prototypes */
RRSlist *listRRSCreate(int period, int numperiods);
RRSlist *listRRSAdd( RRSlist *mylist, long value, time_t timeperiod);
listRRSNode *listRRSGet( RRSlist *mylist, time_t timeperiod);

void listRRSRelease(RRSlist *list);

RRDlist *listRRDCreate(int period, int numperiods);
RRDlist *listRRDAdd( RRDlist *mylist, void *value, time_t timeperiod);
list *listRRDGet( RRDlist *mylist, time_t timeperiod);
void listRRDRelease(RRDlist *list);

/* Directions for iterators */

#endif /* __ADRRLIST_H__ */
