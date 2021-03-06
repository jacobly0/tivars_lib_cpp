/*
 * Part of tivars_lib_cpp
 * (C) 2015-2018 Adrien "Adriweb" Bertrand
 * https://github.com/adriweb/tivars_lib_cpp
 * License: MIT
 */

#include "TypeHandlers.h"
#include "../utils.h"

using namespace std;

namespace tivars
{
    
    data_t TH_0x0D::makeDataFromString(const string& str, const options_t& options)
    {
        (void)options;

        data_t data(2); // reserve 2 bytes for size fields

        auto arr = explode(trim(str, "{}"), ',');
        size_t numCount = arr.size();

        bool formatOk = true;
        for (auto& numStr : arr)
        {
            numStr = trim(numStr);
            if (!TH_0x0C::checkValidString(numStr))
            {
                formatOk = false;
                break;
            }
        }
        if (str.empty() || arr.empty() || !formatOk || numCount > 999)
        {
            throw invalid_argument("Invalid input string. Needs to be a valid complex list");
        }

        data[0] = (uchar) (numCount & 0xFF);
        data[1] = (uchar) ((numCount >> 8) & 0xFF);

        for (const auto& numStr : arr)
        {
            const auto& tmp = TH_0x0C::makeDataFromString(numStr);
            data.insert(data.end(), tmp.begin(), tmp.end());
        }

        return data;
    }

    string TH_0x0D::makeStringFromData(const data_t& data, const options_t& options)
    {
        (void)options;

        string str;

        size_t byteCount = data.size();
        size_t numCount = (size_t) ((data[0] & 0xFF) + ((data[1] & 0xFF) << 8));
        if (byteCount < 2+TH_0x0C::dataByteCount || ((byteCount - 2) % TH_0x0C::dataByteCount != 0)
            || (numCount != (size_t)((byteCount - 2) / TH_0x0C::dataByteCount)) || numCount > 999)
        {
            throw invalid_argument("Invalid data array. Needs to contain 2+" + to_string(TH_0x0C::dataByteCount) + "*n bytes");
        }

        str = "{";

        for (size_t i = 2, num = 0; i < byteCount; i += TH_0x0C::dataByteCount, num++)
        {
            str += TH_0x0C::makeStringFromData(data_t(data.begin()+i, data.begin()+i+TH_0x0C::dataByteCount));
            if (num < numCount - 1) // not last num
            {
                str += ',';
            }
        }

        str += "}";
        
        return str;
    }
}
