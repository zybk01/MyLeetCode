#include "zybkopengl.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "shader.h"
#include "zybkLog.h"
#include "zybkTrace.h"
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <windows.h>
#define STB_IMAGE_IMPLEMENTATION
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "stb_image.h"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"

#define modelPath "D:\\repo\\MyLeetCode\\face_detection_yunet_2022mar.onnx"
#define SCR_WIDTH 800
#define SCR_HEIGHT 800
#define SCR_BLOCK_SIZE (10)
#define SCR_GRID_SIZE (SCR_BLOCK_SIZE + 1)
struct Pos
{
    double x;
    double y;
    double z;
    double a;
};

class GLRenderprocessor
{
public:
    int setUpGlEnv()
    {
        ZYBK_TRACE();
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        mWindow = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
        mViewSize.x = SCR_WIDTH;
        mViewSize.y = SCR_HEIGHT;
        if (mWindow == NULL)
        {
            LOGD("Failed to create GLFW window");
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(mWindow);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            LOGD("Failed to initialize GLAD");
            return -1;
        }
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glEnable(GL_DEPTH_TEST);
        glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);
        mpShader = make_shared<Shader>(vertexMap[VERTEX_TYPE_THREE].c_str(), fragmentMap[FRAGMENT_TYPE_THREE].c_str());
        return 0;
    }
    int prepareVertexArray()
    {
        ZYBK_TRACE();
        // float vertices[] = {
        //     //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
        //     0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // 右上
        //     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // 右下
        //     -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // 左下
        //     -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // 左上
        // };
        // unsigned int indices[] = {
        //     // 注意索引从0开始!
        //     // 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
        //     // 这样可以由下标代表顶点组合成矩形

        //     0, 1, 3, // 第一个三角形
        //     1, 2, 3  // 第二个三角形
        // };
        // unsigned int mIndices[81*3*2];
        // float mVertices[500];
        for (int i = 0; i < SCR_GRID_SIZE * SCR_GRID_SIZE; i++)
        {
            mVertices[5 * i] = ((i % SCR_GRID_SIZE) / (float)(SCR_BLOCK_SIZE)) * 2 - 1;
            mVertices[5 * i + 1] = ((i / SCR_GRID_SIZE) / (float)(SCR_BLOCK_SIZE)) * 2 - 1;
            mVertices[5 * i + 2] = 0.0f;
            mVertices[5 * i + 3] = ((i % SCR_GRID_SIZE) / (float)(SCR_BLOCK_SIZE));
            mVertices[5 * i + 4] = (i / SCR_GRID_SIZE) / (float)(SCR_BLOCK_SIZE);
        }
        for (int i = 0; i < SCR_GRID_SIZE - 1; i++)
        {
            for (int j = 0; j < SCR_GRID_SIZE - 1; j++)
            {
                mIndices[6 * ((SCR_GRID_SIZE - 1) * i + j)] = SCR_GRID_SIZE * i + j;
                mIndices[6 * ((SCR_GRID_SIZE - 1) * i + j) + 1] = SCR_GRID_SIZE * i + j + 1;
                mIndices[6 * ((SCR_GRID_SIZE - 1) * i + j) + 2] = SCR_GRID_SIZE * i + j + SCR_GRID_SIZE + 1;
                mIndices[6 * ((SCR_GRID_SIZE - 1) * i + j) + 3] = SCR_GRID_SIZE * i + j;
                mIndices[6 * ((SCR_GRID_SIZE - 1) * i + j) + 4] = SCR_GRID_SIZE * i + j + SCR_GRID_SIZE + 1;
                mIndices[6 * ((SCR_GRID_SIZE - 1) * i + j) + 5] = SCR_GRID_SIZE * i + j + SCR_GRID_SIZE;
            }
        }
        return 0;
    }

    int prepareGlResource()
    {
        ZYBK_TRACE();

        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO);
        glGenBuffers(1, &mEBO);

        glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(mVertices), mVertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mIndices), mIndices, GL_STATIC_DRAW);

        // 位置属性
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        // 颜色属性
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // 纹理属性
        // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
        // glEnableVertexAttribArray(2);

        glGenTextures(1, &mTexture);
        glBindTexture(GL_TEXTURE_2D, mTexture);
        // 为当前绑定的纹理对象设置环绕、过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // 加载并生成纹理
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load("resources/adwa.jpg", &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            LOGD("Failed to load texture");
        }
        stbi_image_free(data);
        mpShader->use();
        glUniform1i(glGetUniformLocation(mpShader->ID, "ourtexture"), 0); // 手动设置
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
        return 0;
    }

    int bufferVertexArray()
    {
        ZYBK_TRACE();
        glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(mVertices), mVertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mIndices), mIndices, GL_STATIC_DRAW);

        // 位置属性
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        // 颜色属性
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // 纹理属性
        // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
        // glEnableVertexAttribArray(2);

        // glGenTextures(1, &mTexture);
        // glBindTexture(GL_TEXTURE_2D, mTexture);
        // // 为当前绑定的纹理对象设置环绕、过滤方式
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // // 加载并生成纹理
        // int width, height, nrChannels;
        // unsigned char *data = stbi_load("resources/adwa.jpg", &width, &height, &nrChannels, 0);
        // if (data)
        // {
        //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        //     glGenerateMipmap(GL_TEXTURE_2D);
        // }
        // else
        // {
        //     LOGD("Failed to load texture");
        // }
        // stbi_image_free(data);
        // mpShader->use();
        // glUniform1i(glGetUniformLocation(mpShader->ID, "ourtexture"), 0); // 手动设置
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
        return 0;
    }

    int renderProcess()
    {
        ZYBK_TRACE();
        // auto mMouseButtonCb = [&](GLFWwindow *window, int button, int action, int mods) -> void
        // {
        //     mouseButtonCallback(window, button, action, mods);
        // };
        // auto mouseButtonCb = std::bind(mouseButtonCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        glfwSetMouseButtonCallback(mWindow, &mouseButtonCallback);
        while (!glfwWindowShouldClose(mWindow))
        {
            processInput(mWindow);
            {
                std::unique_lock<std::mutex> lock(mMutex);
                for (int i = 0; i < mSelectedCursorPos.size() / 2; i++)
                {
                    int indexI, indexJ;
                    indexI = round(mSelectedCursorPos[2 * i].x * SCR_BLOCK_SIZE);
                    indexJ = round(mSelectedCursorPos[2 * i].y * SCR_BLOCK_SIZE);
                    double textI, textJ;
                    // textI = round(temp[2 * i].x * 10);
                    // textJ = round(temp[2 * i].y * 10);
                    textI = mSelectedCursorPos[2 * i + 1].x - mSelectedCursorPos[2 * i].x;
                    textJ = mSelectedCursorPos[2 * i + 1].y - mSelectedCursorPos[2 * i].y;
                    textI = textI > 0.02 ? 0.02 : textI;
                    textI = textI < -0.02 ? -0.02 : textI;
                    textJ = textJ > 0.02 ? 0.02 : textJ;
                    textJ = textJ < -0.02 ? -0.02 : textJ;
                    mVertices[(indexJ * SCR_GRID_SIZE + indexI) * 5 + 3] -= textI;
                    mVertices[(indexJ * SCR_GRID_SIZE + indexI) * 5 + 4] -= textJ;
                    // mVertices[(5 * 10 + 5)*5 + 3] += mKeyInputMap[GLFW_KEY_LEFT] / 10.0;
                    // mVertices[(5 * 10 + 5)*5 + 4] += mKeyInputMap[GLFW_KEY_UP] / 10.0;
                    // LOGD("offset x %f y %f",  mKeyInputMap[GLFW_KEY_LEFT] / 10.0, mKeyInputMap[GLFW_KEY_UP] / 10.0);
                    LOGD("offset x %f y %f", textI, textJ);
                    mSelectedCursorPos.clear();
                }
            }
            bufferVertexArray();
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glActiveTexture(GL_TEXTURE0); // 在绑定纹理之前先激活纹理单元
            glBindTexture(GL_TEXTURE_2D, mTexture);

            mpShader->use();
            glBindVertexArray(mVAO);
            float timeValue = glfwGetTime();
            float greenValue = (sin(timeValue) / 2.0f) + 0.0f;
            glm::vec3 cubePositions[] = {
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec3(2.0f, 5.0f, -15.0f),
                glm::vec3(-1.5f, -2.2f, -2.5f),
                glm::vec3(-3.8f, -2.0f, -12.3f),
                glm::vec3(2.4f, -0.4f, -3.5f),
                glm::vec3(-1.7f, 3.0f, -7.5f),
                glm::vec3(1.3f, -2.0f, -2.5f),
                glm::vec3(1.5f, 2.0f, -2.5f),
                glm::vec3(1.5f, 0.2f, -1.5f),
                glm::vec3(-1.3f, 1.0f, -1.5f)};
            for (int i = 0; i < 10; i++)
            {
                // glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
                // 译注：下面就是矩阵初始化的一个例子，如果使用的是0.9.9及以上版本
                // 下面这行代码就需要改为:
                // glm::mat4 trans = glm::mat4(1.0f)
                // 之后将不再进行提示
                glm::mat4 trans = glm::mat4(1.0f);
                trans = glm::translate(trans, glm::vec3(mKeyInputMap[GLFW_KEY_LEFT], mKeyInputMap[GLFW_KEY_UP], mKeyInputMap[GLFW_KEY_Q]) + cubePositions[i]);
                // trans = glm::rotate(trans, glm::radians(-180.0f), glm::vec3(0, 0, 1));
                trans = glm::rotate(trans, glm::radians((float)mCursorPos.y), glm::vec3(1, 0, 0));
                glm::mat4 perspective = glm::mat4(1.0f);
                perspective = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

                glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
                float radius = 10.0f;
                float camX = static_cast<float>(sin(glfwGetTime()) * radius);
                float camZ = static_cast<float>(cos(glfwGetTime()) * radius);
                view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                // vec = trans * vec;
                // trans = glm::rotate(trans, glm::radians(mCursorPos.x), glm::vec3(mKeyInputMap[GLFW_KEY_UP], mKeyInputMap[GLFW_KEY_LEFT], 0.0f));

                glUniform4f(glGetUniformLocation(mpShader->ID, "inputD"), mKeyInputMap[GLFW_KEY_UP], mKeyInputMap[GLFW_KEY_LEFT], greenValue, 1);
                // glUniform4f(glGetUniformLocation(mpShader->ID, "inputR"), mCursorPos.x, mCursorPos.y, 1, 1);
                unsigned int transformLoc = glGetUniformLocation(mpShader->ID, "transform");
                glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
                unsigned int perspectiveLoc = glGetUniformLocation(mpShader->ID, "perspective");
                glUniformMatrix4fv(perspectiveLoc, 1, GL_FALSE, glm::value_ptr(perspective));
                unsigned int viewLoc = glGetUniformLocation(mpShader->ID, "view");
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
                glDrawElements(GL_TRIANGLES, SCR_BLOCK_SIZE * SCR_BLOCK_SIZE * 3 * 2, GL_UNSIGNED_INT, 0);
                // glDrawArrays(GL_TRIANGLES, 0, 36);
                // LOGD("print x %f y %f  rx %f ry %f", mKeyInputMap[GLFW_KEY_LEFT], mKeyInputMap[GLFW_KEY_UP], mCursorPos.x, mCursorPos.y);
            }

            glfwSwapBuffers(mWindow);
            glfwPollEvents();
        }

        glDeleteVertexArrays(1, &mVAO);
        glDeleteBuffers(1, &mVBO);
        glDeleteBuffers(1, &mEBO);
        glfwTerminate();
        LOGD("glfwInit!!!");
        return 0;
    }

    void processInput(GLFWwindow *window)
    {
        ZYBK_TRACE();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            mKeyInputMap[GLFW_KEY_UP] += 0.01f;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            mKeyInputMap[GLFW_KEY_UP] -= 0.01f;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            mKeyInputMap[GLFW_KEY_LEFT] -= 0.01f;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            mKeyInputMap[GLFW_KEY_LEFT] += 0.01f;
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            mKeyInputMap[GLFW_KEY_Q] += 0.01f;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        {
            mKeyInputMap[GLFW_KEY_Q] -= 0.01f;
        }
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
        {
            mKeyInputMap[GLFW_KEY_Q] -= 0.01f;
        }
        if (glfwGetKey(window, GLFW_KEY_PAUSE) == GLFW_PRESS)
        {
            for (auto itr : mKeyInputMap)
            {
                itr.second = 0;
            }
        }
        glfwGetCursorPos(window, &mCursorPos.x, &mCursorPos.y);
    }
    static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
    {
        ZYBK_TRACE();
        glViewport(0, 0, width, height);
        mViewSize.x = width;
        mViewSize.y = height;
        LOGD("glViewport width %d , height %d", width, height);
    }

    static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
    {
        ZYBK_TRACE();
        std::unique_lock<std::mutex> lock(mMutex);
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            if (action == GLFW_RELEASE)
            {
                Pos pos;
                glfwGetCursorPos(window, &pos.x, &pos.y);
                pos.x = pos.x / mViewSize.x;
                pos.y = pos.y / mViewSize.y;
                pos.y = 1.0 - pos.y;
                mSelectedCursorPos.push_back(pos);
                LOGD("mouseButtonCallback posx %f , posy %f", pos.x, pos.y);
            }
            else
            {
                Pos pos;
                glfwGetCursorPos(window, &pos.x, &pos.y);
                pos.x = pos.x / mViewSize.x;
                pos.y = pos.y / mViewSize.y;
                pos.y = 1.0 - pos.y;
                mSelectedCursorPos.push_back(pos);
                LOGD("mouseButtonCallback posx %f , posy %f", pos.x, pos.y);
            }
            break;

        default:
            break;
        }
    }

