#include "Parameter.hpp"

namespace MsqLights {
    Parameter::Parameter() {
    }
    Parameter::Parameter(float* ptr, bool readOnly) {
        readOnly_ = readOnly;
        setPtr(ptr);
    }

    Parameter::Parameter(int* ptr, bool readOnly) {
        readOnly_ = readOnly;
        setPtr(ptr);
    }

    Parameter::Parameter(unsigned int* ptr, bool readOnly) {
        readOnly_ = readOnly;
        setPtr(ptr);
    }

    Parameter::Parameter(unsigned char* ptr, bool readOnly) {
        readOnly_ = readOnly;
        setPtr(ptr);
    }

    Parameter::Parameter(std::string* ptr, bool readOnly) {
        readOnly_ = readOnly;
        setPtr(ptr);
    }
    Parameter::Parameter(float* ptr)
    :Parameter(ptr, false) {}
    Parameter::Parameter(int* ptr)
    :Parameter(ptr, false) {}
    Parameter::Parameter(unsigned int* ptr)
    :Parameter(ptr, false) {}
    Parameter::Parameter(unsigned char* ptr)
    :Parameter(ptr, false) {}
    Parameter::Parameter(std::string* ptr)
    :Parameter(ptr, false) {}



    void Parameter::setPtr(float* ptr) {
        type_ = Type::FLOAT;
        ptr_ = (void*)ptr;
    }

    void Parameter::setPtr(int* ptr) {
        type_ = Type::INT;
        ptr_ = (void*)ptr;
    }

    void Parameter::setPtr(unsigned int* ptr) {
        type_ = Type::UINT;
        ptr_ = (void*)ptr;
    }

    void Parameter::setPtr(unsigned char* ptr) {
        type_ = Type::UCHAR;
        ptr_ = (void*)ptr;
    }

    void Parameter::setPtr(std::string* ptr) {
        type_ = Type::STRING;
        ptr_ = (void*)ptr;
    }

    void Parameter::setVal(float val) {
        if(readOnly_)
            return;

        switch (type_) {
            case Type::FLOAT:
                *getFloatPtr() = val;
                break;
            case Type::INT:
                *getIntPtr() = val;
                break;
            case Type::UINT:
                *getUIntPtr() = val;
                break;
            case Type::UCHAR:
                *getUCharPtr() = val;
                break;
            default:
                break;
        }
    }

    void Parameter::setVal(int val) {
        if(readOnly_)
            return;

        switch (type_) {
            case Type::FLOAT:
                *getFloatPtr() = val;
                break;
            case Type::INT:
                *getIntPtr() = val;
                break;
            case Type::UINT:
                *getUIntPtr() = val;
                break;
            case Type::UCHAR:
                *getUCharPtr() = val;
                break;
            default:
                break;
        }
    }

    void Parameter::setVal(unsigned int val) {
        if(readOnly_)
            return;

        switch (type_) {
            case Type::FLOAT:
                *getFloatPtr() = val;
                break;
            case Type::INT:
                *getIntPtr() = val;
                break;
            case Type::UINT:
                *getUIntPtr() = val;
                break;
            case Type::UCHAR:
                *getUCharPtr() = val;
                break;
            default:
                break;
        }
    }

    void Parameter::setVal(unsigned char val) {
        if(readOnly_)
            return;

        switch (type_) {
            case Type::FLOAT:
                *getFloatPtr() = val;
                break;
            case Type::INT:
                *getIntPtr() = val;
                break;
            case Type::UINT:
                *getUIntPtr() = val;
                break;
            case Type::UCHAR:
                *getUCharPtr() = val;
                break;
            default:
                break;
        }
    }

    void Parameter::setVal(std::string val) {
        if(readOnly_)
            return;
        if (type_ != Type::STRING)
            return;
        getStringPtr()->assign(val); 
    }

    float Parameter::getFloat() {
        switch (type_) {
            case Type::FLOAT:
                return *getFloatPtr();
            case Type::INT:
                return *getIntPtr();
            case Type::UINT:
                return *getUIntPtr();
            case Type::UCHAR:
                return *getUIntPtr();
            default:
                break;
        }
    }

    int Parameter::getInt() {
        switch (type_) {
            case Type::FLOAT:
                return *getFloatPtr();
            case Type::INT:
                return *getIntPtr();
            case Type::UINT:
                return *getUIntPtr();
            case Type::UCHAR:
                return *getUIntPtr();
            default:
                break;
        }
    }

    unsigned int Parameter::getUInt() {
        switch (type_) {
            case Type::FLOAT:
                return *getFloatPtr();
            case Type::INT:
                return *getIntPtr();
            case Type::UINT:
                return *getUIntPtr();
            case Type::UCHAR:
                return *getUIntPtr();
            default:
                break;
     
        }
    }

    unsigned char Parameter::getUChar() {
        switch (type_) {
            case Type::FLOAT:
                return *getFloatPtr();
            case Type::INT:
                return *getIntPtr();
            case Type::UINT:
                return *getUIntPtr();
            case Type::UCHAR:
                return *getUIntPtr();
            default:
                break;
        }
    }

    std::string Parameter::getString() {
        // i dont like this
        if (type_ != Type::STRING)
            return "";
        return *(std::string*)ptr_;
    }

    float* Parameter::getFloatPtr() {
        return (float*) ptr_;
    }

    int* Parameter::getIntPtr() {
        return (int*) ptr_;
    }

    unsigned int* Parameter::getUIntPtr() {
        return (unsigned int*) ptr_;
    }

    unsigned char* Parameter::getUCharPtr() {
        return (unsigned char*) ptr_;
    }

    std::string* Parameter::getStringPtr() {
        // i dont like this
        if (type_ != Type::STRING)
            return nullptr; 
        return (std::string*)ptr_;
    }
}