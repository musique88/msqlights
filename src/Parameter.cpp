#include "Parameter.hpp"

namespace MsqLights {

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

    void Parameter::setVal(float val) {
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
        }
    }

    void Parameter::setVal(int val) {
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
        }
    }

    void Parameter::setVal(unsigned int val) {
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
        }
    }


    float Parameter::getFloat() {
        switch (type_) {
            case Type::FLOAT:
                return *getFloatPtr();
            case Type::INT:
                return *getIntPtr();
            case Type::UINT:
                return *getUIntPtr();
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
        }
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
}