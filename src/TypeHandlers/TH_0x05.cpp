/*
 * Part of tivars_lib_cpp
 * (C) 2015-2017 Adrien 'Adriweb' Bertrand
 * https://github.com/adriweb/tivars_lib_cpp
 * License: MIT
 */

#include "TypeHandlers.h"
#include "../utils.h"

using namespace std;

namespace tivars
{
    namespace TH_0x05
    {
        std::unordered_map<uint, std::vector<std::string>> tokens_BytesToName;
        std::unordered_map<std::string, uint> tokens_NameToBytes;
        uchar lengthOfLongestTokenName;
        std::vector<uchar> firstByteOfTwoByteTokens;
        const uint16_t squishedASMTokens[] = { 0xBB6D, 0xEF69, 0xEF7B }; // 83+/84+, 84+CSE, CE
    }

    /* TODO: handle TI-Innovator Send( exception for in-strings tokenization (=> not shortest tokens) */
    data_t TH_0x05::makeDataFromString(const string& str, const options_t& options)
    {
        (void)options;
        data_t data;

        // two bytes reserved for the size. Filled later
        data.push_back(0); data.push_back(0);

        const uchar maxTokSearchLen = min((uchar)str.length(), lengthOfLongestTokenName);

        bool isWithinString = false;

        for (uint strCursorPos = 0; strCursorPos < str.length(); strCursorPos++)
        {
            const string currChar = str.substr(strCursorPos, 1);
            if (currChar == "\"")
            {
                isWithinString = !isWithinString;
            } else if (currChar == "\n" || currChar == "→")
            {
                isWithinString = false;
            }
            /* isWithinString => minimum token length, otherwise maximal munch */
            for (uint currentLength = isWithinString ? 1 : maxTokSearchLen;
                 isWithinString ? (currentLength <= maxTokSearchLen) : (currentLength > 0);
                 currentLength += (isWithinString ? 1 : -1))
            {
                string currentSubString = str.substr(strCursorPos, currentLength);
                if (tokens_NameToBytes.count(currentSubString))
                {
                    uint tokenValue = tokens_NameToBytes[currentSubString];
                    if (tokenValue > 0xFF)
                    {
                        data.push_back((uchar)(tokenValue >> 8));
                    }
                    data.push_back((uchar)(tokenValue & 0xFF));
                    strCursorPos += currentLength - 1;
                    break;
                }
            }
        }

        uint actualDataLen = (uint) (data.size() - 2);
        data[0] = (uchar)(actualDataLen & 0xFF);
        data[1] = (uchar)((actualDataLen >> 8) & 0xFF);
        return data;
    }

    string TH_0x05::makeStringFromData(const data_t& data, const options_t& options)
    {
        if (data.size() < 2)
        {
            throw invalid_argument("Empty data array. Needs to contain at least 2 bytes (size fields)");
        }

        uint langIdx = (uint)((has_option(options, "lang") && options.at("lang") == LANG_FR) ? LANG_FR : LANG_EN);

        const int howManyBytes = (data[0] & 0xFF) + ((data[1] & 0xFF) << 8);
        if (howManyBytes != (int)data.size() - 2)
        {
            cerr << "[Warning] Byte count (" << (data.size() - 2) << ") and size field (" << howManyBytes  << ") mismatch!";
        }

        const uint16_t twoFirstBytes = (uint16_t) ((data[3] & 0xFF) + ((data[2] & 0xFF) << 8));
        if (find(begin(squishedASMTokens), end(squishedASMTokens), twoFirstBytes) != end(squishedASMTokens))
        {
            return "[Error] This is a squished ASM program - cannnot preview it!";
        }

        uint errCount = 0;
        string str("");
        const size_t dataSize = data.size();
        for (uint i = 2; i < (uint)howManyBytes + 2; i++)
        {
            uint currentToken = data[i];
            uint nextToken = (i < dataSize-1) ? data[i+1] : (uint)-1;
            uint bytesKey = currentToken;
            if (is_in_vector(firstByteOfTwoByteTokens, (uchar)currentToken))
            {
                if (nextToken == (uint)-1)
                {
                    cerr << "[Warning] Encountered an unfinished two-byte token! Setting the second byte to 0x00";
                    nextToken = 0x00;
                }
                bytesKey = nextToken + (currentToken << 8);
                i++;
            }
            if (tokens_BytesToName.find(bytesKey) != tokens_BytesToName.end())
            {
                str += tokens_BytesToName[bytesKey][langIdx];
            } else {
                str += " [???] ";
                errCount++;
            }
        }

        if (errCount > 0)
        {
            cerr << "[Warning] " << errCount << " token(s) could not be detokenized (' [???] ' was used)!";
        }

        if (has_option(options, "prettify") && options.at("prettify") == 1)
        {
            str = regex_replace(str, regex("\\[?\\|?([a-z]+)\\]?"), "$1");
        }

        if (has_option(options, "reindent") && options.at("reindent") == 1)
        {
            str = reindentCodeString(str);
        }

        return str;
    }

