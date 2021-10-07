//
// Created by deth on 18/12/2019.
//

#include "mq_sender.h"
#include "sys_error.h"

bool mqcpp::mq_sender::send_message(const std::string& msg)
{
    return send(msg.c_str(), msg.size());
}

bool mqcpp::mq_sender::send_data(const std::vector<uint8_t>& msg)
{
    return send(reinterpret_cast<const char*>(msg.data()), msg.size());
}

bool mqcpp::mq_sender::send(const char* ptr, size_t size)
{
    validate_handle();

    auto res = mq_send(m_qfd, ptr, size, 1);

    if (res == -1) {
        if (errno == EAGAIN) {
            return false;
        }
        throw_system_error(errno, "mq_send failed");
    }
    return true;
}


