/************************************************************************************
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ************************************************************************************/
#include <stdio.h>
#include <string.h>

#include "sealc/cluster.h"
#include "dpp/wsclient.h"

void cluster_log(cluster *c, char *msg)
{
    printf(": %s\n", msg);
}

void cluster_set_events(cluster *c, int event_count, ...) {

}

void cluster_start(cluster *c, int event_count, ... )
{
    if(c->shard_count == 0) {
        c->shard_count = 1;
    };
    for (size_t i = 0; i < c->shard_count; i++)
    {
        char *s = "Starting shard..";
        //char snum[5];
        //itoa(i, snum, 10);    

        //strcat(s, i);
        cluster_log(c, s);
    }

    while (1)
    {
        //printf("e");
    }
    
}