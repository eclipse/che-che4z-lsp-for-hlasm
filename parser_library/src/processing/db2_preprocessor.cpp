/*
 * Copyright (c) 2021 Broadcom.
 * The term "Broadcom" refers to Broadcom Inc. and/or its subsidiaries.
 *
 * This program and the accompanying materials are made
 * available under the terms of the Eclipse Public License 2.0
 * which is available at https://www.eclipse.org/legal/epl-2.0/
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Broadcom, Inc. - initial API and implementation
 */

#include <cassert>
#include <charconv>
#include <regex>
#include <utility>

#include "lexing/logical_line.h"
#include "preprocessor.h"
#include "preprocessor_options.h"
#include "workspaces/parse_lib_provider.h"

namespace {
constexpr std::string_view trim_right(std::string_view s)
{
    const auto i = s.find_last_not_of(' ');

    if (i == std::string_view::npos)
        return s;

    return s.substr(0, i + 1);
}
} // namespace

namespace hlasm_plugin::parser_library::processing {
namespace {
class db2_preprocessor : public preprocessor
{
    const char* m_last_position = nullptr;
    lexing::logical_line m_logical_line;
    std::string m_operands;
    library_fetcher m_libs;
    diagnostic_op_consumer* m_diags = nullptr;
    std::string m_buffer;

    static bool remove_space(std::string_view& s)
    {
        if (s.empty() || s.front() != ' ')
            return false;
        const auto non_space = s.find_first_not_of(' ');

        if (non_space == std::string_view::npos)
        {
            s = {};
            return true;
        }

        s.remove_prefix(non_space);
        return true;
    }

    static bool consume(std::string_view& s, std::string_view lit)
    {
        // case sensitive
        if (s.substr(0, lit.size()) != lit)
            return false;
        s.remove_prefix(lit.size());
        return true;
    }

    void push_sql_working_storage()
    {
        m_buffer.append("***$$$ SQL WORKING STORAGE                      \n"
                        "SQLDSIZ  DC    A(SQLDLEN) SQLDSECT SIZE         \n"
                        "SQLDSECT DSECT                                  \n"
                        "SQLTEMP  DS    CL128     TEMPLATE               \n"
                        "DSNTEMP  DS    F         INT SCROLL VALUE       \n"
                        "DSNTMP2  DS    PL16      DEC SCROLL VALUE       \n"
                        "DSNNROWS DS    F         MULTI-ROW N-ROWS VALUE \n"
                        "DSNNTYPE DS    H         MULTI-ROW N-ROWS TYPE  \n"
                        "DSNNLEN  DS    H         MULTI-ROW N-ROWS LENGTH\n"
                        "DSNPARMS DS    4F        DSNHMLTR PARM LIST     \n"
                        "DSNPNM   DS    CL386     PROCEDURE NAME         \n"
                        "DSNCNM   DS    CL128     CURSOR NAME            \n"
                        "SQL_FILE_READ      EQU 2                        \n"
                        "SQL_FILE_CREATE    EQU 8                        \n"
                        "SQL_FILE_OVERWRITE EQU 16                       \n"
                        "SQL_FILE_APPEND    EQU 32                       \n"
                        "         DS    0D                               \n"
                        "SQLPLIST DS    F                                \n"
                        "SQLPLLEN DS    H         PLIST LENGTH           \n"
                        "SQLFLAGS DS    XL2       FLAGS                  \n"
                        "SQLCTYPE DS    H         CALL-TYPE              \n"
                        "SQLPROGN DS    CL8       PROGRAM NAME           \n"
                        "SQLTIMES DS    CL8       TIMESTAMP              \n"
                        "SQLSECTN DS    H         SECTION                \n"
                        "SQLCODEP DS    A         CODE POINTER           \n"
                        "SQLVPARM DS    A         VPARAM POINTER         \n"
                        "SQLAPARM DS    A         AUX PARAM PTR          \n"
                        "SQLSTNM7 DS    H         PRE_V8 STATEMENT NUMBER\n"
                        "SQLSTYPE DS    H         STATEMENT TYPE         \n"
                        "SQLSTNUM DS    F         STATEMENT NUMBER       \n"
                        "SQLFLAG2 DS    H         internal flags         \n"
                        "SQLRSRVD DS    CL18      RESERVED               \n"
                        "SQLPVARS DS    CL8,F,2H,0CL44                   \n"
                        "SQLAVARS DS    CL8,F,2H,0CL44                   \n"
                        "         DS    0D                               \n"
                        "SQLDLEN  EQU   *-SQLDSECT                       \n");
    }

