// renum.cpp --- Renumber BASIC program lines by katahiromz
// Copyright (C) 2024 Katayama Hirofumi MZ
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
#include "config.h"

// version info
void RENUM_version(void)
{
    std::printf("renum Version 1.2.4 by katahiromz\n");
}

#define RENUM_DEFAULT_OUTPUT "output.bas"
#define UTF8_BOM "\xEF\xBB\xBF"

// how to use
void RENUM_usage(void)
{
    std::printf(
        "\n"
        "RENUM --- Renumber BASIC program lines\n"
        "\n"
        "Usage: renum [OPTIONS] -i your_file.bas -o output.bas\n"
        "\n"
        "Options:\n"
        "  -i FILE                  Specify the input BASIC file to be renumbered.\n"
        "  -o FILE                  Specify the output file (default: %s).\n"
        "  --new-start LINE_NUMBER  Set the new starting line number (default: %d).\n"
        "  --old-start LINE_NUMBER  Set the old starting line number (default: 0).\n"
        "  --step STEP              Set the increment step between lines (default: %d).\n"
        "  --force                  Force renumbering even if any invalid line number.\n"
        "  --help                   Display this help message and exit.\n"
        "  --version                Display version information and exit.\n"
        "\n"
        "For support or further information, please contact: katayama.hirofumi.mz@gmail.com\n",
        RENUM_DEFAULT_OUTPUT, RENUM_LINENO_START, RENUM_LINENO_STEP);
}

typedef std::map<renum_lineno_t, renum_lineno_t> VskLineNoMap;

struct RENUM
{
    std::map<std::string, std::string> m_options;
    renum_lineno_t m_new_start = RENUM_LINENO_START;
    renum_lineno_t m_old_start = 0;
    renum_lineno_t m_step = RENUM_LINENO_STEP;
    bool m_bom = false;
    bool m_force = false;
};

// is it a line number?
inline bool vsk_is_lineno(const std::string& str)
{
    if (str.empty())
        return false;
    for (auto& ch : str)
    {
        if (!vsk_isdigit(ch))
            return false;
    }
    return true;
}

// tokens
enum RENUM_TOKEN
{
#define DEFINE_TOKEN(id, str) id,
#include "renum-tokens.h"
#undef DEFINE_TOKEN
    RT_MAX
};

// convert word to token
RENUM_TOKEN RENUM_word2token(const std::string& word)
{
#define DEFINE_TOKEN(id, str) if (word == str) return id;
#include "renum-tokens.h"
#undef DEFINE_TOKEN
    return RT_MAX;
}

// The tokenizer
struct RENUM_Tokenizer
{
    std::string& m_str;
    size_t m_ich, m_cch;

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
        auto str = m_str.substr(m_ich, m_cch);
        vsk_upper(str);
        return str;
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
            if (!vsk_isblank(ch))
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

