# ----------------------------------------------------------------
# 1. 基础环境设置
# ----------------------------------------------------------------

# 设置 CMake 的最低版本要求。如果用户电脑上的 CMake 版本比这个低，编译就会报错。
cmake_minimum_required(VERSION 3.16)

# 让 CMake 自动生成 compile_commands.json 文件
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# 定义你的项目名称，这里叫 "my_gdextension_project"。
# 同时指定这个项目使用的是 C++ 语言（LANGUAGES CXX）。
project(StgGame LANGUAGES CXX)

# 强制要求使用 C++17 标准（这是 Godot 4 官方对 C++ 版本的硬性要求）。
set(CMAKE_CXX_STANDARD 17)

# 如果编译器不支持 C++17 标准，直接报错停止编译，而不是降级到旧版本。
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 强制让 GCC 静态链接标准 C 库、标准 C++ 库以及 winpthread 线程库
if(MINGW)
    add_link_options(-static-libgcc -static-libstdc++ -static)
endif()

# ----------------------------------------------------------------
# 2. 设置编译产物的输出路径
# ----------------------------------------------------------------

# 创建一个自定义变量，存放你的 Godot 游戏项目里的 bin 文件夹路径。
set(GD_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/godot_project/exarple_gdextension/bin")

# 告诉 CMake，把编译生成的库文件（如 .dll, .so, .dylib）直接输出到上面设定的 bin 文件夹。
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${GD_OUTPUT_DIR}")

# 告诉 CMake，把编译生成的某些可执行文件或运行时文件也输出到上面设定的 bin 文件夹。
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${GD_OUTPUT_DIR}")


# ----------------------------------------------------------------
# 3. 引入官方的库文件
# ----------------------------------------------------------------

# 把官方的 godot-cpp 子文件夹包含进来。
# CMake 会自动去读 godot-cpp 里面的 CMakeLists.txt，并帮你把它编译成一个静态库。
add_subdirectory(godot-cpp)


# ----------------------------------------------------------------
# 4. 创建你自己的游戏代码库
# ----------------------------------------------------------------

# 🔍 启动全自动搜索：在 src 目录及其所有子目录下查找符合规则的文件
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"  # 匹配并抓取所有以 .cpp 结尾的 C++ 源文件
)                # 搜索到的所有文件路径，最后都会存进名为 SOURCES 的变量盒子里

# 🛠️ 创建编译目标：利用刚刚搜索到的文件列表，开始准备制作一个库


add_library(
  ${PROJECT_NAME}  # 读取项目名称（即你在开头指定的那个插件名字）
  SHARED          # 关键步：指定制作成“动态链接库”，在 Windows 上最终会产出 .dll 文件
  ${SOURCES}       # 把上面搜集到的所有代码文件一锅端，喂给编译器进行编译
  )
  
  # ----------------------------------------------------------------
  # 5. 链接与宏定义配置
  # ----------------------------------------------------------------
  
  #  绑定依赖关系：把你的插件项目和 Godot 官方的静态库牢牢粘在一起
  target_link_libraries(
      ${PROJECT_NAME}  # 接收依赖的目标（就是你刚刚定义的 .dll 插件项目）
      PRIVATE          # 声明为私有依赖（代表这些 Godot 函数只在你的插件内部消化使用）
      godot-cpp        # 链接目标：引入你之前花 8 分钟编译好的官方底层 C++ 绑定库
  )

