// renum.cpp --- Renumber BASIC program lines
// License: MIT
#include "renum.h"
#include <vector>
#include <map>
#include <algorithm>
#include <cstring>
#include <cstdio>
#include <cassert>
#include "mstr.h"
#include "encoding.h"

void RENUM_version(void)
{
    std::printf("renum Version 1.0 by katahiromz\n");
}

void RENUM_usage(void)
{
    std::printf(
        "RENUM --- Renumber BASIC program lines\n"
        "\n"
        "Usage: renum [OPTIONS] -i your_file.bas -o output.bas\n"
        "\n"
        "Options:\n"
        "  -i FILE                  The input file\n"
        "  -o FILE                  The output file (default: output.bas)\n"
        "  --start LINE_NUMBER      The starting line number (default: 10)\n"
        "  --step  LINE_NUMBER      The step between line numbers (default: 10)\n"
        "  --help                   Display this message\n"
        "  --version                Display version information\n"
        "\n"
        "Contact: katayama.hirofumi.mz@gmail.com\n"
    );
}

#define UTF8_BOM "\xEF\xBB\xBF"

typedef std::map<renum_lineno_t, renum_lineno_t> VskLineNoMap;

struct RENUM
{
    std::map<std::string, std::string> m_options;
    renum_lineno_t m_start_lineno = 10;
    renum_lineno_t m_step = 10;
    bool m_bom = false;
};

inline bool vsk_is_lineno(const std::string& str)
{
    if (str.empty())
        return false;
    for (auto& ch : str)
    {
        if (!vsk_is_digit(ch))
            return false;
    }
    return true;
}

enum RENUM_TOKEN
{
#define DEFINE_TOKEN(id, str) id,
#include "tokens.h"
#undef DEFINE_TOKEN
    RT_MAX
};

RENUM_TOKEN RENUM_word2token(const char *token)
{
#define DEFINE_TOKEN(id, str) if (std::strcmp(token, str) == 0) return id;
#include "tokens.h"
#undef DEFINE_TOKEN
    return RT_MAX;
}

struct RENUM_Tokenizer
{
    std::string& m_str;
    size_t m_ich;
    size_t m_cch;

    RENUM_Tokenizer(std::string& str) : m_str(str)
    {
        reset();
    }

    void reset()
    {
        m_ich = m_cch = 0;
    }

    bool is_eof() const
    {
        return (m_ich >= m_str.size());
    }

    char getch()
    {
        assert(!is_eof());
        return m_str[m_ich];
    }

    void next()
    {
        assert(!is_eof());
        ++m_ich;
    }

    std::string get_word()
    {
        if (m_cch == 0)
            return get_next_word();
        return vsk_upper(m_str.substr(m_ich, m_cch));
    }

    void replace_word(const std::string& new_word)
    {
        auto old_word = get_word();
        m_str.replace(m_ich, old_word.size(), new_word);
        m_cch = new_word.size();
    }

    void skip_blank()
    {
        while (!is_eof())
        {
            char ch = getch();
            if (!vsk_is_blank(ch))
                break;
            next();
        }
    }

    std::string get_next_word()
    {
        m_ich += m_cch;
        m_cch = 0;

        if (is_eof()) return "";

        skip_blank();

        if (is_eof()) return "";

        std::string ret;

        size_t ich = m_ich;
        char ch = getch();
        ret += ch;
        next();

        if (vsk_is_alpha(ch)) // identifier?
        {
            while (!is_eof())
            {
                ch = getch();
                if (vsk_is_alnum(ch) || ch == '.')
                    ret += ch;
                else
                    break;
                next();
            }
            m_ich = ich;
            m_cch = ret.size();
            return vsk_upper(ret);
        }
        else if (vsk_is_digit(ch)) // numeric?
        {
            while (!is_eof())
            {
                ch = getch();
                if (vsk_is_digit(ch) || ch == '.')
                    ret += ch;
                else
                    break;
                next();
            }
            m_ich = ich;
            m_cch = ret.size();
            return ret;
        }
        else if (ch == '"') // quote?
        {
            while (!is_eof())
            {
                ch = getch();
                ret += ch;
                if (ch == '"')
                    break;
                next();
            }
            m_ich = ich;
            m_cch = ret.size();
            return ret;
        }
        else
        {
            m_ich = ich;
            m_cch = 1;
            return ret;
        }
    }
};