        if (vsk_isalpha(ch)) // identifier?
        {
            while (!is_eof())
            {
                ch = getch();
                if (vsk_isalnum(ch) || ch == '.')
                    ret += ch;
                else
                    break;
                next();
            }
            m_ich = ich;
            m_cch = ret.size();
            vsk_upper(ret);
            return ret;
        }
        else if (vsk_isdigit(ch)) // numeric?
        {
            while (!is_eof())
            {
                ch = getch();
                if (vsk_isdigit(ch) || ch == '.')
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

// get the line number
renum_lineno_t RENUM_line_number_from_line_text(const std::string& line, char **endptr)
{
    if (line.empty())
        return 0;

    auto number = std::strtoul(&line[0], endptr, 10);
    if (endptr && **endptr == ' ')
        ++(*endptr);

    return renum_lineno_t(number);
}

// sort by line numbers
void RENUM_sort_by_line_numbers(std::string& text)
{
    // trim the space of right side
    mstr_trim_right(text, " \t\r\n");

    // split to lines
    std::vector<std::string> lines;
    mstr_split(lines, text, "\n");

    std::sort(lines.begin(), lines.end(), [](const std::string& line0, const std::string& line1){
        auto number0 = RENUM_line_number_from_line_text(line0);
        auto number1 = RENUM_line_number_from_line_text(line1);
        return number0 < number1;
    });

    // join the lines
    text = std::move(mstr_join(lines, "\n"));
    text += '\n';
}

// load a text file
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

// save a text file
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

// insert line numbers to each top of lines
renum_error_t
RENUM_add_line_numbers(
    std::string& text,
    renum_lineno_t start,
    renum_lineno_t step,
    bool force)
{
    // trim the space of right side
    mstr_trim_right(text, " \t\r\n");

    // split to lines
    std::vector<std::string> lines;
    mstr_split(lines, text, "\n");

    renum_lineno_t line_no = start;
    for (auto& line : lines)
    {
        // trim the space of right side
        mstr_trim_right(line, " \t\r\n");
        if (line.empty())
            line = "'";

        // get the line number
        auto number = RENUM_line_number_from_line_text(line);
        if (number > 0 && !force)
        {
            RENUM_ERROR_MESSAGE("Line number already exists at " + std::to_string(number) + "\n");
            return 1;
        }

        // add it to the left side
        line = std::to_string(line_no) + " " + line;

        // step up
        line_no += step;
    }

    // join the lines
    text = std::move(mstr_join(lines, "\n"));
    text += '\n';

    return 0;
}

// renumber a line
bool
RENUM_renumber_one_line(
    const VskLineNoMap& old_to_new_line,
    std::string& line,
    renum_lineno_t old_line_no,
    bool force = false)
{
    auto it0 = old_to_new_line.find(old_line_no);
    if (it0 == old_to_new_line.end())
        return force;
    auto new_line_no = it0->second;

    RENUM_Tokenizer tokenizer(line);

    // scan the lien string
    bool went = false, range = false, expect_lineno = false, comment = false, gosub_goto = false;
    bool expect_label = false;
    while (!tokenizer.is_eof() && !comment)
    {
        auto word = tokenizer.get_next_word();

        if (expect_lineno && vsk_is_lineno(word))
        {
            auto number = RENUM_line_number_from_line_text(word);
            if (number > 0) // line number?
            {
                auto it = old_to_new_line.find(number);
                if (it == old_to_new_line.end()) // not found?
                {
                    RENUM_ERROR_MESSAGE("Undefined line " + std::to_string(number) + " in " + std::to_string(old_line_no) + "\n");
                    if (!force)
                        return false;
                }
                else
                {
                    // replace line number text
                    tokenizer.replace_word(std::to_string(it->second));
                }
            }
        }

        bool expected_label = expect_label;
        expect_label = expect_lineno = false;
        auto token = RENUM_word2token(word.c_str());
        switch (token)
        {
        case RT_GO:
            break;
        case RT_TO:
        case RT_SUB:
            if (went)
                expect_lineno = gosub_goto = true;
            break;
        case RT_GOTO:
        case RT_GOSUB:
            expect_lineno = gosub_goto = true;
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
            expect_lineno = range = true;
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
            gosub_goto = range = false;
            break;
        case RT_ASTERISK:
            expect_label = true;
            break;
        case RT_MAX:
            if (!vsk_is_lineno(word) && !expected_label)
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

    // insert new line number
    line = std::to_string(new_line_no) + " " + line;
    return true;
}

renum_error_t
RENUM_renumber_lines(
    std::string& text,
    renum_lineno_t new_start,
    renum_lineno_t old_start,
    renum_lineno_t step,
    bool force)
{
    // trim the space of right side
    mstr_trim_right(text, " \t\r\n");

    // split to lines
    std::vector<std::string> lines;
    mstr_split(lines, text, "\n");

    // create a mapping from old line to new line
    VskLineNoMap old_to_new_line;
    renum_lineno_t new_line_no = new_start;
    size_t iLine = 1;
    for (auto& line : lines)
    {
        // trim the space of right side
        mstr_trim_right(line, " \t\r\n");

        // try to get old line number
        auto old_line_no = RENUM_line_number_from_line_text(line);
        if (old_line_no <= 0) // No line number?
        {
            if (!force)
            {
                RENUM_ERROR_MESSAGE("No line number found at line " + std::to_string(iLine) + "\n");
                return 1;
            }
        }
        else if (old_line_no >= old_start)
        {
            // update the mapping
            old_to_new_line[old_line_no] = new_line_no;

            // step up
            new_line_no += step;
        }
        else
        {
            // update the mapping
            old_to_new_line[old_line_no] = old_line_no;
        }

        ++iLine;
    }

    // renumber lines
    for (auto& line : lines)
    {
        // get line number and remove it
        char *endptr;
        auto old_line_no = RENUM_line_number_from_line_text(line, &endptr);
        line = endptr;

        // renumber one line and add the line number
        if (!RENUM_renumber_one_line(old_to_new_line, line, old_line_no, force))
        {
            return 1;
        }
    }

    // join the lines
    text = std::move(mstr_join(lines, "\n"));
    text += '\n';

    return 0;
}

#ifdef RENUM_EXE

// parse command line
renum_error_t RENUM_parse_cmdline(RENUM& renum, int argc, char **argv)
{
    renum.m_options.clear();
    renum.m_options["-o"] = RENUM_DEFAULT_OUTPUT;

    for (int iarg = 1; iarg < argc; ++iarg)
    {
        std::string arg = argv[iarg];
        if (arg == "--help" || arg == "/?")
        {
            RENUM_usage();
            return -1;
        }
        if (arg == "--version")
        {
            RENUM_version();
            return -1;
        }
        if (arg == "--force")
        {
            renum.m_force = true;
            continue;
        }
        if (arg == "-i" || arg == "-o" ||
            arg == "--old-start" ||
            arg == "--new-start" ||
            arg == "--step")
        {
            if (iarg + 1 < argc)
            {
                ++iarg;
                renum.m_options[arg] = argv[iarg];
                continue;
            }
            else
            {
                std::fprintf(stderr, "renum: error: No operand for argument '%s'\n", arg.c_str());
                return 1;
            }
        }
        std::fprintf(stderr, "renum: error: invalid argument '%s'\n", arg.c_str());
        return 1;
    }

    auto it0 = renum.m_options.find("--new-start");
    if (it0 != renum.m_options.end())
    {
        char *endptr;
        renum.m_new_start = std::strtoul(it0->second.c_str(), &endptr, 10);
        if (*endptr || renum.m_new_start < 0)
        {
            std::fprintf(stderr, "renum: error: --new-start '%s' is not a positive integer\n", it0->first.c_str());
            return 1;
        }
    }

    auto it1 = renum.m_options.find("--old-start");
    if (it1 != renum.m_options.end())
    {
        char *endptr;
        renum.m_old_start = std::strtoul(it1->second.c_str(), &endptr, 10);
        if (*endptr || renum.m_old_start <= 0)
        {
            std::fprintf(stderr, "renum: error: --old-start '%s' is not a positive integer\n", it1->first.c_str());
            return 1;
        }
    }

    auto it2 = renum.m_options.find("--step");
    if (it2 != renum.m_options.end())
    {
        char *endptr;
        renum.m_step = std::strtoul(it2->second.c_str(), &endptr, 10);
        if (*endptr || renum.m_step <= 0)
        {
            std::fprintf(stderr, "renum: error: --step '%s' is not a positive integer\n", it2->first.c_str());
            return 1;
        }
    }

    auto it3 = renum.m_options.find("-i");
    if (it3 == renum.m_options.end())
    {
        std::fprintf(stderr, "renum: error: No input file specified\n");
        RENUM_usage();
        return 1;
    }

    auto it4 = renum.m_options.find("-o");
    if (it4 == renum.m_options.end())
    {
        std::fprintf(stderr, "renum: error: No output file specified\n");
        RENUM_usage();
        return 1;
    }

    return 0;
}

// the main part of this program / library
renum_error_t RENUM_renum(RENUM& renum)
{
    std::string text;
    renum_error_t error = RENUM_load_file(renum.m_options["-i"], text, renum.m_bom);
    if (error)
        return error;

    renum_lineno_t first_lineno = RENUM_line_number_from_line_text(text, nullptr);
    if (first_lineno == 0)
    {
        error = RENUM_add_line_numbers(text, renum.m_new_start, renum.m_step);
        if (error)
            return error;
    }
    else
    {
        RENUM_sort_by_line_numbers(text);

        error = RENUM_renumber_lines(text, renum.m_new_start, renum.m_old_start, renum.m_step, renum.m_force);
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
