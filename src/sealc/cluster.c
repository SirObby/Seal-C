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

static const char *gateway_close_codes[] = {
    "Unknown error",
    "Unknown opcode",
    "Decode error",
    "Not authenticated",
    "Authentication failed",
    "Already authenticated",
    "Undocumented error?", /* 4006 missing */
    "Invalid seq",
    "Rate limited",
    "Session timed out",
    "Invalid shard",
    "Sharding required",
    "Invalid API version",
    "Invalid intent(s)",
    "Disallow intent(s)"
};

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

static void _shard_tcp_connect (cwr_sock_t *tcp)
{
    shard *shard = tcp->data;

    // TODO: check returns successful
    cwr_tls_connect_with_sni(&shard->tls, SEALC_GW_HOST);
    cwr_ws_connect(&shard->ws, SEALC_GW_WS_URI, sizeof(SEALC_GW_WS_URI) - 1);
    cwr_sock_read_start(tcp); // start receiving data
}

static void _shard_stop_timers (shard *shard)
{
    if (shard->status.ready)
        uv_timer_stop(&shard->heartbeat_timer);
    if (shard->status.reidentifying)
        uv_timer_stop(&shard->identify_timeout);
}

static void _shard_tcp_close (cwr_sock_t *tcp)
{
    shard *shard = tcp->data;
    // TODO: we've closed the whole network stack, we may reconnect.
    _shard_stop_timers(shard);
}

static void _shard_tls_close (cwr_tls_t *tls)
{
    shard *shard = tls->data;
    _shard_stop_timers(shard);
    cwr_sock_shutdown(&shard->tcp);
}

static void _shard_ws_close (cwr_ws_t *ws)
{
    shard *shard = ws->data;
    _shard_stop_timers(shard);
    // XXX: sock gets shutdown after tls is.
    // cwr_sock_shutdown(&shard->tcp);
    cwr_tls_shutdown(&shard->tls);
}

static void _shard_stream_err (cwr_linkable_t *stream)
{
    shard *shard = stream->data;
    _shard_stop_timers(shard);
    // TODO: handle error
}

static void _shard_ws_fail (cwr_ws_t *ws)
{
    shard *shard = ws->data;
    _shard_stop_timers(shard);
    // TODO: we've failed to open a ws connection.
}

static void _shard_close_recv (cwr_ws_t *ws, uint16_t status, const char *reason, size_t reason_len)
{
    shard *shard = ws->data;
    // TODO: server has sent a close frame. we should handle it here.
    // the websocket will close itself after this method has run.
    // we may want to do something else here before that happens.
}

static void _shard_msg_chunk (cwr_ws_t *ws, const char *data, size_t len)
{
    shard *shard = ws->data;

    if (cwr_buf_push_back(&shard->frame_buffer, data, len) == NULL)
        return; // TODO: handle out of memory. maybe just terminate the program?
}

static void _shard_msg_recv (cwr_ws_t *ws)
{
    shard *shard = ws->data;
    cwr_buf_t *msg = &shard->frame_buffer;

    // TODO: handle message
    // I'm just dumping the message to console for now
    printf("%.*s\n", msg->len, msg->base);

    // clear the frame buffer.
    shard->frame_buffer.len = 0;
}

int shard_start(cluster *cluster, shard *shard, uv_loop_t *loop, int id)
{
    memset(shard, 0, sizeof(*shard));
    shard->id = id;
    shard->cluster = cluster;
    shard->loop = loop;
    shard->m_ctx = cluster->m_ctx;

    uv_timer_init(shard->loop, &shard->heartbeat_timer);
    shard->heartbeat_timer.data = shard;
    uv_timer_init(shard->loop, &shard->identify_timeout);
    shard->identify_timeout.data = shard;

    if (cwr_buf_malloc(&shard->frame_buffer, shard->m_ctx, SEALC_FRAME_BUF_SIZE) == NULL)
        return 1;

    int r;

    r = cwr_sock_init(shard->m_ctx, loop, &shard->tcp);

    if (r)
        goto failbuf;

    r = cwr_tls_init(shard->m_ctx, &shard->tcp, &shard->tls);

    if (r)
        goto failbuf;

    r = cwr_ws_init(shard->m_ctx, &shard->tls, &shard->ws);

    if (r)
        goto failtls;


    shard->tcp.data = shard;
    shard->tls.data = shard;
    shard->ws.data = shard;

    shard->tcp.on_connect = _shard_tcp_connect;

    // underlying close handlers
    shard->tcp.on_close = _shard_tcp_close;
    shard->tls.on_close = _shard_tls_close;
    shard->ws.on_close = _shard_ws_close;

    // fatal error handlers
    shard->tcp.io.on_error = (cwr_sock_link_cb)_shard_stream_err;
    shard->tls.io.on_error = (cwr_tls_link_cb)_shard_stream_err;
    shard->ws.io.on_error = (cwr_ws_link_cb)_shard_stream_err;
    shard->ws.on_fail = _shard_ws_fail;

    shard->ws.on_receive_close = _shard_close_recv;
    shard->ws.on_message = _shard_msg_chunk;
    shard->ws.on_message_complete = _shard_msg_recv;

    return cwr_sock_connect_host(&shard->tcp, SEALC_GW_HOST, SEALC_DEFAULT_PORT);
failtls:
    if (shard->tls.sec_ctx.ssl_ctx)
        cwr_sec_ctx_free(&shard->tls.sec_ctx);
    cwr_tls_free(&shard->tls);
failbuf:
    cwr_buf_free(&shard->frame_buffer);
    return 1;
}
