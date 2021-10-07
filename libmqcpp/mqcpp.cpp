#include <stdexcept>
#include <climits>
#include "mqcpp.h"

#include "mqcpp_defs.h"
#include "sys_error.h"
#include <fstream>
#include <cstring>

using namespace mqcpp;

static void validate_creation_name(const std::string& name)
{
    if (name.empty()) {
        throw std::invalid_argument("invalid mq_name: name can't be an empty string.");
    }

    if (name.size() > NAME_MAX) {
        throw std::invalid_argument("invalid mq_name: name is longer than NAME_MAX");
    }

    if (name[0] != '/') {
        throw std::invalid_argument("invalid mq_name: name doesn't start with '/'");
    }

    for (size_t i = 1; i < name.size(); i++) {
        if (name.substr(1).find('/') != std::string::npos) {
            throw std::invalid_argument("invalid mq_name: other than the first character, there must be no other '/");
        }
    }
}

static mqd_t open_queue(const std::string& qname, const creation_options& opts)
{
    validate_creation_name(qname);
    mqd_t qfd;

    if ((opts.flags & creation_flags::CREATE_OPEN) > 0 || (opts.flags & creation_flags::CREATE_ONLY) > 0) {
        //need to supply two more arguments for mq_open()
        mq_attr attr = {};
        attr.mq_maxmsg = opts.max_queue_size;
        attr.mq_msgsize = opts.max_message_size;

        qfd = ::mq_open(qname.c_str(),
                        static_cast<int>(opts.rw_permission) | static_cast<int>(opts.flags),
                        0777, //full permissions
                        &attr);
    }
    else {
        qfd = ::mq_open(qname.c_str(), static_cast<int>(opts.rw_permission));
    }

    if (qfd == INVALID_QHANDLE) {
        throw_system_error(errno, "mq_open failed");
    }

    return qfd;
}

std::unique_ptr<mq_receiver> mqcpp::create_receiver(const std::string& qname)
{
    creation_options opts;
    opts.flags = creation_flags::CREATE_OPEN | creation_flags::NONBLOCKING;
    opts.rw_permission = permission::READ;
    auto qfd = open_queue(qname, opts);
    return std::make_unique<mq_receiver>(qfd, qname);
}



std::unique_ptr<mq_sender> mqcpp::create_sender(const std::string& qname)
{
    creation_options opts;
    opts.flags = creation_flags::CREATE_OPEN | creation_flags::NONBLOCKING;
    opts.rw_permission = permission::WRITE;
    auto qfd = open_queue(qname, opts);
    return std::make_unique<mq_sender>(qfd, qname);

}

void mqcpp::shutdown_queue(const std::string& qname)
{
    auto rs = mq_unlink(qname.c_str());
    if (rs == -1) {
        throw_system_error(errno, "mq_unlink");
    }
}

std::pair<bool, int> mqcpp::get_mq_persistent_info(const std::string& qname)
{
    std::ifstream mqf(mqcpp::MQ_MOUNT_DIR + qname);
    if (mqf) {
        std::string data;
        mqf >> data;
        auto sizestr = data.substr(data.find(':') + 1);
        auto size = std::stoi(sizestr);
        return {true, size};
    }

    if (errno != ENOENT)
        return {false, -1};

    return {true, 0};
}

int mqcpp::get_mq_max_messages()
{
    std::ifstream ifs(mqcpp::MQ_MAX_MSG_FILE);
    if (ifs) {
        int max_msg;
        ifs >> max_msg;
        return max_msg;
    }
    throw_system_error(errno, "get_mq_max_messages");
}

void mqcpp::set_mq_max_messages(int msgmax)
{
    std::ofstream ofs(MQ_MAX_MSG_FILE, std::ios::binary);
    if (ofs) {
        ofs << msgmax;
    }
    else {
        throw_system_error(errno, "set_mq_max_messages");
    }
}