private:
    std::shared_ptr<Shader> mpShader;
    std::unordered_map<int, float> mKeyInputMap;
    GLFWwindow *mWindow;
    Pos mCursorPos;
    static std::mutex mMutex;
    static std::vector<Pos> mSelectedCursorPos;
    static Pos mViewSize;
    unsigned int mVAO;
    unsigned int mVBO;
    unsigned int mEBO;
    unsigned int mTexture;
    unsigned int mIndices[SCR_BLOCK_SIZE * SCR_BLOCK_SIZE * 3 * 2];
    float mVertices[SCR_GRID_SIZE * SCR_GRID_SIZE * 5];
};
std::mutex GLRenderprocessor::mMutex;
std::vector<Pos> GLRenderprocessor::mSelectedCursorPos;
Pos GLRenderprocessor::mViewSize;

cv::Mat visualize(const cv::Mat& image, const cv::Mat& faces, float fps = -1.f)
{
    static cv::Scalar box_color{0, 255, 0};
    static std::vector<cv::Scalar> landmark_color{
        cv::Scalar(255,   0,   0), // right eye
        cv::Scalar(  0,   0, 255), // left eye
        cv::Scalar(  0, 255,   0), // nose tip
        cv::Scalar(255,   0, 255), // right mouth corner
        cv::Scalar(  0, 255, 255)  // left mouth corner
    };
    static cv::Scalar text_color{0, 255, 0};

    auto output_image = image.clone();

    if (fps >= 0)
    {
        cv::putText(output_image, cv::format("FPS: %.2f", fps), cv::Point(0, 15), cv::FONT_HERSHEY_SIMPLEX, 0.5, text_color, 2);
    }

    for (int i = 0; i < faces.rows; ++i)
    {
        // Draw bounding boxes
        int x1 = static_cast<int>(faces.at<float>(i, 0));
        int y1 = static_cast<int>(faces.at<float>(i, 1));
        int w = static_cast<int>(faces.at<float>(i, 2));
        int h = static_cast<int>(faces.at<float>(i, 3));
        cv::rectangle(output_image, cv::Rect(x1, y1, w, h), box_color, 2);

        // Confidence as text
        float conf = faces.at<float>(i, 14);
        cv::putText(output_image, cv::format("%.4f", conf), cv::Point(x1, y1+12), cv::FONT_HERSHEY_DUPLEX, 0.5, text_color);

        // Draw landmarks
        for (int j = 0; j < landmark_color.size(); ++j)
        {
            int x = static_cast<int>(faces.at<float>(i, 2*j+4)), y = static_cast<int>(faces.at<float>(i, 2*j+5));
            cv::circle(output_image, cv::Point(x, y), 2, landmark_color[j], 2);
        }
    }
    return output_image;
}