void RENUM_tokenizer_tests(void)
{
    std::string str, word;
    {
        str = "' GOTO sample";
        RENUM_Tokenizer tokenizer(str);
        word = tokenizer.get_next_word();
        assert(word == "'");
    }

    {
        str = "PRINT \"How do you do?\"";
        RENUM_Tokenizer tokenizer(str);
        word = tokenizer.get_next_word();
        assert(word == "PRINT");
        word = tokenizer.get_next_word();
        assert(word == "\"How do you do?\"");
        word = tokenizer.get_next_word();
        assert(word == "");
    }
    {
        str = "GOTO 110";
        RENUM_Tokenizer tokenizer(str);
        word = tokenizer.get_next_word();
        assert(word == "GOTO");
        word = tokenizer.get_next_word();
        assert(word == "110");
    }
    {
        str = "ON MD GOTO 170,180,190";
        RENUM_Tokenizer tokenizer(str);
        word = tokenizer.get_next_word();
        assert(word == "ON");
        word = tokenizer.get_next_word();
        assert(word == "MD");
        word = tokenizer.get_next_word();
        assert(word == "GOTO");
        word = tokenizer.get_next_word();
        assert(word == "170");
        word = tokenizer.get_next_word();
        assert(word == ",");
        word = tokenizer.get_next_word();
        assert(word == "180");
        word = tokenizer.get_next_word();
        assert(word == ",");
        word = tokenizer.get_next_word();
        assert(word == "190");
    }
    {
        str = "PRINT \"TEST\", 130";
        RENUM_Tokenizer tokenizer(str);
        word = tokenizer.get_next_word();
        assert(word == "PRINT");
        word = tokenizer.get_next_word();
        assert(word == "\"TEST\"");
        word = tokenizer.get_next_word();
        assert(word == ",");
        word = tokenizer.get_next_word();
        assert(word == "130");
    }
}

renum_lineno_t vsk_line_number_from_line_text(const std::string& line, char **endptr = nullptr)
{
    if (line.empty())
        return 0;

    auto number = std::strtoul(&line[0], endptr, 10);
    if (endptr && **endptr == ' ')
        ++(*endptr);

    return renum_lineno_t(number);
}

void vsk_sort_by_line_numbers(std::string& text)
{
    mstr_trim_right(text, " \t\r\n");

    std::vector<std::string> lines;
    mstr_split(lines, text, "\n");

    std::sort(lines.begin(), lines.end(), [](const std::string& line0, const std::string& line1){
        auto number0 = vsk_line_number_from_line_text(line0);
        auto number1 = vsk_line_number_from_line_text(line1);
        return number0 < number1;
    });

    text = std::move(mstr_join(lines, "\n"));
    text += '\n';
}

renum_error_t RENUM_load_file(const std::string& filename, std::string& text, bool& bom)
{
    text.clear();

    FILE *fin = fopen(filename.c_str(), "r");
    if (!fin)
    {
        std::fprintf(stderr, "renum: error: Unable to open file '%s'\n", filename.c_str());
        return 1;
    }

    char buf[512];
    if (std::fgets(buf, sizeof(buf), fin))
    {
        if (std::memcmp(buf, UTF8_BOM, 3) == 0)
        {
            bom = true;
            text += &buf[3];
        }
        else
        {
            text += buf;
        }

        while (std::fgets(buf, sizeof(buf), fin))
        {
            text += buf;
        }
    }

    std::fclose(fin);
    return 0;
}

renum_error_t RENUM_save_file(const std::string& filename, const std::string& text, bool bom)
{
    FILE *fout = fopen(filename.c_str(), "w");
    if (!fout)
    {
        std::fprintf(stderr, "renum: error: Unable to open file '%s'\n", filename.c_str());
        return 1;
    }

    if (bom)
        std::fwrite(UTF8_BOM, 3, 1, fout);

    if (!std::fwrite(text.c_str(), text.size(), 1, fout))
    {
        std::fprintf(stderr, "renum: error: Unable to write file '%s'\n", filename.c_str());
        std::fclose(fout);
        return 1;
    }

    std::fclose(fout);
    return 0;
}

