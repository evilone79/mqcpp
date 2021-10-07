//
// Created by deth on 19/12/2019.
//

#ifndef MSGQ_MQ_BASE_H
#define MSGQ_MQ_BASE_H

#include "mqcpp_defs.h"

namespace mqcpp
{

class mq_base
{
public:
    mqd_t m_qfd = INVALID_QHANDLE;

    mq_base(mqd_t qfd, const std::string& name);
    std::string name() const;
    int msg_count() const;

protected:
    static mq_attr get_attributes(mqd_t qfd);
    void validate_handle();
    ~mq_base();

    std::string m_name;
};

}

#endif //MSGQ_MQ_BASE_H
