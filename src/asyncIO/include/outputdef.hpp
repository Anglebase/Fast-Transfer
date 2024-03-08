#pragma once
/**
 * 类型安全输出特定实现
 */
#ifndef _OUTPUTDEF_HPP_
#define _OUTPUTDEF_HPP_

#include "typesafeoutput.hxx"
#include <string>
#include <sstream>

namespace pack_asyncIO
{
    template<class T>
    TypesafeAsyncOutput &operator<<(TypesafeAsyncOutput &out, T n)
    {
        std::ostringstream oss;
        oss << n;
        return out << oss.str().c_str();
    }

    template <class T>
    TypesafeAsyncOutput &operator<<(TypesafeAsyncOutput &out, T *ptr)
    {
        const std::size_t lenth = sizeof(ptr);
        char *p = (char *)(void *)&ptr;
        out << "0x";
        for (int i = lenth - 1; i >= 0; i--)
        {
            auto h4 = p[i] >> 4 & 0x0F;
            auto l4 = p[i] & 0x0F;
            out << (char)(h4 < 10 ? h4 + '0' : h4 - 10 + 'A');
            out << (char)(l4 < 10 ? l4 + '0' : l4 - 10 + 'A');
        }
        return out;
    }
}

#endif