/*
 * Part of tivars_lib_cpp
 * (C) 2015 Adrien 'Adriweb' Bertrand
 * https://github.com/adriweb/tivars_lib_cpp
 * License: MIT
 */

#include "TIModels.h"
#include "utils.h"

using namespace std;

namespace tivars
{

    enum TIFeatureFlags
    {
        has82things  = 0b00000001, // (1 << 0);
        hasComplex   = 0b00000010, // (1 << 1);
        hasFlash     = 0b00000100, // (1 << 2);
        hasApps      = 0b00001000, // (1 << 3);
        hasClock     = 0b00010000, // (1 << 4);
        hasColorLCD  = 0b00100000, // (1 << 5);
        hasEZ80CPU   = 0b01000000, // (1 << 6);
        hasExactMath = 0b10000000, // (1 << 7);
    };

    unordered_map<string, TIModel>  models;

    /**
     *  Make and insert the associative arrays for the model.
     *
     * @param int|null  orderID The orderID (for the extensions association)
     * @param int       flags   The flags determining available features
     * @param string    name    The name of the calc using this model
     * @param string    sig     The signature (magic bytes) used for this model
     */
    void TIModels::insertModel(int orderID, uint flags, const string name, const string sig)
    {
        TIModel model(orderID, name, flags, sig);

        if (!is_in_umap_string_TIModel(models, name))
            models[name] = model;

        string flags_str = to_string(flags);
        if (!is_in_umap_string_TIModel(models, flags_str))
            models[flags_str] = model;

        if (!is_in_umap_string_TIModel(models, sig))
            models[sig] = model;
    }

    // TODO : Research actual compatibility flags/"versions" from libtifiles, and maybe even TI ?
    void TIModels::initTIModelsArray()
    {
        uint flags82     = 0           | TIFeatureFlags::has82things;
        uint flags83     = flags82     | TIFeatureFlags::hasComplex;
        uint flags82a    = flags83     | TIFeatureFlags::hasFlash;
        uint flags83p    = flags82a    | TIFeatureFlags::hasApps;
        uint flags84p    = flags83p    | TIFeatureFlags::hasClock;
        uint flags84pcse = flags84p    | TIFeatureFlags::hasColorLCD;
        uint flags84pce  = flags84pcse | TIFeatureFlags::hasEZ80CPU;
        uint flags83pce  = flags84pce  | TIFeatureFlags::hasExactMath;

        insertModel(-1, 0,           "Unknown", "");
        insertModel(0,  flags82,     "82",      "**TI82**");
        insertModel(1,  flags83,     "83",      "**TI83**");
        insertModel(2,  flags82a,    "82A",     "**TI83F*");
        insertModel(3,  flags83p,    "82+",     "**TI83F*");
        insertModel(3,  flags83p,    "83+",     "**TI83F*");
        insertModel(3,  flags84p,    "84+",     "**TI83F*");
        insertModel(4,  flags84pcse, "84+CSE",  "**TI83F*");
        insertModel(5,  flags84pce,  "84+CE",   "**TI83F*");
        insertModel(6,  flags83pce,  "83PCE",   "**TI83F*");
    }

    /**
     * @param   int     flags  The model flags
     * @return  string          The model name for those flags
     */
    string TIModels::getDefaultNameFromFlags(uint flags)
    {
        string flags_str = to_string(flags);
        return isValidFlags(flags) ? models[flags_str].getName() : "Unknown";
    }

    /**
     * @param   string  name   The model name
     * @return  int             The model flags for that name
     */
    uint TIModels::getFlagsFromName(string name)
    {
        return isValidName(name) ? models[name].getFlags() : 0;
    }

    /**
     * @param   int     flags  The model flags
     * @return  string          The signature for those flags
     */
    string TIModels::getSignatureFromFlags(uint flags)
    {
        string flags_str = to_string(flags);
        return isValidFlags(flags) ? models[flags_str].getSig() : "";
    }

    /**
     * @param   string  name
     * @return  string          The signature for that name
     */
    string TIModels::getSignatureFromName(string name)
    {
        return isValidName(name) ? models[name].getSig() : "";
    }

    /**
     * @param   string  sig    The signature
     * @return  string          The default calc name whose file formats use that signature
     */
    string TIModels::getDefaultNameFromSignature(string sig)
    {
        return isValidSignature(sig) ? models[sig].getName() : "";
    }

    /**
     * @param   string  sig    The signature
     * @return  int             The default calc order ID whose file formats use that signature
     */
    int TIModels::getDefaultOrderIDFromSignature(string sig)
    {
        return isValidSignature(sig) ? models[sig].getOrderId() : -1;
    }

    /**
     * @param   string  name
     * @return  int             The default calc order ID whose file formats use that signature
     */
    int TIModels::getOrderIDFromName(string name)
    {
        return isValidName(name) ? models[name].getOrderId() : -1;
    }

    /**
     * @param   int     flags  The model flags
     * @return  int             The default calc order ID whose file formats use that signature
     */
    int TIModels::getDefaulOrderIDFromFlags(uint flags)
    {
        string flags_str = to_string(flags);
        return isValidFlags(flags) ? models[flags_str].getOrderId() : -1;
    }

    /**
     * @param   string  sig    The signature
     * @return  string          The minimum compatibility flags for that signaure
     */
    uint TIModels::getMinFlagsFromSignature(string sig)
    {
        return isValidSignature(sig) ? models[sig].getFlags() : 0;
    }


    bool TIModels::isValidFlags(uint flags)
    {
        string flags_str = to_string(flags);
        return (flags != 0 && is_in_umap_string_TIModel(models, flags_str));
    }

    bool TIModels::isValidName(string name)
    {
        return (name != "" && is_in_umap_string_TIModel(models, name));
    }

    bool TIModels::isValidSignature(string sig)
    {
        return (sig != "" && is_in_umap_string_TIModel(models, sig));
    }

};

// TIModels::initTIModelsArray();