renum_error_t RENUM_add_line_numbers(std::string& text, renum_lineno_t start = 10, renum_lineno_t step = 10)
{
    mstr_trim_right(text, " \t\r\n");

    std::vector<std::string> lines;
    mstr_split(lines, text, "\n");

    renum_lineno_t line_no = start;
    for (auto& line : lines)
    {
        mstr_trim_right(line, " \t\r\n");
        if (line.empty())
            line = "'";

        auto number = vsk_line_number_from_line_text(line);
        if (number)
        {
            std::fprintf(stderr, "renum: error: Line number already exists at %ld\n", number);
            return 1;
        }

        line = std::to_string(line_no) + " " + line;
        line_no += step;
    }

    text = std::move(mstr_join(lines, "\n"));
    text += '\n';
    return 0;
}

bool RENUM_renumber_one_line(VskLineNoMap& old_to_new_line, std::string& line, renum_lineno_t old_line_no)
{
    auto new_line_no = old_to_new_line[old_line_no];

    RENUM_Tokenizer tokenizer(line);

    bool went = false, range = false, expect_lineno = false, comment = false, gosub_goto = false;
    while (!tokenizer.is_eof() && !comment)
    {
        auto word = tokenizer.get_next_word();

        if (expect_lineno && vsk_is_lineno(word))
        {
            auto number = vsk_line_number_from_line_text(word);
            auto it = old_to_new_line.find(number);
            if (it == old_to_new_line.end())
                return false;

            tokenizer.replace_word(std::to_string(it->second));
        }

        expect_lineno = false;
        auto token = RENUM_word2token(word.c_str());
        switch (token)
        {
        case RT_GO:
            break;
        case RT_TO:
        case RT_SUB:
            if (went)
            {
                expect_lineno = true;
                gosub_goto = true;
            }
            break;
        case RT_GOTO:
        case RT_GOSUB:
            expect_lineno = true;
            gosub_goto = true;
            break;
        case RT_RESUME:
        case RT_EDIT:
        case RT_RUN:
        case RT_RESTORE:
        case RT_RETURN:
        case RT_AUTO:
        case RT_THEN:
        case RT_ELSE:
            expect_lineno = true;
            gosub_goto = false;
            break;
        case RT_DELETE:
        case RT_LIST:
        case RT_LLIST:
            expect_lineno = true;
            range = true;
            gosub_goto = false;
            break;
        case RT_MINUS:
            if (range)
                expect_lineno = true;
            break;
        case RT_COMMENT:
        case RT_REM:
            comment = true;
            gosub_goto = false;
            break;
        case RT_COMMA:
            if (gosub_goto)
                expect_lineno = true;
            break;
        case RT_COLON:
            gosub_goto = false;
            range = false;
            break;
        case RT_MAX:
            if (!vsk_is_lineno(word))
                gosub_goto = false;
            break;
        }

        went = (token == RT_GO);

        if (range)
        {
            if (token != RT_MINUS && !vsk_is_lineno(word))
                range = false;
        }
    }

    line = std::to_string(new_line_no) + " " + line;
    return true;
}

renum_error_t RENUM_renumber_lines(std::string& text, renum_lineno_t start, renum_lineno_t step)
{
    mstr_trim_right(text, " \t\r\n");

    std::vector<std::string> lines;
    mstr_split(lines, text, "\n");

    renum_lineno_t new_line_no = start;
    VskLineNoMap old_to_new_line;
    for (auto& line : lines)
    {
        mstr_trim_right(line, " \t\r\n");
        auto old_line_no = vsk_line_number_from_line_text(line);
        if (old_line_no <= 0)
        {
            std::fprintf(stderr, "renum: error: Invalid line number\n");
            return 1;
        }
        old_to_new_line[old_line_no] = new_line_no;

        new_line_no += step;
    }

    for (auto& line : lines)
    {
        char *endptr;
        auto old_line_no = vsk_line_number_from_line_text(line, &endptr);
        line = endptr;

        if (!RENUM_renumber_one_line(old_to_new_line, line, old_line_no))
        {
            std::fprintf(stderr, "renum: error: Invalid line number at %ld\n", old_line_no);
            return 1;
        }
    }

    text = std::move(mstr_join(lines, "\n"));
    text += '\n';
    return 0;
}

