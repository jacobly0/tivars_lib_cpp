/*
 * Part of tivars_lib_cpp
 * (C) 2015-2018 Adrien "Adriweb" Bertrand
 * https://github.com/adriweb/tivars_lib_cpp
 * License: MIT
 */

#include "TypeHandlers.h"
#include "../utils.h"
#include <regex>

using namespace std;

namespace tivars
{

    data_t TH_0x1C::makeDataFromString(const string& str, const options_t& options)
    {
        (void)options;

        throw runtime_error("Unimplemented");

        if (str.empty() || !is_numeric(str))
        {
            throw invalid_argument("Invalid input string. Needs to be a valid Exact Real Radical");
        }
    }

    string TH_0x1C::makeStringFromData(const data_t& data, const options_t& options)
    {
        (void)options;

        if (data.size() != dataByteCount)
        {
            throw invalid_argument("Empty data array. Needs to contain " + to_string(dataByteCount) + " bytes");
        }

        string dataStr;
        for (uint i = 0; i < TH_0x1C::dataByteCount; i++)
        {
            dataStr += (data[i] < 0x10 ? "0" : "") + dechex(data[i]); // zero left pad
        }

        string type = dataStr.substr(0, 2);
        if (!(type == "1c" || type == "1d")) // real or complex (two reals, see TH_1D)
        {
            throw invalid_argument("Invalid data bytes - invalid vartype: " + type);
        }

        int subtype = atoi(dataStr.substr(2, 1).c_str());
        if (subtype < 0 || subtype > 3)
        {
            throw invalid_argument("Invalid data bytes - unknown subtype: " + to_string(subtype));
        }

        const vector<string> parts = {
            (subtype == 1 || subtype == 3 ? "-" : "") + trimZeros(dataStr.substr(9, 3)),
            trimZeros(dataStr.substr(15, 3)),
            (subtype == 2 || subtype == 3 ? "-" : "+") + trimZeros(dataStr.substr(6, 3)),
            trimZeros(dataStr.substr(12, 3)),
            trimZeros(dataStr.substr(3, 3))
        };

        string str = "(" + parts[0] + "*√(" + parts[1] + ")" + parts[2] + "*√(" + parts[3]  + "))/" + parts[4];

        // Improve final display
        str = regex_replace(str, regex("\\+1\\*"), "+");  str = regex_replace(str, regex("\\(1\\*"),  "(");
        str = regex_replace(str, regex("-1\\*"),   "-");  str = regex_replace(str, regex("\\(-1\\*"), "(-");
        str = regex_replace(str, regex("\\+-"),    "-");

        return str;
    }

}