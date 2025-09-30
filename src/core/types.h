#pragma once
#include <string>
#include <vector>

struct Message {
    std::string role;           //"system" | "user" | "assistant"
    std::string content;
};

using ChatHistory = std::vector<Message>;