    void inject_SQLCA()
    {
        m_buffer.append("***$$$ SQLCA                          \n"
                        "SQLCA    DS    0F                     \n"
                        "SQLCAID  DS    CL8      ID            \n"
                        "SQLCABC  DS    F        BYTE COUNT    \n"
                        "SQLCODE  DS    F        RETURN CODE   \n"
                        "SQLERRM  DS    H,CL70   ERR MSG PARMS \n"
                        "SQLERRP  DS    CL8      IMPL-DEPENDENT\n"
                        "SQLERRD  DS    6F                     \n"
                        "SQLWARN  DS    0C       WARNING FLAGS \n"
                        "SQLWARN0 DS    C'W' IF ANY            \n"
                        "SQLWARN1 DS    C'W' = WARNING         \n"
                        "SQLWARN2 DS    C'W' = WARNING         \n"
                        "SQLWARN3 DS    C'W' = WARNING         \n"
                        "SQLWARN4 DS    C'W' = WARNING         \n"
                        "SQLWARN5 DS    C'W' = WARNING         \n"
                        "SQLWARN6 DS    C'W' = WARNING         \n"
                        "SQLWARN7 DS    C'W' = WARNING         \n"
                        "SQLEXT   DS    0CL8                   \n"
                        "SQLWARN8 DS    C                      \n"
                        "SQLWARN9 DS    C                      \n"
                        "SQLWARNA DS    C                      \n"
                        "SQLSTATE DS    CL5                    \n"
                        "***$$$\n");
    }
    void inject_SQLDA()
    {
        m_buffer.append("***$$$ SQLDA                                            \n"
                        "SQLTRIPL EQU    C'3'                                    \n"
                        "SQLDOUBL EQU    C'2'                                    \n"
                        "SQLSINGL EQU    C' '                                    \n"
                        "*                                                       \n"
                        "         SQLSECT SAVE                                   \n"
                        "*                                                       \n"
                        "SQLDA    DSECT                                          \n"
                        "SQLDAID  DS    CL8      ID                              \n"
                        "SQLDABC  DS    F        BYTE COUNT                      \n"
                        "SQLN     DS    H        COUNT SQLVAR/SQLVAR2 ENTRIES    \n"
                        "SQLD     DS    H        COUNT VARS (TWICE IF USING BOTH)\n"
                        "*                                                       \n"
                        "SQLVAR   DS    0F       BEGIN VARS                      \n"
                        "SQLVARN  DSECT ,        NTH VARIABLE                    \n"
                        "SQLTYPE  DS    H        DATA TYPE CODE                  \n"
                        "SQLLEN   DS    0H       LENGTH                          \n"
                        "SQLPRCSN DS    X        DEC PRECISION                   \n"
                        "SQLSCALE DS    X        DEC SCALE                       \n"
                        "SQLDATA  DS    A        ADDR OF VAR                     \n"
                        "SQLIND   DS    A        ADDR OF IND                     \n"
                        "SQLNAME  DS    H,CL30   DESCRIBE NAME                   \n"
                        "SQLVSIZ  EQU   *-SQLDATA                                \n"
                        "SQLSIZV  EQU   *-SQLVARN                                \n"
                        "*                                                       \n"
                        "SQLDA    DSECT                                          \n"
                        "SQLVAR2  DS     0F      BEGIN EXTENDED FIELDS OF VARS   \n"
                        "SQLVAR2N DSECT  ,       EXTENDED FIELDS OF NTH VARIABLE \n"
                        "SQLLONGL DS     F       LENGTH                          \n"
                        "SQLRSVDL DS     F       RESERVED                        \n"
                        "SQLDATAL DS     A       ADDR OF LENGTH IN BYTES         \n"
                        "SQLTNAME DS     H,CL30  DESCRIBE NAME                   \n"
                        "*                                                       \n"
                        "         SQLSECT RESTORE                                \n"
                        "***$$$\n");
    }
    void inject_SQLSECT()
    {
        m_buffer.append("         MACRO                          \n"
                        "         SQLSECT &TYPE                  \n"
                        "         GBLC  &SQLSECT                 \n"
                        "         AIF ('&TYPE' EQ 'RESTORE').REST\n"
                        "&SQLSECT SETC  '&SYSECT'                \n"
                        "         MEXIT                          \n"
                        ".REST    ANOP                           \n"
                        "&SQLSECT CSECT                          \n"
                        "         MEND                           \n");
    }

