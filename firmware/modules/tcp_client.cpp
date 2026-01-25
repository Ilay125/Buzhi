#include "tcp_client.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>

#ifndef DEBUG_printf
#define DEBUG_printf printf
#endif

extern "C" {
    volatile bool g_tcp_rx_ready = false;
    volatile int  g_tcp_rx_len   = 0;
    char          g_tcp_rx_buf[16] = {0};   // ~10 chars + null + slack
}

TcpClient::TcpClient()
    : pcb_(nullptr),
      remote_port_(kDefaultPort),
      buffer_len_(0),
      sent_len_(0),
      complete_(false),
      connected_(false),
      run_count_(0),
      iterations_(10),
      poll_time_s_(5),
      result_status_(-1) {
    // Default address is 0.0.0.0 until set_server_ip() is called
    ip4addr_aton("0.0.0.0", &remote_addr_);
    std::memset(buffer_, 0, sizeof(buffer_));
}

TcpClient::~TcpClient() {
    // Ensure pcb is closed if user forgets
    close_pcb();
}

void TcpClient::set_server_ip(const char* ip_string) {
    ip4addr_aton(ip_string, &remote_addr_);
}

void TcpClient::set_server_port(uint16_t port) {
    remote_port_ = port;
}

void TcpClient::set_iterations(int iterations) {
    iterations_ = iterations;
}

void TcpClient::set_poll_interval_seconds(int seconds) {
    poll_time_s_ = seconds;
}

bool TcpClient::start() {
    complete_ = false;
    connected_ = false;
    result_status_ = -1;
    run_count_ = 0;
    buffer_len_ = 0;
    sent_len_ = 0;

    if (pcb_ != nullptr) {
        close_pcb();
    }

    DEBUG_printf("Connecting to %s port %u\n", ip4addr_ntoa(&remote_addr_), remote_port_);

    pcb_ = tcp_new_ip_type(IP_GET_TYPE(&remote_addr_));
    if (!pcb_) {
        DEBUG_printf("failed to create pcb\n");
        complete_ = true;
        result_status_ = -1;
        return false;
    }

    tcp_arg(pcb_, this);
    tcp_recv(pcb_, &TcpClient::s_recv);
    tcp_poll(pcb_, &TcpClient::s_poll, (poll_time_s_ > 0) ? (poll_time_s_ * 2) : 0);
    tcp_err(pcb_, &TcpClient::s_error);
    tcp_sent(pcb_, &TcpClient::s_sent);

    // Lock around lwIP calls (good practice; safe even in poll mode)
    cyw43_arch_lwip_begin();
    err_t err = tcp_connect(pcb_, &remote_addr_, remote_port_, &TcpClient::s_connected);
    cyw43_arch_lwip_end();

    if (err != ERR_OK) {
        DEBUG_printf("tcp_connect failed %d\n", err);
        finish(err);
        return false;
    }
    return true;
}

void TcpClient::service() {
#if PICO_CYW43_ARCH_POLL
    cyw43_arch_poll();
    cyw43_arch_wait_for_work_until(make_timeout_time_ms(1));
#endif
}

// ------------------- Static callback forwarders -------------------

err_t TcpClient::s_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
    return static_cast<TcpClient*>(arg)->on_connected(tpcb, err);
}

err_t TcpClient::s_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    return static_cast<TcpClient*>(arg)->on_sent(tpcb, len);
}

err_t TcpClient::s_poll(void *arg, struct tcp_pcb *tpcb) {
    return static_cast<TcpClient*>(arg)->on_poll(tpcb);
}


void TcpClient::s_error(void *arg, err_t err) {
    // arg can be nullptr in some lwIP error paths
    if (arg) static_cast<TcpClient*>(arg)->on_error(err);
}

err_t TcpClient::s_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    return static_cast<TcpClient*>(arg)->on_recv(tpcb, p, err);
}

// ------------------- Instance handlers -------------------

err_t TcpClient::on_connected(struct tcp_pcb *tpcb, err_t err) {
    (void)tpcb;
    if (err != ERR_OK) {
        DEBUG_printf("connect failed %d\n", err);
        return finish(err);
    }
    connected_ = true;

    rx_buf_.clear();
    rx_queue_.clear();

    const char *hello = "hi\n";
    tcp_write(pcb_, hello, strlen(hello), TCP_WRITE_FLAG_COPY);
    tcp_output(pcb_);

    DEBUG_printf("Connected. Waiting for buffer from server\n");
    return ERR_OK;
}

err_t TcpClient::on_sent(struct tcp_pcb *tpcb, u16_t len) {
    (void)tpcb;
    DEBUG_printf("tcp_client_sent %u\n", len);
    sent_len_ += (int)len;

    if (sent_len_ >= (int)kBufSize) {
        run_count_++;

        if (run_count_ >= iterations_) {
            return finish(0);
        }

        // Expect a new buffer from server
        buffer_len_ = 0;
        sent_len_ = 0;
        DEBUG_printf("Waiting for buffer from server\n");
    }

    return ERR_OK;
}

err_t TcpClient::on_poll(struct tcp_pcb *tpcb) {
    (void)tpcb;
    // never treat "no activity" as failure
    return ERR_OK;
}



void TcpClient::on_error(err_t err) {
    if (err != ERR_ABRT) {
        DEBUG_printf("tcp_client_err %d\n", err);
        finish(err);
    }
}

err_t TcpClient::on_recv(struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    (void)tpcb;
    (void)err;

    if (!p) {
        // server closed
        connected_ = false;
        return ERR_OK;
    }

    cyw43_arch_lwip_check();

    // Copy first ~10 bytes from the pbuf chain into global buffer
    const int MAX_COPY = 10;
    int copied = 0;

    for (struct pbuf *q = p; q != nullptr && copied < MAX_COPY; q = q->next) {
        const char *data = (const char *)q->payload;
        int take = (int)q->len;
        if (take > (MAX_COPY - copied)) take = (MAX_COPY - copied);
        memcpy(g_tcp_rx_buf + copied, data, (size_t)take);
        copied += take;
    }

    g_tcp_rx_buf[copied] = '\0';
    g_tcp_rx_len = copied;
    g_tcp_rx_ready = true;

    tcp_recved(pcb_, p->tot_len);
    pbuf_free(p);

    return ERR_OK;
}




// ------------------- Finish/close helpers -------------------

err_t TcpClient::finish(int status) {
    if (!complete_) {
        result_status_ = status;
        if (status == 0) DEBUG_printf("test success\n");
        else DEBUG_printf("test failed %d\n", status);
        complete_ = true;
    }
    return close_pcb();
}

err_t TcpClient::close_pcb() {
    err_t err = ERR_OK;
    if (pcb_ != nullptr) {
        tcp_arg(pcb_, nullptr);
        tcp_poll(pcb_, nullptr, 0);
        tcp_sent(pcb_, nullptr);
        tcp_recv(pcb_, nullptr);
        tcp_err(pcb_, nullptr);

        err = tcp_close(pcb_);
        if (err != ERR_OK) {
            DEBUG_printf("close failed %d, calling abort\n", err);
            tcp_abort(pcb_);
            err = ERR_ABRT;
        }
        pcb_ = nullptr;
    }
    return err;
}

bool TcpClient::has_message() const {
    return !rx_queue_.empty();
}

std::string TcpClient::pop_message() {
    if (rx_queue_.empty())
        return {};

    std::string msg = rx_queue_.front();
    rx_queue_.erase(rx_queue_.begin());
    return msg;
}