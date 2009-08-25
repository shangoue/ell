#ifndef INCLUDED_ELL_C_TYPE_H
#define INCLUDED_ELL_C_TYPE_H

#include "CLexer.h"

namespace ell
{           
    namespace C
    {
        struct Type
        {
            virtual Storage getStorage() = 0;
            virtual BasicType getBasicType() = 0;
            virtual Arithm getArithm() = 0;
            virtual bool isConst() = 0;
            virtual bool isVolatile() = 0;
        };

        struct BaseType : public Type
        {
            BaseType()
              : storage(AUTO),
                basic_type(INT),
                arithm(SIGNED),
                const_flag(false),
                volatile_flag(false)
            { }

            Storage getStorage() { return storage; }
            BasicType getBasicType() { return basic_type; }
            Arithm getArithm() { return arithm; }
            bool getConstness() { return const_flag; }
            bool getVolatileness() { return volatile_flag; }

            enum Storage
            {
                AUTO,
                EXTERN,
                STATIC,
                REGISTER
            } storage;

            enum BasicType
            {
                CHAR,
                INT,
                SHORT,
                LONG,
                LONG_LONG,
                FLOAT,
                DOUBLE,
                LONG_DOUBLE
            } basic_type;

            enum Arithm
            {
                SIGNED,
                UNSIGNED,
                IEEE754
            } arithm;

            bool const_flag;
            bool volatile_flag;
        };

        struct TypeDecorator : public Type
        {
            TypeDecorator(Type * decorated)
              : decorated(decorated)
            { }

#           define D(N) N get##N() { return decorated->get##N(); }
            D(Storage) D(BasicType) D(Arithm) D(Constness) D(Volatileness)
#           undef D

            Type * decorated;
        };

        struct Pointer : public TypeDecorator
        {
            Pointer()
              : restrict_flag(false),
                const_flag(false)
            { }

            bool restrict_flag;
            bool const_flag;

            bool getConstness() { return const_flag; }
        };

        struct Array : public Pointer
        {
            /// -1 if not specified
            int dimension;
        };

        struct Function : public TypeDecorator
        {
            std::vector<Type *> arguments;
        };
    }
}

#endif // INCLUDED_ELL_C_TYPE_H
