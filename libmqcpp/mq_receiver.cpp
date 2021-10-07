//
// Created by deth on 18/12/2019.
//

#include "mq_receiver.h"
#include "sys_error.h"
#include <signal.h>

using namespace mqcpp;

std::mutex mq_receiver::s_setmtx;
std::set<mqd_t> mq_receiver::s_qset;
sigevent mq_receiver::s_sev;

// asynchronous system handler, invoked when item is added to previously empty Q
void mq_receiver::sig_handler(sigval sv)
{
    mq_attr attr = {};

    auto rcvr = reinterpret_cast<mq_receiver*>(sv.sival_ptr);
    auto& prom = rcvr->m_promise;

    if (mq_getattr(rcvr->m_qfd, &attr) == -1) {
        prom.set_exception(
            std::make_exception_ptr(make_system_error(errno, "mq_getattr")));
        return;
    }
    // read the msg the client is waiting for
    auto received = mq_receive(rcvr->m_qfd, rcvr->m_rcvbuf.data(), attr.mq_msgsize, nullptr);
    if (received == -1) {
        prom.set_exception(
            std::make_exception_ptr(make_system_error(errno, "mq_receive")));
        return;
    }
    // and satisfy promise
    prom.set_value(std::string(rcvr->m_rcvbuf.cbegin(), rcvr->m_rcvbuf.cbegin() + received));
}


std::future<std::string> mqcpp::mq_receiver::receive_msg_async()
{
    validate_handle();

    if (m_terminate.load()) {
        throw termination_requested(m_qfd);
    }

    auto received = mq_receive(m_qfd, m_rcvbuf.data(), m_rcvbuf.size(), nullptr);
    if (received == -1) {
        if (errno == EAGAIN) { // empty Q - request async system notification
            return request_notification();
        }
        throw_system_error(errno, "mq_receive");
    }

    // return ready future otherwise
    return std::async(std::launch::deferred,
                      [this, received] { return std::string(m_rcvbuf.cbegin(), m_rcvbuf.cbegin() + received);});
}

std::future<std::string> mqcpp::mq_receiver::request_notification()
{
    s_sev.sigev_notify = SIGEV_THREAD;
    s_sev.sigev_notify_function = mq_receiver::sig_handler;
    s_sev.sigev_notify_attributes = NULL;
    s_sev.sigev_value.sival_ptr = this;

    if (mq_notify(m_qfd, &s_sev) == -1) {
        throw_system_error(errno, "mq_notify");
    }

    m_promise = std::promise<std::string>();
    return m_promise.get_future();
}

std::vector<std::string> mq_receiver::drain_all()
{
    validate_handle();
    std::vector<std::string> msgs;

    auto msg_in_q = msg_count();
    msgs.reserve(msg_in_q);
    while (msg_in_q > 0) {
        auto received = mq_receive(m_qfd, m_rcvbuf.data(), m_rcvbuf.size(), nullptr);
        if (received != -1) {
            msgs.emplace_back(m_rcvbuf.cbegin(), m_rcvbuf.cbegin() + received);
        }

        --msg_in_q;
    }

    return msgs;
}

mq_receiver::mq_receiver(mqd_t qfd, const std::string& name)
    : mq_base(qfd, name), m_terminate(false)
{
    std::lock_guard<std::mutex> lck(s_setmtx);
    if (s_qset.find(qfd) != s_qset.cend()) {
        throw std::logic_error("reader already open for the given handle");
    }

    s_qset.insert(qfd);
}

mq_receiver::~mq_receiver()
{
    notify_shutdown();
    // remove receiver from global map
    if (m_qfd != INVALID_QHANDLE) {
        std::lock_guard<std::mutex> lck(s_setmtx);
        s_qset.erase(m_qfd);
    }
}

void mq_receiver::notify_shutdown()
{
    bool expected = false;
    if (m_terminate.compare_exchange_strong(expected, true)) {
        // remove notification request if any
        mq_notify(m_qfd, nullptr);
        // release blocked readers if any
        if (m_qfd != INVALID_QHANDLE) {
            try {
                m_promise.set_exception(
                    std::make_exception_ptr(termination_requested(m_qfd)));
            }
            catch (std::future_error&) {}
        }
    }
}



