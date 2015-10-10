/*
 * Part of tivars_lib_cpp
 * (C) 2015 Adrien 'Adriweb' Bertrand
 * https://github.com/adriweb/TIs_lib
 * License: MIT
 */

#include "TIModel.h"
#include "TIModels.h"

using namespace std;

namespace tivars
{
    
    bool TIModel::supportsType(TIVarType& type)
    {
        const vector<string>& exts = type.getExts();
        return this->orderID >= 0 && this->orderID < exts.size() && exts[this->orderID] != "";
    }

    /*** "Constructors" ***/

    /**
     * @param   int     flags  The version compatibliity flags
     * @return  TIModel
     * @throws  \Exception
     */
    TIModel TIModel::createFromFlags(uint flags)
    {
        if (TIModels::isValidFlags(flags))
        {
            TIModel model;
            model.flags = flags;
            model.orderID = TIModels::getDefaulOrderIDFromFlags(flags);
            model.sig = TIModels::getSignatureFromFlags(flags);
            model.name = TIModels::getDefaultNameFromFlags(flags);
            return model;
        } else
        {
            throw invalid_argument("Invalid version ID");
        }
    }

    /**
     * @param   string  name   The version name
     * @return  TIModel
     * @throws  \Exception
     */
    TIModel TIModel::createFromName(string name)
    {
        if (TIModels::isValidName(name))
        {
            TIModel model;
            model.name = name;
            model.orderID = TIModels::getOrderIDFromName(name);
            model.flags = TIModels::getFlagsFromName(name);
            model.sig = TIModels::getSignatureFromName(name);
            return model;
        } else
        {
            throw invalid_argument("Invalid version name");
        }
    }

    /**
     * @param   string  sig    The signature (magic bytes)
     * @return  TIModel
     * @throws  \Exception
     */
    TIModel TIModel::createFromSignature(string sig)
    {
        if (TIModels::isValidSignature(sig))
        {
            TIModel model;
            model.sig = sig;
            model.orderID = TIModels::getDefaultOrderIDFromSignature(sig);
            model.flags = TIModels::getMinFlagsFromSignature(sig);
            model.name = TIModels::getDefaultNameFromSignature(sig);
            return model;
        } else
        {
            throw new invalid_argument("Invalid version signature");
        }
    }

}