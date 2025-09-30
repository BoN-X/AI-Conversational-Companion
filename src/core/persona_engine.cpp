#include "persona_engine.h"
#include <fstream>
#include <sstream>
#include <string>

std::string PersonaEngine::buildSystemPrompt(const std::string& assets_dir) const {
    const std::string path = assets_dir + "persona.json";
    std::ifstream in(path, std::ios::binary);
    std::ostringstream ss;
    ss << "你是一位虚拟女友，请严格遵循以下人设与边界：\n";
    if (in) {
        ss << in.rdbuf();
    } else {
        ss << R"({"name":"Luna","style":"温柔理性","boundaries":["不提供医疗/财务建议"]})";
    }
    ss << "\n输出要求：自然口语、简洁、积极，适度加入情绪化表达但注意分寸。";
    return ss.str();
}