#ifdef RENUM_EXE

renum_error_t RENUM_parse_cmdline(RENUM& renum, int argc, char **argv)
{
    renum.m_start_lineno = 10;
    renum.m_step = 10;

    renum.m_options.clear();
    renum.m_options["-o"] = "output.bas";

    for (int iarg = 1; iarg < argc; ++iarg)
    {
        auto arg = argv[iarg];
        if (std::strcmp(arg, "--help") == 0 || std::strcmp(arg, "/?") == 0)
        {
            RENUM_usage();
            return -1;
        }
        if (std::strcmp(arg, "--version") == 0)
        {
            RENUM_version();
            return -1;
        }
        if (std::strcmp(arg, "-i") == 0 ||
            std::strcmp(arg, "-o") == 0 ||
            std::strcmp(arg, "--start") == 0 ||
            std::strcmp(arg, "--step") == 0)
        {
            if (iarg + 1 < argc)
            {
                ++iarg;
                renum.m_options[arg] = argv[iarg];
                continue;
            }
            else
            {
                std::fprintf(stderr, "renum: error: No operand for argument '&s'\n", arg);
                return 1;
            }
        }
        std::fprintf(stderr, "renum: error: invalid argument '%s'\n", arg);
        return 1;
    }

    auto it0 = renum.m_options.find("--start");
    if (it0 != renum.m_options.end())
    {
        char *endptr;
        renum.m_start_lineno = std::strtoul(it0->second.c_str(), &endptr, 10);
        if (*endptr || renum.m_start_lineno <= 0)
        {
            std::fprintf(stderr, "renum: error: start_lineno '%s' is not a positive integer\n", it0->first.c_str());
            return 1;
        }
    }

    auto it1 = renum.m_options.find("--step");
    if (it1 != renum.m_options.end())
    {
        char *endptr;
        renum.m_step = std::strtoul(it1->second.c_str(), &endptr, 10);
        if (*endptr || renum.m_step <= 0)
        {
            std::fprintf(stderr, "renum: error: step '%s' is not a positive integer\n", it1->first.c_str());
            return 1;
        }
    }

    auto it2 = renum.m_options.find("-i");
    if (it2 == renum.m_options.end())
    {
        std::fprintf(stderr, "renum: error: No input file specified\n");
        RENUM_usage();
        return 1;
    }

    auto it3 = renum.m_options.find("-o");
    if (it3 == renum.m_options.end())
    {
        std::fprintf(stderr, "renum: error: No output file specified\n");
        RENUM_usage();
        return 1;
    }

    return 0;
}

renum_error_t RENUM_renum(RENUM& renum)
{
    std::string text;
    renum_error_t error = RENUM_load_file(renum.m_options["-i"], text, renum.m_bom);
    if (error)
        return error;

    vsk_sort_by_line_numbers(text);

    renum_lineno_t first_lineno = vsk_line_number_from_line_text(text, nullptr);
    if (first_lineno == 0)
    {
        error = RENUM_add_line_numbers(text, renum.m_start_lineno, renum.m_step);
        if (error)
            return error;
    }
    else
    {
        error = RENUM_renumber_lines(text, renum.m_start_lineno, renum.m_step);
        if (error)
            return error;
    }

    error = RENUM_save_file(renum.m_options["-o"], text, renum.m_bom);
    return error;
}

int RENUM_main(int argc, char **argv)
{
    if (argc <= 1)
    {
        std::fprintf(stderr, "renum: error: No arguments specified\n");
        RENUM_usage();
        return 1;
    }

    RENUM renum;
    renum_error_t error = RENUM_parse_cmdline(renum, argc, argv);
    if (error == -1)
        return 0;
    if (error == 1)
        return 1;

    return RENUM_renum(renum);
}

int main(int argc, char **argv)
{
#ifndef NDEBUG
    RENUM_tokenizer_tests();
#endif
    return RENUM_main(argc, argv);
}

#endif  // def RENUM_EXE
