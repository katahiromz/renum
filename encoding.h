#pragma once

inline bool vsk_is_upper(char ch)
{
    return ('A' <= ch && ch <= 'Z');
}

inline bool vsk_is_lower(char ch)
{
    return ('a' <= ch && ch <= 'z');
}

inline bool vsk_is_alpha(char ch)
{
    return vsk_is_lower(ch) || vsk_is_upper(ch);
}

inline bool vsk_is_digit(char ch)
{
    return ('0' <= ch && ch <= '9');
}

inline bool vsk_is_alnum(char ch)
{
    return vsk_is_alpha(ch) || vsk_is_digit(ch);
}

inline bool vsk_is_blank(char ch)
{
    return ch == ' ' || ch == '\t';
}

inline std::string vsk_upper(const std::string& str)
{
    std::string ret;
    for (auto& ch : str)
    {
        if (vsk_is_lower(ch))
            ret += ch + ('A' - 'a');
        else
            ret += ch;
    }
    return ret;
}
