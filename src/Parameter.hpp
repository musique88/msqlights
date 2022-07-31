#include <string>


namespace MsqLights {
    class Parameter {
    public:
        enum class Type {
            FLOAT,
            INT,
            UINT,
            UCHAR,
            STRING,
            OTHER
        };
        Type type_;
        void* ptr_;

        Parameter();
        Parameter(float* ptr);
        Parameter(int* ptr);
        Parameter(unsigned int* ptr);
        Parameter(unsigned char* ptr);
        Parameter(std::string* ptr);

        void setPtr(float* ptr);
        void setPtr(int* ptr);
        void setPtr(unsigned int* ptr);
        void setPtr(unsigned char* ptr);
        void setPtr(std::string* ptr);

        void setVal(float val);
        void setVal(int val);
        void setVal(unsigned int val);
        void setVal(unsigned char val);
        void setVal(std::string val);
 
        float getFloat();
        int getInt();
        unsigned int getUInt();
        unsigned char getUChar();
        std::string getString();

        float* getFloatPtr();
        int* getIntPtr();
        unsigned int* getUIntPtr();
        unsigned char* getUCharPtr();
        std::string* getStringPtr();
    };
}