    void process_include(std::string_view operands, size_t lineno)
    {
        if (operands == "SQLCA")
        {
            inject_SQLCA();
            return;
        }
        if (operands == "SQLDA")
        {
            inject_SQLDA();
            return;
        }
        m_buffer.append("***$$$\n");

        std::optional<std::string> include_text;
        if (m_libs)
            include_text = m_libs(operands);
        if (!include_text.has_value())
        {
            if (m_diags)
                m_diags->add_diagnostic(diagnostic_op::error_P0002(range(position(lineno, 0)), operands));
            return;
        }

        std::string_view include = include_text.value();

        while (!include.empty())
        {
            if (fill_buffer(include, lineno, false) > 0)
                continue;
            while (true)
            {
                const auto text = lexing::extract_line(include).first;
                m_buffer.append(text);
                m_buffer.append("\n");
                if (text.size() <= lexing::default_ictl_copy.continuation
                    || text[lexing::default_ictl_copy.continuation] == ' ')
                    break;
            }
        }
    }
    static bool consume_words(
        std::string_view& l, std::initializer_list<std::string_view> words, bool tolerate_no_space_at_end = false)
    {
        const auto init_l = l;
        for (const auto& w : words)
        {
            if (!consume(l, w))
            {
                l = init_l; // all or nothing
                return false;
            }
            if (!remove_space(l))
            {
                if (tolerate_no_space_at_end && l.empty() && &w == words.end() - 1)
                    return true;
                l = init_l; // all or nothing
                return false;
            }
        }
        return true;
    }

    static bool is_end(std::string_view s)
    {
        if (!consume(s, "END"))
            return false;

        if (s.empty() || s.front() == ' ')
            return true;

        return false;
    }

    static std::string_view create_line_preview(std::string_view input)
    {
        const auto begin_offset = lexing::default_ictl.begin - 1;
        using namespace std::literals;
        if (input.size() < begin_offset)
            return {};

        input = input.substr(begin_offset, lexing::default_ictl.end - begin_offset);

        if (const auto rn = input.find_first_of("\r\n"sv); rn != std::string_view::npos)
            input = input.substr(0, rn);

        return input;
    }

    static bool ignore_line(std::string_view s) { return s.empty() || s.front() == '*' || s.substr(0, 2) == ".*"; }

    static std::string_view extract_label(std::string_view& s)
    {
        if (s.empty() || s.front() == ' ')
            return {};

        auto space = s.find(' ');
        if (space == std::string_view::npos)
            space = s.size();

        std::string_view result = s.substr(0, space);

        s.remove_prefix(space);

        return result;
    }

    enum class line_type
    {
        ignore,
        exec_sql,
        include,
        sql_type
    };

    static line_type consume_instruction(std::string_view& line_preview)
    {
        if (line_preview.empty())
            return line_type::ignore;

        switch (line_preview.front())
        {
            case 'E':
                if (consume_words(line_preview, { "EXEC", "SQL" }))
                {
                    if (consume_words(line_preview, { "INCLUDE" }))
                        return line_type::include;
                    else
                        return line_type::exec_sql;
                }
                return line_type::ignore;

            case 'S':
                if (consume_words(line_preview, { "SQL", "TYPE", "IS" }))
                    return line_type::sql_type;
                return line_type::ignore;

            default:
                return line_type::ignore;
        }
    }

    void add_ds_line(std::string_view label, std::string_view label_suffix, std::string_view type, bool align = true)
    {
        m_buffer.append(label)
            .append(label_suffix)
            .append(align && label.size() + label_suffix.size() < 8 ? 8 - (label.size() + label_suffix.size()) : 0, ' ')
            .append(" DS ")
            .append(align ? 2 + (type.front() != '0') : 0, ' ')
            .append(type)
            .append("\n");
    };

    struct lob_info_t
    {
        unsigned long long scale;
        unsigned long long limit;
        std::string prefix;
    };

    static lob_info_t lob_info(char type, char scale)
    {
        lob_info_t result;
        switch (scale)
        {
            case 'K':
                result.scale = 1024ULL;
                break;
            case 'M':
                result.scale = 1024ULL * 1024;
                break;
            case 'G':
                result.scale = 1024ULL * 1024 * 1024;
                break;
            default:
                result.scale = 1ULL;
                break;
        }
        switch (type)
        {
            case 'B':
                result.limit = 65535;
                result.prefix = "CL";
                break;
            case 'C':
                result.limit = 65535;
                result.prefix = "CL";
                break;
            case 'D':
                result.limit = 65534;
                result.prefix = "GL";
                break;
        }
        return result;
    }

