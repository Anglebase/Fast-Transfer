cmake_minimum_required(VERSION 3.25)

project(FastTransmission)
set(EXECUTABLE_OUTPUT_PATH ../bin)
# 异步输出相关
set(asyncIOsrc src/asyncIO/asyncio.cxx;src/asyncIO/typesafeoutput.cxx)
# 源文件
set(sourcefile src/main.cpp;src/net.cpp;src/send.cpp;src/recv.cpp;src/spre.cpp)

link_libraries(ws2_32)

add_executable(fasttrans ${asyncIOsrc};${sourcefile})