void resize(cv::Mat scaleGrayMat, cv::Mat grayimage) {
    float ratioCol = grayimage.cols / (float)scaleGrayMat.cols;
    float ratioRow = grayimage.rows / (float)scaleGrayMat.rows;
    for (int i = 0; i < scaleGrayMat.cols; i++) {
        float srcX = (i + 0.5) * ratioCol - 0.5;
        for (int j = 0; j < scaleGrayMat.rows; j ++) {
            float srcY = (j + 0.5) * ratioRow - 0.5;
            float u = 1.0 + srcX - round(srcX);
            float v = 1.0 + srcY - round(srcY);
            int srcI = int(ceil(srcX + 0.5)) - 1;
            int srcJ = int(ceil(srcY + 0.5)) - 1;
            srcI = srcI > (grayimage.cols - 2) ? grayimage.cols - 2 : srcI;
            srcJ = srcJ > (grayimage.rows - 2) ? grayimage.rows - 2 : srcJ;
            scaleGrayMat.data[j * scaleGrayMat.cols + i] = (1 - u) * (1 - v) * grayimage.at<uchar>(srcI, srcJ) + (1 - u) * (v)*grayimage.at<uchar>(srcI, srcJ + 1) + (u) * (1 - v) * grayimage.at<uchar>(srcI + 1, srcJ) + (u) * (v)*grayimage.at<uchar>(srcI + 1, srcJ + 1);
        }
    }
}

