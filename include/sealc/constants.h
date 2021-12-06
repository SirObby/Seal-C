#ifndef _SEALC_CONSTANTS_H
#define _SEALC_CONSTANTS_H

/* Gateway defines */
#define SEALC_GW_VERSION             9
#define SEALC_GW_VERSION_STR         "9"
#ifdef SEALC_GW_COMPRESS
#define SEALC_GW_WS_QUERY            "?v=" SEALC_GW_VERSION_STR "&encoding=json&compress=zlib-stream"
#else
#define SEALC_GW_WS_QUERY            "?v=" SEALC_GW_VERSION_STR "&encoding=json"
#endif
#define SEALC_GW_WS_URI              "wss://gateway.discord.gg/" SEALC_GW_WS_QUERY
#define SEALC_GW_HOST                "gateway.discord.gg"
#define SEALC_FRAME_BUF_SIZE         4096
#define SEALC_DECOMP_BUF_SIZE        SEALC_FRAME_BUF_SIZE
/* Allow sending 2 gateway commands per second */
#define SEALC_GW_COMMAND_LIMIT       2
#define SEALC_GW_COMMAND_LIMIT_RESET 1000

/* API Defines */
#define SEALC_DEFAULT_PORT           "443"
#define SEALC_API_HOST               "discord.com"
#define SEALC_API_PORT               SEALC_DEFAULT_PORT
#define SEALC_API_VERSION            9
#define SEALC_API_VERSION_STR        "9"
#define SEALC_API_BASE_PATH          "/api/v" SEALC_API_VERSION_STR
#define SEALC_CDN_HOST               "cdn.discordapp.com"
#define SEALC_CDN_PORT               SEALC_DEFAULT_PORT
#define SEALC_USER_AGENT             "DiscordBot (" SEALC_GIT_REPO ", " SEALC_VERSION_STR ")"

#endif
