#include "Parser.hpp"
#include <cctype>

bool Parser::parse(const std::string& s, Move& mv) {
    int i = 0;
    while (i < s.size() && isspace(s[i])) i++;

    if (i == s.size() || !isdigit(s[i])) return false;

    int num = 0;
    while (i < s.size() && isdigit(s[i])) {
        num = num * 10 + (s[i] - '0');
        i++;
    }

    if (num < 1 || num > 16) return false;

    while (i < s.size() && isspace(s[i])) i++;

    MoveType mt;

    if (toupper(s[i]) == 'R') mt = MoveType::R;
    else if (toupper(s[i]) == 'B') mt = MoveType::B;
    else if (toupper(s[i]) == 'T' && toupper(s[i + 1]) == 'R') mt = MoveType::TR;
    else if (toupper(s[i]) == 'T' && toupper(s[i + 1]) == 'B') mt = MoveType::TB;
    else return false;

    mv.src = num - 1;
    mv.type = mt;
    return true;
}
