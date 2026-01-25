#pragma once

// No OS
#define NO_SYS 1

// RAW API only
#define LWIP_NETCONN 0
#define LWIP_SOCKET 0

// Core network features used by cyw43 examples
#define LWIP_TCP 1
#define LWIP_UDP 1
#define LWIP_DNS 1
#define LWIP_DHCP 1
#define LWIP_ARP 1
#define LWIP_ICMP 1

// ---- Memory: MUST NOT use libc malloc in threadsafe_background ----
#define MEM_LIBC_MALLOC 0
#define MEM_SIZE (16 * 1024)
#define MEMP_MEM_MALLOC 0

// pbuf pool (match your BUF_SIZE=2048-ish use; can be smaller, but this is fine)
#define PBUF_POOL_SIZE 16
#define PBUF_POOL_BUFSIZE 2048

// TCP tuning (optional)
#define TCP_MSS 1460
#define TCP_SND_BUF (4 * TCP_MSS)
#define TCP_WND (4 * TCP_MSS)

// Keepalive (optional)
#define LWIP_TCP_KEEPALIVE 1