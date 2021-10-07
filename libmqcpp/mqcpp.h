#ifndef MSGQ_MQCPP_H
#define MSGQ_MQCPP_H


#include <string>
#include <memory>


#include "mq_receiver.h"
#include "mq_sender.h"

/*
 * mqcpp - is the C++ wrapper around posix message queue API.
 * Note, that this implementation is MPSC type, meaning Multiple Producer - Single Consumer.
 */
namespace mqcpp
{

/*
 *  Creates new or opens existing receiving endpoint.
 */
std::unique_ptr<mq_receiver> create_receiver(const std::string &qname);
/*
 *  Creates new or opens existing sending endpoint.
 */
std::unique_ptr<mq_sender> create_sender(const std::string &qname);
/*
 *  CLoses queue and removes associated backing file
 */
void shutdown_queue(const std::string &qname);
/*
 *  Checks whether queue exists and returns its size (QSIZE)
 */
std::pair<bool, int> get_mq_persistent_info(const std::string &qname);
/*
 *  returns /proc/sys/fs/max_msg value
 */
int get_mq_max_messages();
/*
 *  updates /proc/sys/fs/max_msg value.
 *  NOTE: admin privileges required
 */
void set_mq_max_messages(int msgmax);

}
#endif //MSGQ_MQCPP_H
