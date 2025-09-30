#pragma once
#include <string>

class PersonaEngine {
public:
    std::string buildSystemPrompt(const std::string& assets_dir = "assets/") const;
};
