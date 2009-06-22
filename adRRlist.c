/* adRRlist.c - A generic doubly linked list implementation
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


#include <stdlib.h>
#include "adlist.h"
#include "adRRlist.h"
#include "zmalloc.h"

/* Create a new list. The created list can be freed with
 * AlFreeList(), but private value of every node need to be freed
 * by the user before to call AlFreeList().
 *
 * On error, NULL is returned. Otherwise the pointer to the new list. */
RRSlist *listRRSCreate(int period, int numperiods)
{
    RRSlist *list;
    listRRSNode *nodes;

    if ((list = zmalloc(sizeof(*list))) == NULL)
        return NULL;

    list->values = zmalloc(sizeof(nodes)*numperiods );
    if (list->values == NULL ) {
        zfree(list);
        return NULL;
    }

//    nodes = list->values;
    list->period= period;
    list->numperiods = numperiods;
    for (int i=0; i < numperiods; i++ ) {
        list->values[i] = zmalloc ( sizeof(listRRSNode ));
        list->values[i]->value=0;
        list->values[i]->periodvalue=0;
        list->values[i]->count=0;
        list->values[i]->min=0;
        list->values[i]->max=-1;
    }

    return list;
}

RRDlist *listRRDCreate(int period, int numperiods)
{
    struct RRDlist *list;
    struct listRRDNode **nodes;

    if ((list = zmalloc(sizeof(*list))) == NULL)
        return NULL;
    if ((list->lists = zmalloc(sizeof(nodes)*numperiods )) == NULL) {
        zfree(list);
        return NULL;
    }

    list->period= period;
    list->numperiods = numperiods;

//    list->lists= nodes;
    for (int i=0; i < numperiods; i++ ) {
        list->lists[i] = zmalloc( sizeof(listRRDNode));
        list->lists[i]->periodvalue=0;
        // we only create the list when we need it.
        list->lists[i]->value =NULL;
    }
    return list;
}

void listRRSRelease(RRSlist *list)
{

    for (int i=0; i< list->numperiods ; i++) {
        zfree( list->values[i] );
    }
    zfree( list->values );
    zfree( list);
}
void listRRDRelease(RRDlist *list) 
{
    for (int i=0; i < list->numperiods; i++ ) {
        if ( list->lists[i]->value != NULL ) {
            listRelease( list->lists[i]->value );
            zfree(list->lists[i]);
        }
    }
    zfree( list->lists);
    zfree( list);
}

RRSlist *listRRSAdd( RRSlist* list, long value,  time_t timeperiod ) 
{
    int periodnum;
    listRRSNode *thisone;
    if ( list == NULL ) {
        return NULL;
    }
    periodnum = ( timeperiod / list->period ) ;
    thisone = list->values[ periodnum % list->numperiods ];
    if (thisone->periodvalue == periodnum ) {
        thisone->value += value;
        thisone->count++;
        if ( thisone->min > value ) {
            thisone->min = value;
        }
        if ( thisone->max < value ) {
            thisone->max = value;
        }
    } else {
        thisone->count=1L;
        thisone->value=value;
        thisone->min=value;
        thisone->max=value;
        thisone->periodvalue=periodnum;
    }

    return list;
}

RRDlist *listRRDAdd( RRDlist* thislist, void* value,  time_t timeperiod ) 
{
    int periodnum;
    int val;
    //listRRDNode *thisone;
    list *oldList;
    if ( thislist == NULL ) {
        return NULL;
    }
    periodnum = ( timeperiod / thislist->period ) ;
    val = periodnum % thislist->numperiods;
//    thisone = thislist->lists[ val ];
    if (thislist->lists[val]->value == NULL || thislist->lists[val]->periodvalue != periodnum ) {
        oldList = thislist->lists[val]->value;
        thislist->lists[val]->value = listCreate();
        thislist->lists[val]->periodvalue = periodnum;
        if (oldList != NULL ) {
            listRelease( oldList );
        }
    }
    if (!listAddNodeTail(thislist->lists[val]->value,value)) return NULL;

    return thislist;
}

list *listRRDGet( RRDlist* thislist, time_t timeperiod ) 
{
    int periodnum;
    listRRDNode *thisone;
    
    if ( thislist == NULL ) {
        return NULL;
    }
    periodnum = ( timeperiod / thislist->period ) ;
    thisone = thislist->lists[ periodnum % thislist->numperiods ];
    if (thisone->value == NULL || thisone->periodvalue != periodnum ) {
        return NULL;
    }

    return thisone->value;
}

listRRSNode *listRRSGet( RRSlist* thislist, time_t timeperiod ) 
{
    int periodnum;
    listRRSNode *thisone;

    if ( thislist == NULL ) {
        return NULL;
    }
    periodnum = ( timeperiod / thislist->period ) ;
    thisone = (thislist->values[ periodnum % thislist->numperiods ]);
    if (thisone->count == 0 || thisone->periodvalue != periodnum ) {
        return NULL;
    }

    return thisone;
}
