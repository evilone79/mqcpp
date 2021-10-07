#include <iostream>
#include <mqcpp.h>
#include <queue>

int main()
{
    auto echoer = mqcpp::create_receiver("/echo_queue");
    auto sender = mqcpp::create_sender("/main_queue");

    std::atomic_bool terminating{false};

    std::cout << "Start sending messages..\n";

    std::thread echo_thread([&] {
        while (!terminating) {
            std::string msg;
            try {
                msg = echoer->receive_msg_async().get();
            } catch (mqcpp::termination_requested&) {
                std::cout << "Terminating echoer..\n";
                break;
            } catch (const std::exception& e) {
                std::cerr << e.what() << '\n';
                continue;
            }
            std::cout << "Echoed " << msg << "\n";
        }
    });

    for (int i = 0; i < 100; ++i) {
        try {
            sender->send_message("message_" + std::to_string(i));
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
    }
    sender->send_message("quit");    

    echoer->notify_shutdown();

    if (echo_thread.joinable())
        echo_thread.join();

    try {
        mqcpp::shutdown_queue(sender->name());
        mqcpp::shutdown_queue(echoer->name());
    } catch (std::exception& e) {
        std::cout << "Failed to unlink queue. " << e.what() << "\n";
    }

    return 0;
}
