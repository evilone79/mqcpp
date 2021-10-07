//
// Created by deth on 18/12/2019.
//

#ifndef MSGQ_MQCPP_DEFS_H
#define MSGQ_MQCPP_DEFS_H

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <string>

namespace mqcpp
{

static constexpr auto MAX_MSG_SIZE = 8192;
static constexpr auto MAX_QUEUE_SIZE = 10;
static constexpr auto INVALID_QHANDLE = -1;

static constexpr auto MQ_MOUNT_DIR = "/dev/mqueue/";
static constexpr auto MQ_MAX_MSG_FILE = "/proc/sys/fs/mqueue/msg_max";

enum class permission {
    READ = O_RDONLY,
    WRITE = O_WRONLY,
    READ_WRITE = O_RDWR,
};

enum creation_flags : int {
    OPEN_ONLY = 0,
    CREATE_ONLY = O_CREAT | O_EXCL,
    CREATE_OPEN = O_CREAT,
    NONBLOCKING = O_NONBLOCK
};

struct creation_options {
    int flags = 0;
    permission rw_permission;
    int max_message_size = MAX_MSG_SIZE;
    int max_queue_size = MAX_QUEUE_SIZE;
};



}

#endif //MSGQ_MQCPP_DEFS_H
