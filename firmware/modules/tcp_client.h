#ifndef WRAPPER_TCP_CLIENT_H
#define WRAPPER_TCP_CLIENT_H

#include <cstdint>
#include <cstddef>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/ip_addr.h"

#include <vector>
#include <string>


// A small TCP client that connects to a server, waits for BUF_SIZE bytes,
// then sends them back. Repeats for N iterations (like the original example).
class TcpClient {
public:
    // Adjust if you want different defaults
    static constexpr uint16_t kDefaultPort = 4242;
    static constexpr size_t   kBufSize     = 2048;

    TcpClient();
    ~TcpClient();

    // Non-copyable (lwIP pcb ownership)
    TcpClient(const TcpClient&) = delete;
    TcpClient& operator=(const TcpClient&) = delete;

    // Configure destination and test behavior
    void set_server_ip(const char* ip_string); // e.g. "192.168.1.50"
    void set_server_port(uint16_t port);
    void set_iterations(int iterations);
    void set_poll_interval_seconds(int seconds);

    // Start connection (returns true if tcp_connect() was successfully initiated)
    bool start();

    // For poll-mode builds, call periodically in your main loop.
    // (Safe to call in non-poll builds too; it will do nothing extra.)
    void service();

    bool is_complete() const { return complete_; }
    bool is_connected() const { return connected_; }

    // Optional: last status (0 success, otherwise lwIP error / -1)
    int result_status() const { return result_status_; }

    std::string received_string() const {
        return std::string(rx_data_.begin(), rx_data_.end());
    }

    bool has_message() const;
    std::string pop_message();

private:
    // ---- lwIP callbacks (static -> forward to instance) ----
    static err_t s_connected(void *arg, struct tcp_pcb *tpcb, err_t err);
    static err_t s_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
    static err_t s_poll(void *arg, struct tcp_pcb *tpcb);
    static void  s_error(void *arg, err_t err);
    static err_t s_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);

    // ---- instance handlers ----
    err_t on_connected(struct tcp_pcb *tpcb, err_t err);
    err_t on_sent(struct tcp_pcb *tpcb, u16_t len);
    err_t on_poll(struct tcp_pcb *tpcb);
    void  on_error(err_t err);
    err_t on_recv(struct tcp_pcb *tpcb, struct pbuf *p, err_t err);

    // Close pcb and mark complete
    err_t close_pcb();
    err_t finish(int status);

private:
    struct tcp_pcb* pcb_;
    ip_addr_t       remote_addr_;
    uint16_t        remote_port_;

    uint8_t         buffer_[kBufSize];
    int             buffer_len_;
    int             sent_len_;

    bool            complete_;
    bool            connected_;
    int             run_count_;
    int             iterations_;
    int             poll_time_s_;

    int             result_status_;

    std::vector<char> rx_data_;
    std::vector<char> rx_buf_;                 // assembling current message

    std::vector<std::string> rx_queue_;        // completed messages


};

#endif // WRAPPER_TCP_CLIENT_H