    string TH_0x05::reindentCodeString(const string& str_orig, const options_t& options)
    {
        int lang;
        if (has_option(options, "lang"))
        {
            lang = options.at("lang");
        } else {
            lang = (str_orig.size() > 1 && str_orig[0] == '.' && ::isalpha(str_orig[1])) ? PRGMLANG_AXE : PRGMLANG_BASIC;
        }

        string str(str_orig);

        regex eolRegex("\"[^→\"\\n]+[→\"\\n]|(\\:)|(^\"[^→]*$)");
        string output_text;
        sregex_token_iterator begin(str.begin(), str.end(), eolRegex, {-1, 0});
        sregex_token_iterator end;
        for_each(begin, end, [&](const string& m) { output_text += (m == ":") ? "\n" : m; });
        str = output_text;

        str = regex_replace(str, regex("([^\\s])(Del|Eff)Var "), "$1\n$2Var");

        vector<string> lines_tmp = explode(str, '\n');
        vector<pair<uint, string>> lines; // indent, text
        for (uint i=0; i<lines_tmp.size(); i++)
        {
            lines.push_back(make_pair(0, lines_tmp[i]));
        }

        vector<string> increaseIndentAfter   = { "If", "For", "While", "Repeat" };
        vector<string> decreaseIndentOfToken = { "Then", "Else", "End", "ElseIf", "EndIf", "End!If" };
        vector<string> closingTokens         = { "End", "EndIf", "End!If" };
        uint nextIndent = 0;
        string oldFirstCommand = "", firstCommand = "";
        for (uint key=0; key<lines.size(); key++)
        {
            auto lineData = lines[key];
            oldFirstCommand = firstCommand;

            string trimmedLine = trim(lineData.second);
            if (trimmedLine.length() > 0) {
                char* trimmedLine_c = (char*) trimmedLine.c_str();
                firstCommand = strtok(trimmedLine_c, " ");
                firstCommand = trim(firstCommand);
                trimmedLine = string(trimmedLine_c);
                trimmedLine_c = (char*) trimmedLine.c_str();
                if (firstCommand == trimmedLine)
                {
                    firstCommand = strtok(trimmedLine_c, "(");
                    firstCommand = trim(firstCommand);
                }
            } else {
                firstCommand = "";
            }

            lines[key].first = nextIndent;

            if (is_in_vector(increaseIndentAfter, firstCommand))
            {
                nextIndent++;
            }
            if (lines[key].first > 0 && is_in_vector(decreaseIndentOfToken, firstCommand))
            {
                lines[key].first--;
            }
            if (nextIndent > 0 && (is_in_vector(closingTokens, firstCommand) || (oldFirstCommand == "If" && firstCommand != "Then" && lang != PRGMLANG_AXE)))
            {
                nextIndent--;
            }
        }

        str = "";
        for (const auto& line : lines)
        {
            str += str_repeat(" ", line.first * 3) + line.second + '\n';
        }

        return str;
    }

    void TH_0x05::initTokens()
    {
        ifstream t("programs_tokens.csv");
        string csvFileStr((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());

        if (csvFileStr.length() > 0)
        {
            vector<vector<string>> lines;
            ParseCSV(csvFileStr, lines);

            for (const auto& tokenInfo : lines)
            {
                uint bytes;
                if (tokenInfo[6] == "2") // number of bytes for the token
                {
                    if (!is_in_vector(firstByteOfTwoByteTokens, hexdec(tokenInfo[7])))
                    {
                        firstByteOfTwoByteTokens.push_back(hexdec(tokenInfo[7]));
                    }
                    bytes = hexdec(tokenInfo[8]) + (hexdec(tokenInfo[7]) << 8);
                } else {
                    bytes = hexdec(tokenInfo[7]);
                }
                tokens_BytesToName[bytes] = { tokenInfo[4], tokenInfo[5] }; // EN, FR
                tokens_NameToBytes[tokenInfo[4]] = bytes; // EN
                tokens_NameToBytes[tokenInfo[5]] = bytes; // FR
                uchar maxLenName = (uchar) max(tokenInfo[4].length(), tokenInfo[5].length());
                if (maxLenName > lengthOfLongestTokenName)
                {
                    lengthOfLongestTokenName = maxLenName;
                }
            }
        } else {
            throw runtime_error("Could not open the tokens csv file");
        }
    }

}