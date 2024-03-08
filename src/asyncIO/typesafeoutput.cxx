#include "include/typesafeoutput.hxx"
#include "include/asyncio.hxx"
#include <cstring>
using namespace pack_asyncIO;

TypesafeAsyncOutput::TypesafeAsyncOutput(FILE *out_) : output(out_) {}
TypesafeAsyncOutput::~TypesafeAsyncOutput()
{
    if (this->output == stdout)
        return;
    if (this->output == stdin)
        return;
    if (this->output == stderr)
        return;
    fclose(this->output);
}

namespace pack_asyncIO
{
    TypesafeAsyncOutput out{stdout};
    TypesafeAsyncOutput &operator<<(TypesafeAsyncOutput &out, char c)
    {
        asyncIO.write(&c, sizeof(char), 1, out.output);
        return out;
    }

    TypesafeAsyncOutput &operator<<(TypesafeAsyncOutput &out, const char *c)
    {
        asyncIO.write(c, sizeof(char), strlen(c), out.output);
        return out;
    }
}