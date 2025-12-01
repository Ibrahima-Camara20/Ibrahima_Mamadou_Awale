#include "Parser.hpp"
#include <cctype>

bool Parser::parse(const std::string& s, Move& mv) {
    std::string::size_type i = 0;
    while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) ++i;

    if (i == s.size() || !std::isdigit(static_cast<unsigned char>(s[i]))) return false;

    int num = 0;
    while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i]))) {
        num = num * 10 + (s[i] - '0');
        ++i;
    }

    if (num < 1 || num > 16) return false;

    while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) ++i;

    if (i >= s.size()) return false;

    MoveType mt;
    char a = std::toupper(static_cast<unsigned char>(s[i]));
    char b = (i + 1 < s.size()) ? std::toupper(static_cast<unsigned char>(s[i + 1])) : '\0';

    if (a == 'R') mt = MoveType::R;
    else if (a == 'B') mt = MoveType::B;
    else if (a == 'T' && b == 'R') mt = MoveType::TR;
    else if (a == 'T' && b == 'B') mt = MoveType::TB;
    else return false;

    mv.src = num - 1;
    mv.type = mt;
    return true;
}
