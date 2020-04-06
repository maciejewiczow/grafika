#include <iostream>
#include <cmath>
#include <iomanip>

#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "Shader.h"
#include "VertexArray.h"
#include "Program.h"
#include "Uniform.h"
#include "FirstPersonControlls.h"

using Vec3f = glm::tvec3<GLfloat>;

struct Vertex {
    Vec3f position;
    Vec3f color;
};

constexpr double pi = 3.141592653589793238462643383279502884;
constexpr double twoPi = pi*2;

std::ostream& operator<<(std::ostream& out, const glm::vec3& v) {
    out << "[" << v.x << ", " << v.y << ", " << v.z << "]";
    return out;
}

int main() {
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;

    glm::tvec2<unsigned int> resolution{ 1300, 900 };

    // Okno renderingu
    sf::Window window(sf::VideoMode(resolution.x, resolution.y, 32), "OpenGL", sf::Style::Titlebar | sf::Style::Close, settings);
    window.setVerticalSyncEnabled(true);

    // Inicjalizacja GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW Initalization failed\n";
        return 0;
    }

    glEnable(GL_DEPTH_TEST);

    // Utworzenie VAO (Vertex Array Object)
    gl::VertexArray vao;
    vao.bind();

    // Utworzenie VBO (Vertex Buffer Object)
    // i skopiowanie do niego danych wierzchołkowych
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    gl::Shader vertexShader;
    try {
        vertexShader = gl::Shader::fromFile("stripes.vert.glsl", gl::ShaderType::Vertex);
        vertexShader.compile();

        std::cout << "Vertex shader compilation OK\n";
    } catch (gl::shader_exception& e) {
        std::cerr << "Vertex shader compilation failed:\n" << e.what() << std::endl;
        return 0;
    }

    gl::Shader fragmentShader;
    try {
        fragmentShader = gl::Shader::fromFile("stripes.frag.glsl", gl::ShaderType::Fragment);
        fragmentShader.compile();

        std::cout << "Fragment shader compilation OK\n";
    } catch (gl::shader_exception& e) {
        std::cerr << "Fragment shader compilation failed:\n" << e.what() << std::endl;
        return 0;
    }

    // Zlinkowanie obu shaderów w jeden wspólny program
    gl::Program prog;
    try {
        prog.useShader(vertexShader)
            .useShader(fragmentShader)
            .bindFragDataLocation(0, "outColor")
            .link()
            .bind();
    } catch (gl::program_link_exception& e) {
        std::cerr << "Program linking failed!\n" << e.what() << "\n";
        return 0;
    }

    // Specifikacja formatu danych wierzchołkowych
    GLint posAttrib = prog.getAttributeLocation("position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, sizeof(Vertex::position)/sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));

    GLint colAttrib = prog.getAttributeLocation("color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, sizeof(Vertex::color)/sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, color));

    std::vector<Vertex> vertices{
        // back quad 
        {{-1.f,  1.f, -1.f}, { .0f, 1.0f,  .0f}},  // top left
        {{ 1.f,  1.f, -1.f}, { .0f, 1.0f, 1.0f}},  // top right
        {{ 1.f, -1.f, -1.f}, { .0f,  .0f, 1.0f}},  // bottom right
        {{-1.f, -1.f, -1.f}, { .0f,  .0f,  .0f}},  // bottom left
        // front quad
        {{-1.f,  1.f,  1.f}, {1.0f, 1.0f,  .0f}},
        {{ 1.f,  1.f,  1.f}, {1.0f, 1.0f, 1.0f}},
        {{ 1.f, -1.f,  1.f}, {1.0f,  .0f, 1.0f}},
        {{-1.f, -1.f,  1.f}, {1.0f,  .0f,  .0f}},
    };
    std::vector<GLuint> indices{
        // back
        0, 1, 2,
        0, 2, 3,
        // front
        4, 5, 6,
        4, 6, 7,
        // left
        0, 4, 7,
        0, 3, 7,
        // right
        1, 5, 6,
        1, 2, 6,
        // top
        0, 1, 5,
        0, 4, 5,
        // bottom
        2, 3, 7,
        2, 6, 7,
    };

    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // uniforms
    auto model = prog.createUniform<glm::mat4>("model");
    auto view = prog.createUniform<glm::mat4>("view");
    auto projection = prog.createUniform<glm::mat4>("projection");
    auto time = prog.createUniform<GLfloat>("time", .0f);
    auto stripesDirection = prog.createUniform<glm::vec3>("stripes_dir", glm::normalize(glm::vec3{ 2.f, -1.f, 1.5f }));

    float scale = 5.f;
    model = glm::scale(glm::mat4{ 1.0f }, { scale, scale, scale });

    gl::PerspectiveCamera camera{ (float) pi/3, resolution, 0.05f, 100.0f };
    camera.setPosition({ -13.f, 15.f, -12.f });
    camera.lookAt({ .0f, .0f, .0f });

    gl::FirstPersonControlls controlls{ camera, window };
    projection = camera.getProjectionMatrix();
    view = camera.getViewMatrix();

    controlls.setViewUniform(view);

    // application state
    bool running = true;
    bool mouseCapturedBeforeBlur = false;

    while (running) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::MouseButtonPressed:
                controlls.toggleMouseCapture();
                break;

            case sf::Event::MouseMoved:
                if (controlls.isMouseCaptured())
                    std::cout << camera.getPosition() << "\t" << camera.getDirection() << " \n";
                break;

            case sf::Event::LostFocus:
                mouseCapturedBeforeBlur = controlls.isMouseCaptured();
                controlls.releaseMouse();
                break;

            case sf::Event::GainedFocus:
                if (mouseCapturedBeforeBlur)
                    controlls.captureMouse();
                break;

            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::R)
                    controlls.lookAt({ .0f, .0f, .0f });

                std::cout << std::fixed << std::setprecision(5);
                std::cout << camera.getPosition() << "\t" << camera.getDirection() << "\n";

                if (event.key.code != sf::Keyboard::Escape)
                    break;
            case sf::Event::Closed:
                running = false;
                break;
            }
        }
        controlls.onUpdate();

        // Nadanie scenie koloru czarnego
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, (GLsizei) indices.size(), GL_UNSIGNED_INT, indices.data());
        // Wymiana buforów tylni/przedni
        window.display();
        time += 0.004f;
    }
    // Kasowanie programu i czyszczenie buforów
    glDeleteBuffers(1, &vbo);

    // Zamknięcie okna renderingu
    window.close();
    return 0;
}
