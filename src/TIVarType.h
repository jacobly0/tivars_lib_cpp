/*
 * Part of tivars_lib_cpp
 * (C) 2015 Adrien 'Adriweb' Bertrand
 * https://github.com/adriweb/tivars_lib_cpp
 * License: MIT
 */

#ifndef TIVARTYPE_H
#define TIVARTYPE_H

#include "autoloader.h"
#include "TypeHandlers/ITIVarTypeHandler.h"
#include "TIModel.h"

namespace tivars
{

    class TIVarType
    {

    public:

        TIVarType()
        {}

        TIVarType(int id, std::string name, std::vector<std::string> exts, ITIVarTypeHandler* typeHandler) : id(id), name(name), exts(exts), typeHandler(typeHandler)
        {}

        ~TIVarType()
        {}

        /* Getters */
        int getId() { return this->id; }
        std::string getName() { return this->name; }
        std::vector<std::string> getExts() { return this->exts; }
        ITIVarTypeHandler* getTypeHandler() { return this->typeHandler; }

        static ITIVarTypeHandler* determineTypeHandler(int typeID);

        /*** "Constructors" ***/
        static TIVarType createFromID(uint id);
        static TIVarType createFromName(std::string name);

    private:
        int id = -1;
        std::string name = "Unknown";
        std::vector<std::string> exts;
        ITIVarTypeHandler* typeHandler = nullptr;

    };

}

#endif