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

constexpr GLfloat pi = 3.141592f;
constexpr GLfloat twoPi = pi*2;

Vec3f hsv2rgb(GLfloat h, GLfloat s, GLfloat v) {
    Vec3f result;

    // no saturation means gray
    if (s <= 0.0) {
        result.x = v;
        result.y = v;
        result.z = v;
        return result;
    }

    GLfloat hh = fmod(h, twoPi)/(pi/3);

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

void genShapeData(std::vector<Vertex>& verts, std::vector<GLuint>& indices, std::size_t polygon_vert_count, float radius, Vec3f center_color) {
    verts.reserve(polygon_vert_count + 1);
    indices.reserve(polygon_vert_count*3);
    // the center vertex
    verts.push_back({ Vec3f{.0f, .0f, .0f}, center_color });

    GLfloat step = twoPi/polygon_vert_count;
    for (GLfloat angle = 0; angle < twoPi; angle += step)
        verts.push_back({
                Vec3f{radius*cosf(angle), radius*sinf(angle), .0f},
                hsv2rgb(angle, 1, 1),
            });

    for (GLuint i = 1; i < verts.size()-1; i++) {
        indices.push_back(i);
        indices.push_back(i+1);
        indices.push_back(0); // all triangles contain the center vertex
    }

    // last triangle connects the last vertex with the first outer vertex
    indices.push_back((GLuint) verts.size() - 1);
    indices.push_back(1);
    indices.push_back(0);
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

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    genShapeData(vertices, indices, 10, .7f, Vec3f{ .5f, .5f, .5f });

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertices.size(), vertices.data(), GL_STATIC_DRAW);

    gl::Shader vertexShader;
    try {
        vertexShader = gl::Shader::fromFile("default.vert.glsl", gl::ShaderType::Vertex);
        vertexShader.compile();

        std::cout << "Vertex shader compilation OK\n";
    } catch (gl::shader_exception& e) {
        std::cerr << "Vertex shader compilation failed!\n" << e.what() << std::endl;
        return 0;
    }

    gl::Shader fragmentShader;
    try {
        fragmentShader = gl::Shader::fromFile("radial.frag.glsl", gl::ShaderType::Fragment);
        fragmentShader.compile();

        std::cout << "Fragment shader compilation OK\n";
    } catch (gl::shader_exception& e) {
        std::cerr << "Fragment shader compilation failed!\n" << e.what() << std::endl;
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

    // Rozpoczęcie pętli zdarzeń
    GLfloat time = .0;
    bool running = true;
    while (running) {
        sf::Event windowEvent;
        while (window.pollEvent(windowEvent)) {
            switch (windowEvent.type) {
            case sf::Event::KeyPressed:
                if (windowEvent.key.code != sf::Keyboard::Escape)
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

        // Narysowanie trójkąta na podstawie 3 wierzchołków
        glDrawElements(GL_TRIANGLES, (GLsizei) indices.size(), GL_UNSIGNED_INT, indices.data());
        // Wymiana buforów tylni/przedni
        window.display();
        time += 0.0003f;
    }
    // Kasowanie programu i czyszczenie buforów
    glDeleteProgram(shaderProgram);
    glDeleteBuffers(1, &vbo);

    // Zamknięcie okna renderingu
    window.close();
    return 0;
}
