#pragma once
/**
 * 异步IO
 */
#ifndef _ASYNCIO_HXX_
#define _ASYNCIO_HXX_

#include <thread>
#include <cstdio>
#include <queue>
#include <map>

namespace pack_asyncIO
{

    // 参数中转包
    struct ForwardPack
    {
        void *data;
        std::size_t size;
        std::size_t count;
        FILE *io;

        int *rescnt;
        bool has_value;

        ForwardPack();
        ForwardPack(const void *data, std::size_t size, std::size_t count, FILE *io, bool has_v = true);
        ForwardPack(const ForwardPack &);
        ForwardPack(ForwardPack &&);
        const ForwardPack &operator=(const ForwardPack &);
        const ForwardPack &operator=(ForwardPack &&);
        ~ForwardPack();
    };
    // 异步IO类
    class AsyncInputOutput
    {
        using CallBack = void (*)(AsyncInputOutput *, void *, std::size_t, std::size_t, FILE *);

    private:
        std::queue<ForwardPack> params;
        std::queue<CallBack> functions;
        std::map<std::string, ForwardPack> results;
        std::queue<std::string> keys;

        std::thread iothread;
        bool execute;

        static AsyncInputOutput *single;
        AsyncInputOutput();

    public:
        AsyncInputOutput(const AsyncInputOutput &) = delete;
        AsyncInputOutput(AsyncInputOutput &&) = delete;
        const AsyncInputOutput &operator=(const AsyncInputOutput &) = delete;
        const AsyncInputOutput &operator=(AsyncInputOutput &&) = delete;
        ~AsyncInputOutput();
        static AsyncInputOutput *getSingleObject();

        /**
         * @brief 同步数据读取
         * @param data 存储读取的数据
         * @param size 要读取的单个单元大小
         * @param count 要读取单元的个数
         * @param in 读取源
         */
        void sync_read(void *data, std::size_t size, std::size_t count, FILE *in);
        /**
         * @brief 异步数据读取
         * @param alias 数据别名
         * @param size 要读取的单个单元大小
         * @param count 要读取单元的个数
         * @param in 读取源
         * @note 如果键(数据别名)已存在，则会抛出 name_redfined 异常
         */
        void read(const std::string &alias, std::size_t size, std::size_t count, FILE *in);
        /**
         * @brief 同步数据写入
         * @param data 要写的数据
         * @param size 要写的单个单元大小
         * @param count 要写单元的个数
         * @param out 写入目标
         */
        void sync_write(const void *data, std::size_t size, std::size_t count, FILE *out);
        /**
         * @brief 异步数据写入
         * @param data 要写的数据
         * @param size 要写的单个单元大小
         * @param count 要写单元的个数
         * @param out 写入目标
         */
        void write(const void *data, std::size_t size, std::size_t count, FILE *out);

        /**
         * @brief 检测任务队列是否已空
         * @return 若任务队列为空则为 true 否则为 false
         */
        bool finish() const;
        /**
         * @brief 通过别名获取异步读取的数据
         * @return 存有读取内容及信息的参数中转包
         * @note 如果结果尚未产生则会阻塞代码；该函数调用一次后会移除所存储的结果；如果键不存在则会抛出 name_undefined 异常
         */
        ForwardPack get(const std::string &alias);

        /**
         * 同步调用修改文件指针位置
        */
        void sync_seek(FILE *fp, long s, int pos);
        /**
         * 异步调用修改文件指针位置
         * @param pos 要修改的位置
         * @param fp 被修改的文件
        */
        void fseek(int pos, FILE *fp);
    };
};
// 异步IO全局访问接口
#define asyncIO (*pack_asyncIO::AsyncInputOutput::getSingleObject())
#endif