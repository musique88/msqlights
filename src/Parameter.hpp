
namespace MsqLights {
    class Parameter {
    public:
        enum class Type {
            FLOAT,
            INT,
            UINT
        };
        Type type_;
        void* ptr_;

        void setPtr(float* ptr);
        void setPtr(int* ptr);
        void setPtr(unsigned int* ptr);

        void setVal(float val);
        void setVal(int val);
        void setVal(unsigned int val);
 
        float getFloat();
        int getInt();
        unsigned int getUInt();

        float* getFloatPtr();
        int* getIntPtr();
        unsigned int* getUIntPtr();
    };
}