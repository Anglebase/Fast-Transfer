#include "include/asyncio.hxx"
/**
 * 异步IO的底层实现
 */
#include <thread>
#include <cstdio>
#include <queue>
#include <chrono>
#include <stdexcept>
using namespace std;
using namespace std::chrono_literals;
using namespace pack_asyncIO;

// 参数转发包空值构造
ForwardPack::ForwardPack() : data(nullptr), size(0), count(0), io(nullptr), has_value(false) {}
// 参数转发包构造
ForwardPack::ForwardPack(const void *data, std::size_t size, std::size_t count, FILE *io, bool hv)
{
    std::size_t lenth = (this->size = size) * (this->count = count);
    this->io = io;
    this->data = new char[lenth];
    this->rescnt = new int;
    this->has_value = hv;
    if (data)
        memcpy(this->data, data, lenth);
    *this->rescnt = 1;
}
// 拷贝构造
ForwardPack::ForwardPack(const ForwardPack &it)
{
    this->rescnt = it.rescnt;
    *this->rescnt += 1;
    this->data = it.data;
    this->count = it.count;
    this->size = it.size;
    this->io = it.io;
}
// 移动构造
ForwardPack::ForwardPack(ForwardPack &&it)
{
    this->rescnt = it.rescnt;
    *this->rescnt += 1;
    this->data = it.data;
    this->count = it.count;
    this->size = it.size;
    this->io = it.io;
}
// 拷贝赋值
const ForwardPack &ForwardPack::operator=(const ForwardPack &it)
{
    this->rescnt = it.rescnt;
    *this->rescnt += 1;
    this->data = it.data;
    this->count = it.count;
    this->size = it.size;
    this->io = it.io;
    return *this;
}
// 移动赋值
const ForwardPack &ForwardPack::operator=(ForwardPack &&it)
{
    *this->rescnt += 1;
    this->data = it.data;
    this->count = it.count;
    this->size = it.size;
    this->io = it.io;
    return *this;
}
// 析构
ForwardPack::~ForwardPack()
{
    *this->rescnt -= 1;
    if (*this->rescnt <= 0)
    {
        this->rescnt ? (delete this->rescnt), this->rescnt = nullptr : nullptr;
        this->data ? (delete[] ((char *)this->data)), this->data = nullptr : nullptr;
    }
}

// 异步IO底层实现

AsyncInputOutput::AsyncInputOutput()
    : params{}, functions{}, execute{true}, results{},
      // 异步IO独享线程
      iothread{[&]()
               {
                   while (this->execute)
                   {
                       if (this->functions.empty())
                       {
                           this_thread::sleep_for(100ms);
                           continue;
                       }
                       this->functions.front()(this,
                                               this->params.front().data,
                                               this->params.front().size,
                                               this->params.front().count,
                                               this->params.front().io);
                       this->functions.pop();
                       this->params.pop();
                   }
               }}
{
}
// 析构
AsyncInputOutput::~AsyncInputOutput()
{
    // 终止线程
    this->execute = false;
    this->iothread.join();
    if (AsyncInputOutput::single)
    {
        delete AsyncInputOutput::single;
        AsyncInputOutput::single = nullptr;
    }
}

// 全局单例
AsyncInputOutput *AsyncInputOutput::getSingleObject()
{
    return AsyncInputOutput::single;
}

// 检测任务队列是否已完成
bool AsyncInputOutput::finish() const { return this->functions.empty(); }

// 同步读取数据
void AsyncInputOutput::sync_read(void *data, std::size_t size, std::size_t count, FILE *in)
{
    fread(data, size, count, in);
    this->results[this->keys.front()].has_value = true;
    this->keys.pop();
}
// 同步写入数据
void AsyncInputOutput::sync_write(const void *data, std::size_t size, std::size_t count, FILE *in)
{
    fwrite(data, size, count, in);
}
// 异步读取数据
void AsyncInputOutput::read(const string &alias, std::size_t size, std::size_t count, FILE *in)
{
    // 生成返回值缓冲区
    ForwardPack result{nullptr, size, count, in, false};
    if (this->results.find(alias) == this->results.end())
    {
        this->results[alias] = result;
    }
    else
    {
        class name_redefined : public exception
        {
            string what_arg;

        public:
            name_redefined(const string &arg) : what_arg(arg) {}
            const char *what() const noexcept
            {
                return what_arg.c_str();
            }
        };
        throw name_redefined{string("NameRedefined:") + alias};
    }
    // 加入异步任务队列
    this->params.push(this->results[alias]);
    this->keys.push(alias);
    this->functions.push([](AsyncInputOutput *it, void *data, std::size_t size, std::size_t count, FILE *in)
                         { it->sync_read(data, size, count, in); });
}
// 异步写入数据
void AsyncInputOutput::write(const void *data, std::size_t size, std::size_t count, FILE *in)
{
    // 加入异步任务队列
    this->params.push(ForwardPack{data, size, count, in});
    this->functions.push([](AsyncInputOutput *it, void *data, std::size_t size, std::size_t count, FILE *in)
                         { it->sync_write(data, size, count, in); });
}
// 获取异步读取的数据
ForwardPack AsyncInputOutput::get(const string &alias)
{
    try
    {
        while (!(this->results.at(alias)).has_value)
            this_thread::sleep_for(100ms);
        auto result = this->results.at(alias);
        // auto result = ForwardPack{result_.data, result_.size, result_.count, result_.io};
        this->results.erase(alias);
        return result;
    }
    catch (out_of_range)
    {
        class name_undefined : public exception
        {
            string what_arg;

        public:
            name_undefined(const string &arg) : what_arg(arg) {}
            const char *what() const noexcept
            {
                return what_arg.c_str();
            }
        };
        throw name_undefined{string("NameUndefined:") + alias};
    }
}

void AsyncInputOutput::sync_seek(FILE *fp, long, int pos)
{
    std::fseek(fp, 0, pos);
}

void AsyncInputOutput::fseek(int pos, FILE *fp)
{
    // 加入异步任务队列
    this->params.push(ForwardPack{nullptr, *(std::size_t*)&pos, 0, fp});
    this->functions.push([](AsyncInputOutput *it, void *data, std::size_t pos, std::size_t count, FILE *fp)
                         { it->sync_seek(fp, 0, *(int*)&pos); });
}

AsyncInputOutput *AsyncInputOutput::single = new AsyncInputOutput;