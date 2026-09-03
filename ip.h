#ifndef IP_H
#define IP_H

#include <stddef.h>
#include <stdint.h>

#define IP_VERSION_IPV4 4

#define IP_HDR_SIZE_MIN 20
#define IP_HDR_SIZE_MAX 60

#define IP_TOTAL_SIZE_MAX UINT16_MAX /* maximum value of uint16 */
#define IP_PAYLOAD_SIZE_MAX (IP_TOTAL_SIZE_MAX - IP_HDR_SIZE_MIN)

#define IP_ADDR_LEN 4
#define IP_ADDR_STR_LEN 16 /* "ddd.ddd.ddd.ddd\0" */

typedef uint32_t ip_addr_t;

// IP ヘッダ構造体
struct ip_hdr
{
    uint8_t vhl;      // バージョン + ヘッダ長
    uint8_t tos;      // サービスタイプ(Type of Service)
    uint16_t total;   // パケット全体の長さ(バイト単位)
    uint16_t id;      // 識別子
    uint16_t offset;  // フラグ + フラグメントオフセット
    uint8_t ttl;      // 生存時間(Time to Live)
    uint8_t protocol; // 上位プロトコル
    uint16_t sum;     // ヘッダチェックサム
    ip_addr_t src;    // 送信元IPアドレス
    ip_addr_t dst;    // 宛先IPアドレス
};

extern const ip_addr_t IP_ADDR_ANY;
extern const ip_addr_t IP_ADDR_BROADCAST;

extern int
ip_addr_pton(const char *p, ip_addr_t *n);
extern char *
ip_addr_ntop(ip_addr_t n, char *p, size_t size);

extern int
ip_init(void);

#endif
