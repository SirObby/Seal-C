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

typedef struct _cluster cluster;
struct _cluster
{
    char *token;
    int shard_count;
//    void (*cluster_log)(cluster *, char *);
//    void (*cluster_start)(cluster *);
    discord_idle_cb on_ready_evnt;
    
};

typedef struct _discord_user discord_user;
struct _discord_user {
    char *username;
    int discriminator;
};

/** 
 * @brief Idle callback
 */
typedef void (*discord_idle_cb)(cluster *cluster, discord_user *bot);

void cluster_log(cluster *c, char *msg);
void cluster_start(cluster *c);
void set_on_ready(cluster *c, discord_idle_cb callback);