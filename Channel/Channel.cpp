#include "buffered_channel.h"
#include <iostream>
#include <thread>

void producer(BufferedChannel<int>& channel) {
    for (int i = 0; i < 5; ++i) {
        channel.send(i);
        std::cout << "Sent: " << i << std::endl;
    }
    channel.close();
}

void consumer(BufferedChannel<int>& channel) {
    while (true) {
        auto [value, success] = channel.recv();
        if (!success) break;
        std::cout << "Received: " << value << std::endl;
    }
}

int main() {
    BufferedChannel<int> channel(2); 

    std::thread t1(producer, std::ref(channel));
    std::thread t2(consumer, std::ref(channel));

    t1.join();
    t2.join();

    return 0;
}
