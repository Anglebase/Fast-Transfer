#pragma once
/**
 * 类型安全IO
 */
#ifndef _TYPESAFEIO_HXX_
#define _TYPESAFEIO_HXX_

#include <cstdio>

namespace pack_asyncIO
{
    class TypesafeAsyncOutput
    {
        TypesafeAsyncOutput(const TypesafeAsyncOutput &) = delete;
        TypesafeAsyncOutput(TypesafeAsyncOutput &&) = delete;
        const TypesafeAsyncOutput &operator=(const TypesafeAsyncOutput &) = delete;
        const TypesafeAsyncOutput &operator=(TypesafeAsyncOutput &&) = delete;
        FILE *output;

    public:
        TypesafeAsyncOutput(FILE *out_);
        ~TypesafeAsyncOutput();

        friend TypesafeAsyncOutput &operator<<(TypesafeAsyncOutput &out, char c);
        friend TypesafeAsyncOutput &operator<<(TypesafeAsyncOutput &out, const char *c);
    };

    extern TypesafeAsyncOutput out;

    using fout = TypesafeAsyncOutput;
}

#endif