    bool handle_lob(const std::regex& pattern, std::string_view label, std::string_view operands)
    {
        std::match_results<std::string_view::const_iterator> match;
        if (!std::regex_match(operands.cbegin(), operands.cend(), match, pattern))
            return false;

        switch (match[1].second[-1])
        {
            case 'E': // ..._FILE
                add_ds_line(label, "", "0FL4");
                add_ds_line(label, "_NAME_LENGTH", "FL4", false);
                add_ds_line(label, "_DATA_LENGTH", "FL4", false);
                add_ds_line(label, "_FILE_OPTIONS", "FL4", false);
                add_ds_line(label, "_NAME", "CL255", false);
                break;

            case 'R': // ..._LOCATOR
                add_ds_line(label, "", "FL4");
                break;

            default: {
                const auto li = lob_info(*match[1].first, match[3].matched ? *match[3].first : 0);
                unsigned long long len;
                std::from_chars(&*match[2].first, &*match[2].first + match[2].length(), len);
                len *= li.scale;

                add_ds_line(label, "", "0FL4");
                add_ds_line(label, "_LENGTH", "FL4", false);
                add_ds_line(label, "_DATA", li.prefix + std::to_string(len <= li.limit ? len : li.limit), false);
                if (len > li.limit)
                    m_buffer
                        .append(" ORG   *+(")
                        // there seems be this strage artifical limit
                        .append(std::to_string(std::min(len - li.limit, 1073676289ull)))
                        .append(")\n");
                break;
            }
        }
        return true;
    };

    bool process_sql_type_operands(std::string_view operands, std::string_view label)
    {
        if (operands.size() < 2)
            return false;

        // keep the capture groups in sync
        static const auto xml_type = std::regex(
            "XML[ ]+AS[ ]+"
            "(?:"
            "(BINARY[ ]+LARGE[ ]+OBJECT|BLOB|CHARACTER[ ]+LARGE[ ]+OBJECT|CHAR[ ]+LARGE[ ]+OBJECT|CLOB|DBCLOB)"
            "[ ]+([[:digit:]]{1,9})([KMG])?"
            "|"
            "(BLOB_FILE|CLOB_FILE|DBCLOB_FILE)"
            ")"
            "(?: .*)?");
        static const auto lob_type = std::regex(
            "(?:"
            "(BINARY[ ]+LARGE[ ]+OBJECT|BLOB|CHARACTER[ ]+LARGE[ ]+OBJECT|CHAR[ ]+LARGE[ ]+OBJECT|CLOB|DBCLOB)"
            "[ ]+([[:digit:]]{1,9})([KMG])?"
            "|"
            "(BLOB_FILE|CLOB_FILE|DBCLOB_FILE|BLOB_LOCATOR|CLOB_LOCATOR|DBCLOB_LOCATOR)"
            ")"
            "(?: .*)?");

        static const auto table_like =
            std::regex("TABLE[ ]+LIKE[ ]+('(?:[^']|'')+'|(?:[^']|'')+)[ ]+AS[ ]+LOCATOR(?: .*)?");

        switch (operands[0])
        {
            case 'R':
                switch (operands[1])
                {
                    case 'E':
                        if (!consume_words(operands, { "RESULT_SET_LOCATOR", "VARYING" }, true))
                            break;
                        add_ds_line(label, "", "FL4");
                        return true;

                    case 'O':
                        if (!consume_words(operands, { "ROWID" }, true))
                            break;
                        add_ds_line(label, "", "H,CL40");
                        return true;
                }
                break;

            case 'T':
                if (!std::regex_match(operands.begin(), operands.end(), table_like))
                    break;
                add_ds_line(label, "", "FL4");
                return true;

            case 'X':
                return handle_lob(xml_type, label, operands);

            case 'B':
            case 'C':
            case 'D':
                return handle_lob(lob_type, label, operands);
        }
        return false;
    }

    void process_regular_line(std::string_view label, size_t first_line_skipped)
    {
        if (!label.empty())
            m_buffer.append(label).append(" DS 0H\n");

        m_buffer.append("***$$$\n");

        for (const auto& segment : m_logical_line.segments)
        {
            m_buffer.append(segment.line);

            auto operand_part = segment.code;
            if (first_line_skipped)
            {
                const auto appended_line_size = segment.line.size();
                operand_part.remove_prefix(first_line_skipped);
                if (!label.empty())
                    m_buffer.replace(m_buffer.size() - appended_line_size,
                        label.size(),
                        label.size(),
                        ' '); // mask out any label-like characters
                m_buffer[m_buffer.size() - appended_line_size] = '*';

                first_line_skipped = 0;
            }
            m_buffer.append("\n");
            m_operands.append(operand_part);
        }
    }

