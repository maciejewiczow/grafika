#include <iostream>
#include <cmath>
#include <iomanip>
#include <string>

#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

#include "Shader.h"
#include "VertexArray.h"
#include "Program.h"
#include "Uniform.h"
#include "FirstPersonControls.h"
#include "Texture.h"

using Vec3f = glm::tvec3<GLfloat>;

struct Vertex {
    Vec3f position;
    Vec3f color;
    glm::tvec2<GLfloat> texCoord;
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
    //window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(55);

    // Inicjalizacja GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW Initalization failed\n";
        return -1;
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
        vertexShader = gl::Shader::fromFile("assets/shaders/textured.vert.glsl", gl::ShaderType::Vertex);
        vertexShader.compile();

        std::cout << "Vertex shader compilation OK\n";
    } catch (gl::shader_exception& e) {
        std::cerr << "Vertex shader compilation failed:\n" << e.what() << std::endl;
        return -1;
    }

    gl::Shader fragmentShader;
    try {
        fragmentShader = gl::Shader::fromFile("assets/shaders/textured.frag.glsl", gl::ShaderType::Fragment);
        fragmentShader.compile();

        std::cout << "Fragment shader compilation OK\n";
    } catch (gl::shader_exception& e) {
        std::cerr << "Fragment shader compilation failed:\n" << e.what() << std::endl;
        return -1;
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
        return -1;
    }

    gl::Texture korwin_tex;
    try {
        korwin_tex.loadImage("assets/textures/korwinium.jpg")
            .bind()
            .setWrapping(gl::Texture::Wrap::Repeat)
            .setMinFilter(gl::Texture::MinFilter::Nearest)
            .setMagFilter(gl::Texture::MagFilter::Nearest)
            .upload();

        std::cout << "Texture loading OK\n";
    } catch (gl::image_load_exception& e) {
        std::cerr << "Failed to load the korwinium texture!\n" << e.what() << "\n";
        return -1;
    }

    // Specifikacja formatu danych wierzchołkowych
    GLint posAttrib = prog.getAttributeLocation("position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, sizeof(Vertex::position)/sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));

    GLint colAttrib = prog.getAttributeLocation("color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, sizeof(Vertex::color)/sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, color));

    GLint texPosAttrib = prog.getAttributeLocation("texCoord");
    glEnableVertexAttribArray(texPosAttrib);
    glVertexAttribPointer(texPosAttrib, sizeof(Vertex::color)/sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, texCoord));

    std::vector<Vertex> vertices{
        // base
        {{-1.f, -.5f, -1.f}, { .0f, 1.0f,  .0f}, { 1.f/3.f, 1.f/3.f }},  // back left
        {{ 1.f, -.5f, -1.f}, { .0f, 1.0f, 1.0f}, { 2.f/3.f, 1.f/3.f }},  // back right
        {{ 1.f, -.5f,  1.f}, { .0f,  .0f, 1.0f}, { 2.f/3.f, 2.f/3.f }},  // front right
        {{-1.f, -.5f,  1.f}, { .0f,  .0f,  .0f}, { 1.f/3.f, 2.f/3.f }},  // front left
        // top - 4 verts with different tex coords
        {{ .0f, 1.f,  .0f}, { 1.f,  1.f,  .0f}, { .5f, 1.f }}, // front
        {{ .0f, 1.f,  .0f}, { 1.f,  1.f,  .0f}, { 1.f, .5f }}, // right
        {{ .0f, 1.f,  .0f}, { 1.f,  1.f,  .0f}, { .5f, .0f }}, // back
        {{ .0f, 1.f,  .0f}, { 1.f,  1.f,  .0f}, { .0f, .5f }}, // left
    };
    std::vector<GLuint> indices{
        // bottom
        0, 1, 2,
        0, 2, 3,
        // front
        2, 3, 4,
        // right
        1, 2, 5,
        // back
        0, 1, 6,
        // left
        0, 3, 7,
    };

    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // uniforms
    auto model = prog.createUniform<glm::mat4>("model");
    auto view = prog.createUniform<glm::mat4>("view");
    auto projection = prog.createUniform<glm::mat4>("projection");

    float scale = 5.f;
    model = glm::scale(glm::mat4{ 1.0f }, { scale, scale, scale });

    gl::PerspectiveCamera camera{ (float) pi/3, resolution, 0.05f, 100.0f };
    camera.setPosition({ -2.f, 15.f, 13.f });
    camera.lookAt({ .0f, .0f, .0f });

    gl::FirstPersonControls controls{ camera, window };
    projection = camera.getProjectionMatrix();
    view = camera.getViewMatrix();

    controls.setViewUniform(view);

    // application state
    bool running = true;
    sf::Clock clock;
    sf::Time timeStep;

    const char* titleBase = "Korwinium (OpenGL) - ";

    while (running) {
        timeStep = clock.getElapsedTime();
        clock.restart();

        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::MouseButtonPressed:
                controls.toggleMouseCapture();
                break;

            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::R)
                    controls.lookAt({ .0f, .0f, .0f });

                if (event.key.code != sf::Keyboard::Escape)
                    break;
            case sf::Event::Closed:
                running = false;
                break;
            }
        }
        controls.update(static_cast<float>(timeStep.asMicroseconds()));

        // Nadanie scenie koloru czarnego
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, indices.data());
        // Wymiana buforów tylni/przedni
        window.display();

        auto stepUs = timeStep.asMicroseconds();

        model = glm::rotate(model.value(), 0.0000012f*stepUs, { .0f, 1.f, .0f });
        if (stepUs > 0) {
            std::string title{ titleBase };
            title += std::to_string(1000000/stepUs);
            title += " FPS (";
            title += std::to_string(stepUs);
            title += "us/frame)";
            window.setTitle(title);
        }
    }
    // Kasowanie programu i czyszczenie buforów
    glDeleteBuffers(1, &vbo);

    // Zamknięcie okna renderingu
    window.close();

    return 0;
}
