// Nagłówki
#include <iostream>
#include <array>
#include <cmath>

#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "Shader.h"
#include "VertexArray.h"

using Vec3f = sf::Vector3<GLfloat>;

struct Vertex {
    Vec3f position;
    Vec3f color;
};

constexpr double pi = 3.141592653589793238462643383279502884;
constexpr double twoPi = pi*2;

Vec3f hsv2rgb(GLfloat h, GLfloat s, GLfloat v) {
    Vec3f result;

    // no saturation means gray
    if (s <= 0.0) {
        result.x = v;
        result.y = v;
        result.z = v;
        return result;
    }

    GLfloat hh = static_cast<GLfloat>(fmod(h, twoPi)/(pi/3));

    long i = (long) hh;
    GLfloat ff = hh - i;
    GLfloat p = v * (1.0f - s);
    GLfloat q = v * (1.0f - (s * ff));
    GLfloat t = v * (1.0f - (s * (1.0f - ff)));

    switch (i) {
    case 0:
        result.x = v;
        result.y = t;
        result.z = p;
        break;
    case 1:
        result.x = q;
        result.y = v;
        result.z = p;
        break;
    case 2:
        result.x = p;
        result.y = v;
        result.z = t;
        break;

    case 3:
        result.x = p;
        result.y = q;
        result.z = v;
        break;
    case 4:
        result.x = t;
        result.y = p;
        result.z = v;
        break;
    case 5:
    default:
        result.x = v;
        result.y = p;
        result.z = q;
        break;
    }
    return result;
}

void genShapeData(std::vector<Vertex>& verts, std::vector<GLuint>& indices, std::size_t polygon_vert_count, std::size_t polygon_vert_max_count, float radius) {
    verts.reserve(polygon_vert_max_count);
    indices.reserve(polygon_vert_max_count*3);

    GLfloat step = twoPi/polygon_vert_count;
    for (int i = 0; i < polygon_vert_count; i++) {
        GLfloat angle = step*i;

        verts.push_back({
                Vec3f{radius*cosf(angle), radius*sinf(angle), .0f},
                hsv2rgb(angle, 1, 1),
            });
    }

    for (GLuint i = 0; i < polygon_vert_max_count; i++) {
        indices.push_back(0); // all triangles contain the first vertex
        indices.push_back(i+1);
        indices.push_back(i+2);
    }
}

void updateShapeVerts(std::vector<Vertex>& verts, std::size_t new_count, float radius) {
    GLfloat step = twoPi/new_count;
    for (int i = 0; i < new_count; i++) {
        GLfloat angle = step*i;

        Vertex vert{
            Vec3f{radius*cosf(angle), radius*sinf(angle), .0f},
            hsv2rgb(angle, 1, 1),
        };

        if (i < verts.size())
            verts[i] = vert;
        else
            verts.push_back(vert);
    }
}


int main() {
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;

    // Okno renderingu
    sf::Window window(sf::VideoMode(800, 800, 32), "OpenGL", sf::Style::Titlebar | sf::Style::Close, settings);

    // Inicjalizacja GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW Initalization failed\n";
        return 0;
    }

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
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader.m_shaderId);
    glAttachShader(shaderProgram, fragmentShader.m_shaderId);
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    // Specifikacja formatu danych wierzchołkowych
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, sizeof(Vertex::position)/sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));

    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, sizeof(Vertex::color)/sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, color));

    GLint timeUnif = glGetUniformLocation(shaderProgram, "time");
    glUniform1f(timeUnif, .0);

    // application state
    GLfloat time = .0;
    GLsizei vert_count = 3;
    int event_count = 0;
    GLenum primivite = GL_TRIANGLES;
    bool running = true;
    double prev_y = 0;

    constexpr std::size_t max_verts = 1000;

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    GLfloat r = .8f;
    genShapeData(vertices, indices, vert_count, max_verts, r);

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
        glClear(GL_COLOR_BUFFER_BIT);

        glUniform1f(timeUnif, time);

        glDrawElements(primivite, (GLsizei) (vert_count - 2) * 3, GL_UNSIGNED_INT, indices.data());
        // Wymiana buforów tylni/przedni
        window.display();
        time += 0.0002f;
    }
    // Kasowanie programu i czyszczenie buforów
    glDeleteProgram(shaderProgram);
    glDeleteBuffers(1, &vbo);

    // Zamknięcie okna renderingu
    window.close();
    return 0;
}
