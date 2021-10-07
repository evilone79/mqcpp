//
// Created by deth on 19/12/2019.
//

#include <stdexcept>
#include "mq_base.h"
#include "sys_error.h"

void mqcpp::mq_base::validate_handle()
{
    if (m_qfd == INVALID_QHANDLE) {
        throw std::runtime_error("mq descriptor is not valid");
    }
}

mqcpp::mq_base::~mq_base()
{
    if (m_qfd != INVALID_QHANDLE) {
        mq_close(m_qfd);
    }
}

mqcpp::mq_base::mq_base(mqd_t qfd, const std::string& name)
    : m_qfd(qfd), m_name(name)
{
}

std::string mqcpp::mq_base::name() const
{
    return m_name;
}

int mqcpp::mq_base::msg_count() const
{
    return get_attributes(m_qfd).mq_curmsgs;
}

mq_attr mqcpp::mq_base::get_attributes(mqd_t qfd)
{
    mq_attr attrib = {};
    const auto res = ::mq_getattr(qfd, &attrib);
    if (res == -1) {
        throw_system_error(errno, "mq_getattr");
    }
    return attrib;
}

