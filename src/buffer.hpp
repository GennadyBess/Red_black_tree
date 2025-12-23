#pragma once
#include <vector>
// #include <cstddef>

class CircularBuffer {
private:
    std::vector<char> buffer;
    std::size_t head;
    std::size_t tail;
    std::size_t count;

public:
    CircularBuffer(std::size_t capacity)
        : buffer(capacity),
        head(0),
        tail(0),
        count(0) {}

    void push(char value) {
        buffer[head] = value;
        head = (head + 1) % buffer.size();

        if (count == buffer.size()) {
            tail = (tail + 1) % buffer.size();
        } else {
            count++;
        }
    }

    bool pop(char& value) {
        if (count == 0) return false;

        value = buffer[tail];
        tail = (tail + 1) % buffer.size();
        count--;
        return true;
    }


    bool full() const {
        return count == buffer.size();
    }

    void clear() {
        head = tail = count = 0;
    }

    std::size_t size() const {
        return count;
    }

    std::size_t capacity() const {
        return buffer.size();
    }


};


