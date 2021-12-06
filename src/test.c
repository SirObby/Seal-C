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

/*

    This exists here to test for some reason, idk D++ has it, so I do.

*/
#include <stdio.h>

#include <sealc/sealc.h>

void on_ready(cluster *cluster) {
    printf("sus");
}

int main() {
    cwr_malloc_ctx_t m_ctx;
    cwr_malloc_ctx_new(&m_ctx);
    cluster bot;
    bot.m_ctx = &m_ctx;
    bot.token = "EE";
    bot.shard_count = 1;

    // cluster_set_events(&bot, 1, discord_ready);
    // cluster_start(&bot, 1, on_ready);

    shard shard1;
    shard_start(&bot, &shard1, uv_default_loop(), 1);

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    return 0;
}