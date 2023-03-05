#ifndef SHADER_H
#define SHADER_H

#include "glad/glad.h" // 包含glad来获取所有的必须OpenGL头文件

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

enum VertexType : int
{
    VERTEX_TYPE_NONE,
    VERTEX_TYPE_ONE,
    VERTEX_TYPE_TWO,
    VERTEX_TYPE_THREE,
    VERTEX_TYPE_MAX,
};
enum FragmentType : int
{
    FRAGMENT_TYPE_NONE,
    FRAGMENT_TYPE_ONE,
    FRAGMENT_TYPE_TWO,
    FRAGMENT_TYPE_THREE,
    FRAGMENT_TYPE_MAX,
};

extern std::unordered_map<VertexType, std::string> vertexMap;
extern std::unordered_map<FragmentType, std::string> fragmentMap;

class Shader
{
public:
    // 程序ID
    unsigned int ID;

    // 构造器读取并构建着色器
    Shader(const char *vertexPath, const char *fragmentPath);
    // 使用/激活程序
    void use();
    // uniform工具函数
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
};

#endif