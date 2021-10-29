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

#include "sealc/cluster.h"

void cluster_log(cluster *c, char *msg)
{
}

void cluster_start(cluster *c)
{
    if(c->shard_count == 0) {
        c->shard_count = 1;
    };
    for (size_t i = 0; i < c->shard_count; i++)
    {
        cluster_log(c, "Starting shard..");
    }

    (*c->on_ready_evnt);
    
}

void set_on_ready(cluster *c, discord_idle_cb callback) {
    c->on_ready_evnt = callback;
    //(*f);
}