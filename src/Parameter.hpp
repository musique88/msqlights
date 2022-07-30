
namespace MsqLights {
    class Parameter {
    public:
        enum class Type {
            FLOAT,
            INT,
            UINT,
            UCHAR
        };
        Type type_;
        void* ptr_;

        Parameter();
        Parameter(float* ptr);
        Parameter(int* ptr);
        Parameter(unsigned int* ptr);
        Parameter(unsigned char* ptr);

        void setPtr(float* ptr);
        void setPtr(int* ptr);
        void setPtr(unsigned int* ptr);
        void setPtr(unsigned char* ptr);

        void setVal(float val);
        void setVal(int val);
        void setVal(unsigned int val);
        void setVal(unsigned char ptr);
 
        float getFloat();
        int getInt();
        unsigned int getUInt();
        unsigned char getUChar();

        float* getFloatPtr();
        int* getIntPtr();
        unsigned int* getUIntPtr();
        unsigned char* getUCharPtr();
    };
}