    void process_sql_type_line(size_t first_line_skipped)
    {
        m_buffer.append("***$$$\n");
        m_buffer.append("*")
            .append(m_logical_line.segments.front().code.substr(0, lexing::default_ictl.end - 1))
            .append("\n");

        for (const auto& segment : m_logical_line.segments)
        {
            m_operands.append(segment.code.substr(first_line_skipped));
            first_line_skipped = 0;
        }

        m_buffer.append("***$$$\n");
    }

    /* returns number of consumed lines */
    size_t fill_buffer(std::string_view& input, size_t lineno, bool include_allowed)
    {
        using namespace std::literals;

        std::string_view line_preview = create_line_preview(input);

        if (ignore_line(line_preview))
            return 0;

        size_t first_line_skipped = line_preview.size();
        std::string_view label = extract_label(line_preview);

        if (!remove_space(line_preview))
            return 0;

        if (is_end(line_preview))
        {
            push_sql_working_storage();

            return 0;
        }

        auto instruction = consume_instruction(line_preview);
        if (instruction == line_type::ignore)
            return 0;

        if (!line_preview.empty())
            first_line_skipped = line_preview.data() - input.data();

        // now we have a valid line

        m_operands.clear();
        m_logical_line.clear();

        bool extracted = lexing::extract_logical_line(m_logical_line, input, lexing::default_ictl);
        assert(extracted);

        if (m_logical_line.continuation_error && m_diags)
            m_diags->add_diagnostic(diagnostic_op::error_P0001(range(position(lineno, 0))));

        switch (instruction)
        {
            case line_type::exec_sql:
                process_regular_line(label, first_line_skipped);
                if (sql_has_codegen(m_operands))
                    generate_sql_code_mock();
                m_buffer.append("***$$$\n");
                break;

            case line_type::include:
                process_regular_line(label, first_line_skipped);

                if (std::string_view operands = trim_right(m_operands); include_allowed)
                    process_include(operands, lineno);
                else if (m_diags)
                    m_diags->add_diagnostic(diagnostic_op::error_P0003(range(position(lineno, 0)), operands));
                break;

            case line_type::sql_type:
                process_sql_type_line(first_line_skipped);
                // DB2 preprocessor exhibits strange behavior when SQL TYPE line is continued
                if (m_logical_line.segments.size() > 1 && m_diags)
                    m_diags->add_diagnostic(diagnostic_op::error_P0005(range(position(lineno, 0))));
                if (label.empty())
                    label = " "; // best matches the observed behavior
                if (!process_sql_type_operands(m_operands, label) && m_diags)
                    m_diags->add_diagnostic(diagnostic_op::error_P0004(range(position(lineno, 0))));
                break;
        }

        return m_logical_line.segments.size();
    }

    static bool sql_has_codegen(std::string_view sql)
    {
        // handles only the most obvious cases (imprecisely)
        static const auto no_code_statements = std::regex("(?:DECLARE|WHENEVER)(?: .*)?", std::regex_constants::icase);
        return !std::regex_match(sql.begin(), sql.end(), no_code_statements);
    }
    void generate_sql_code_mock()
    {
        // this function generates non-realistic sql statement replacement code, because people do strange things...
        m_buffer.append("         LA    15,SQLCA      \n"
                        "         L     15,=V(DSNHLI) \n"
                        "         BALR  14,15         \n");
    }

    // Inherited via preprocessor
    std::optional<std::string> generate_replacement(std::string_view& input, size_t& lineno) override
    {
        if (input.data() == m_last_position)
            return std::nullopt;

        m_buffer.clear();
        if (std::exchange(m_last_position, input.data()) == nullptr)
        {
            // injected right after ICTL or *PROCESS
            inject_SQLSECT();
        }

        lineno += fill_buffer(input, lineno, true);
        if (m_buffer.size())
            return m_buffer;
        else
            return std::nullopt;
    }

public:
    db2_preprocessor(library_fetcher libs, diagnostic_op_consumer* diags)
        : m_libs(std::move(libs))
        , m_diags(diags)
    {}
};
} // namespace

std::unique_ptr<preprocessor> preprocessor::create(
    const db2_preprocessor_options&, library_fetcher libs, diagnostic_op_consumer* diags)
{
    return std::make_unique<db2_preprocessor>(std::move(libs), diags);
}
} // namespace hlasm_plugin::parser_library::processing