int opengl()
{
    ZYBK_TRACE();
    cv::Mat image = cv::imread("resources/adwa.jpg");
    cv::Mat simage;
    cv::Mat faces;
    cv::imshow("adad", image);
    cv::resize(image, simage, cv::Size(512, 512));
    cv::imshow("aaa", simage);
    cv::Mat scaleMat = (cv::Mat_<float>(2,2) << 3, 0, 0, 3);
    cv::Mat scaleMatInv = scaleMat.inv();
    LOGD("invMat %f %f %f %f", scaleMatInv.at<float>(0, 0), scaleMatInv.at<float>(0, 1), scaleMatInv.at<float>(1, 0), scaleMatInv.at<float>(1, 1));
    cv::print(scaleMatInv);
    cv::Mat grayimage;
    image = simage;
    cv::cvtColor(image, grayimage, cv::COLOR_RGB2GRAY);
    cv::imshow("aa4a", grayimage);
    cv::Mat scaleGrayMat = (cv::Mat_<uchar>(image.cols * 6, image.rows * 6));
    
    LOGD("opengl finished!!!");
    resize(scaleGrayMat, grayimage);
    LOGD("opengl finished!!!");
    cv::imshow("a3aa1212", scaleGrayMat);
    while (1)
    {
        cv::waitKey(0);
        /* code */
    };

    // cv::Ptr<cv::FaceDetectorYN> facedetector = cv::FaceDetectorYN::create(modelPath, "", simage.size());

    // facedetector->detect(simage, faces);
    // LOGD("face size %d %d", faces.size().width, faces.size().width);
    // LOGD("face raw %d %d", faces.rows, faces.cols);
    // for (int i = 0; i < faces.rows; ++i)
    // {
    //     int x1 = static_cast<int>(faces.at<float>(i, 0));
    //     int y1 = static_cast<int>(faces.at<float>(i, 1));
    //     int w = static_cast<int>(faces.at<float>(i, 2));
    //     int h = static_cast<int>(faces.at<float>(i, 3));
    //     float conf = faces.at<float>(i, 14);
    //     LOGD("%d: x1=%d, y1=%d, w=%d, h=%d, conf=%.4f\n", i, x1, y1, w, h, conf);
    // }

    // cv::imshow("aaaa", visualize(simage, faces));

    // GLRenderprocessor processor;
    // processor.setUpGlEnv();
    // processor.prepareVertexArray();
    // processor.prepareGlResource();
    // processor.renderProcess();
    LOGD("opengl finished!!!");
    return 0;
}
