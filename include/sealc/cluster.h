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

#ifndef _SEALC_CLUSTER_H
#define _SEALC_CLUSTER_H

enum discord_events
{
    discord_application_command_create,
    discord_application_command_delete,
    discord_application_command_update,
    discord_channel_create,
    discord_channel_delete,
    discord_channel_pins_update,
    discord_channel_update,
    discord_guild_ban_add,
    discord_guild_ban_remove,
    discord_guild_create,
    discord_guild_delete,
    discord_guild_emojis_update,
    discord_guild_integrations_update,
    discord_guild_join_request_delete,
    discord_guild_member_add,
    discord_guild_member_remove,
    discord_guild_member_update,
    discord_guild_members_chunk,
    discord_guild_role_create,
    discord_guild_role_delete,
    discord_guild_role_update,
    discord_guild_scheduled_event_create,
    discord_guild_scheduled_event_delete,
    discord_guild_scheduled_event_update,
    discord_guild_scheduled_event_user_add,
    discord_guild_scheduled_event_user_remove,
    discord_guild_stickers_update,
    discord_integration_create,
    discord_integration_delete,
    discord_integration_update,
    discord_interaction_create,
    discord_invite_create,
    discord_invite_delete,
    discord_logger,
    discord_message_create,
    discord_message_delete,
    discord_message_delete_bulk,
    discord_message_reaction_add,
    discord_message_reaction_remove,
    discord_message_reaction_remove_all,
    discord_message_reaction_remove_emoji,
    discord_message_update,
    discord_precense_update,
    discord_ready,
    discord_resumed,
    discord_stage_instance_create,
    discord_stage_instance_delete,
    discord_stage_instance_update,
    discord_thread_create,
    discord_thread_delete,
    discord_thread_list_sync,
    discord_thread_member_update,
    discord_thread_members_update,
    discord_thread_update,
    discord_typing_start,
    discord_user_update,
    discord_voice_server_update,
    discord_voice_state_update,
    discord_webhooks_update
};

typedef struct _cluster cluster;
struct _cluster
{
    char *token;
    int shard_count;
//    void (*cluster_log)(cluster *, char *);
//    void (*cluster_start)(cluster *);
    enum discord_events de[58];
};

void cluster_start(cluster *c, int event_count, ... );
void cluster_log(cluster *c, char *msg);
void cluster_set_events(cluster *c, int event_count, ...);

#endif
