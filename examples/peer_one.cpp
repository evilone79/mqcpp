#include <iostream>
#include <mqcpp.h>
#include <queue>

int main()
{
    auto max_msg = mqcpp::get_mq_max_messages();
    std::cout << "Max messages per queue: " << max_msg << "\n";

    auto echoer = mqcpp::create_sender("/echo_queue");
    auto receiver = mqcpp::create_receiver("/main_queue");

    std::cout << "Waiting for incoming messages..\n";
    for (;;) {
        std::string msg;
        try {
            msg = receiver->receive_msg_async().get();
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            continue;
        }
        if (msg == "quit") {
            std::cout << "Received quit command. Closing.\n";
            break;
        }
        std::cout << "Received message: '" << msg << "'. Echoing..\n";
        try {
            echoer->send_message(msg);
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
    }

    return 0;
}
