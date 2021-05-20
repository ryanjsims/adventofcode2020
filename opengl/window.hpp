#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <functional>
#include <chrono>
#include <thread>

namespace rjs {
    class window {
    public:
        window(std::string title, int x, int y, int width, int height, int fps = 0){
            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_VISIBLE, false);
            m_window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
            m_width = width;
            m_height = height;
            m_framerate = fps;
            glfwSetWindowPos(m_window, x, y);
            if(m_window == NULL){
                std::cerr << "Failed to create window" << std::endl;
                glfwTerminate();
                exit(1);
            }
            glfwMakeContextCurrent(m_window);
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            {
                std::cout << "Failed to initialize GLAD" << std::endl;
                glfwTerminate();
                exit(1);
            }
            auto on_resize = [](GLFWwindow* win, int width, int height){
                glViewport(0, 0, width, height);
            };

            glViewport(0, 0, width, height);
            glEnable(GL_DEPTH_TEST);
            glfwSetWindowSizeCallback(m_window, on_resize);

            draw = [title](double deltaTime){
                std::cerr << "Error in window: " << title << "\n    No draw callback provided! Exiting!" << std::endl;
                exit(1);
            };
        }

        ~window(){
            glfwDestroyWindow(m_window);
        }

        void set_pos(int x, int y){
            glfwSetWindowPos(m_window, x, y);
        }

        void on_draw(std::function<void(double deltaTime)> callback){
            draw = callback;
        }

        //Callback params: window, key, scancode, action, modifiers
        void on_input(void (*callback)(GLFWwindow*, int, int, int, int)){
            glfwSetKeyCallback(m_window, callback);
        }

        //Callback params: window, utf32 codepoint
        void on_textinput(void (*callback)(GLFWwindow*, unsigned int)){
            glfwSetCharCallback(m_window, callback);
        }

        void run(bool* flag=nullptr){
            glfwShowWindow(m_window);
            using namespace std::chrono;
            time_point<high_resolution_clock> next_frametime, prev_frametime = high_resolution_clock::now();
            duration<double> deltaTime;
            while(!glfwWindowShouldClose(m_window) && (flag == nullptr || *flag)) {
                if(m_framerate > 0){
                    next_frametime = high_resolution_clock::now() + milliseconds(1000 / m_framerate);
                }
                deltaTime = high_resolution_clock::now() - prev_frametime;
                draw(deltaTime.count());
                prev_frametime = high_resolution_clock::now();
                glfwSwapBuffers(m_window);
                glfwPollEvents();
                if(m_framerate){
                    std::this_thread::sleep_until(next_frametime);
                }
                
            }
        }

        int width(){
            return m_width;
        }

        int height(){
            return m_height;
        }

    private:
        GLFWwindow* m_window;
        int m_width;
        int m_height;
        int m_framerate;
        std::function<void(double deltaTime)> draw;

    };
}
#endif