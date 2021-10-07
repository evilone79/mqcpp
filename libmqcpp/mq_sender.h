//
// Created by deth on 18/12/2019.
//

#ifndef MSGQ_MQ_SENDER_H
#define MSGQ_MQ_SENDER_H

#include "mqcpp_defs.h"
#include "mq_base.h"
#include <vector>

namespace mqcpp
{

class mq_sender : public mq_base
{
public:
    mq_sender() : mq_base(0, "") {}

    explicit mq_sender(mqd_t qfd, const std::string& name) :
        mq_base(qfd, name)
    {
    }

    /*
     * Sends string message. Message size must not exceed mqcpp::MAX_MSG_SIZE
     */
    bool send_message(const std::string& msg);
    /*
     * Sends data array. Data size must not exceed mqcpp::MAX_MSG_SIZE
     */
    bool send_data(const std::vector<uint8_t>& data);

private:
    bool send(const char* ptr, size_t size);

};

}
#endif //MSGQ_MQ_SENDER_H
