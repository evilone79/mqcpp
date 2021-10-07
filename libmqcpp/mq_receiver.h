//
// Created by deth on 18/12/2019.
//

#ifndef MSGQ_MQ_RECEIVER_H
#define MSGQ_MQ_RECEIVER_H

#include <future>
#include <set>
#include <vector>

#include "mq_base.h"
#include "mqcpp_defs.h"
#include <atomic>


namespace mqcpp
{

class termination_requested : public std::runtime_error
{
public:
    termination_requested(mqd_t qid) :
        std::runtime_error("termination requested for " + std::to_string(qid)) {}
};


class mq_receiver : public mq_base
{

    static void sig_handler(sigval sv);

public:
    mq_receiver() : mq_base(0, "") {}

    explicit mq_receiver(mqd_t qfd, const std::string& name);
    ~mq_receiver();

    /*
     * Gets next message from queue synchronously,
     * relying upon timeout parameter. If timeout is reached prior
     * receiving new message, false is returned, true otherwise.
     * If timeout_ms set to 0, function will return immediately.
     */
//    bool receive_msg(std::string& msg, int timeout_ms);
    /*
     *  Gets next message from queue asynchronously. If called after
     *  notify_shutdown() was issued, mqcpp::termination_requested exception
     *  will be thrown
     */
    std::future<std::string> receive_msg_async();
    /*
     *  Reads all messages that are currently on the queue, if any.
     *  This method may still be called after notify_shutdown() was issued
     */
    std::vector<std::string> drain_all();
    /*
     *  Cancels any asynchronous operation on the receiver and
     *  mqcpp::termination_requested exception will be thrown on thread,
     *  awaiting on receive_msg_async() future.
     */
    void notify_shutdown();

private:
    std::future<std::string> request_notification();
    std::array<char, mqcpp::MAX_MSG_SIZE> m_rcvbuf;
    std::atomic_bool m_terminate;
    std::promise<std::string> m_promise;

    static std::mutex s_setmtx;
    static std::set<mqd_t> s_qset;
    static sigevent s_sev;
};

}
#endif //MSGQ_MQ_RECEIVER_H
