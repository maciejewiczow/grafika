#include <iostream>
#include <array>
#include <cmath>

#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "Shader.h"
#include "VertexArray.h"
#include "Program.h"
#include "Uniform.h"

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
        vertexShader = gl::Shader::fromFile("default.vert.glsl", gl::ShaderType::Vertex);
        vertexShader.compile();

        std::cout << "Vertex shader compilation OK\n";
    } catch (gl::shader_exception& e) {
        std::cerr << "Vertex shader compilation failed:\n" << e.what() << std::endl;
        return 0;
    }

    gl::Shader fragmentShader;
    try {
        fragmentShader = gl::Shader::fromFile("radial.frag.glsl", gl::ShaderType::Fragment);
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

    glBufferData(GL_ARRAY_BUFFER, vertices.capacity()*sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size()*sizeof(Vertex), vertices.data());

    while (running) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::MouseMoved:
                if (event.mouseMove.y != prev_y)
                    event_count++;
                else
                    break;

                if (event_count == 10) {
                    if (event.mouseMove.y > prev_y&& vert_count < max_verts)
                        vert_count++;
                    else if (event.mouseMove.y < prev_y && vert_count > 3)
                        vert_count--;

                    updateShapeVerts(vertices, vert_count, r);

                    glBindBuffer(GL_ARRAY_BUFFER, vbo);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, vert_count*sizeof(Vertex), vertices.data());

                    event_count = 0;
                    prev_y = event.mouseMove.y;
                }

                break;

            case sf::Event::KeyPressed:
                switch (event.key.code) {
                case sf::Keyboard::Num1:
                    primivite = GL_POINTS;
                    break;
                case sf::Keyboard::Num2:
                    primivite = GL_LINES;
                    break;
                case sf::Keyboard::Num3:
                    primivite = GL_LINE_STRIP;
                    break;
                case sf::Keyboard::Num4:
                    primivite = GL_LINE_LOOP;
                    break;
                case sf::Keyboard::Num5:
                    primivite = GL_TRIANGLES;
                    break;
                case sf::Keyboard::Num6:
                    primivite = GL_TRIANGLE_STRIP;
                    break;
                case sf::Keyboard::Num7:
                    primivite = GL_TRIANGLE_FAN;
                    break;
                case sf::Keyboard::Num8:
                    primivite = GL_QUADS;
                    break;
                case sf::Keyboard::Num9:
                    primivite = GL_QUAD_STRIP;
                    break;
                case sf::Keyboard::Num0:
                    primivite = GL_POLYGON;
                    break;
                };

                if (event.key.code != sf::Keyboard::Escape)
                    break;
            case sf::Event::Closed:
                running = false;
                break;
            }
        }

        // Nadanie scenie koloru czarnego
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, (GLsizei) indices.size(), GL_UNSIGNED_INT, indices.data());
        // Wymiana buforów tylni/przedni
        window.display();
        time += 0.0002f;
    }
    // Kasowanie programu i czyszczenie buforów
    glDeleteBuffers(1, &vbo);

    // Zamknięcie okna renderingu
    window.close();
    return 0;
}
