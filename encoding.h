// Character code-related codes by katahiromz
// License: MIT
#pragma once

inline bool vsk_isupper(char ch)
{
    return ('A' <= ch && ch <= 'Z');
}

inline bool vsk_islower(char ch)
{
    return ('a' <= ch && ch <= 'z');
}

inline bool vsk_isalpha(char ch)
{
    return vsk_islower(ch) || vsk_isupper(ch);
}

inline bool vsk_isdigit(char ch)
{
    return ('0' <= ch && ch <= '9');
}

inline bool vsk_isalnum(char ch)
{
    return vsk_isalpha(ch) || vsk_isdigit(ch);
}

inline bool vsk_isblank(char ch)
{
    return ch == ' ' || ch == '\t';
}

inline void vsk_upper(std::string& str)
{
    for (auto& ch : str)
    {
        if (vsk_islower(ch))
            ch += ('A' - 'a');
    }
}
