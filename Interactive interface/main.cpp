// Std. Includes
#include <iostream>
#include <map>
#include <string>
// GLAD
#include <glad/glad.h>
// GLFW
#include <GLFW/glfw3.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H
// Shader
#include "Shader.h"

const float PI = 3.14159265359;

// Properties
GLuint WIDTH = 800, HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void RenderChar(Shader& shader, char c, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
GLuint VAO_char, VBO_char;

// The MAIN function, from here we start our application and run the Game loop
int main()
{
    float key[40][12];
    float charPos[40][2];
    // Init the keys positions
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            int index = i * 8 + j;
            float x = -0.875f + 0.25f * j;
            float y = 0.8f - 0.4f * i;
            charPos[index][0] = x;
            charPos[index][1] = y;
            key[index][0] = x - 0.1f;
            key[index][1] = y - 0.15f;
            key[index][3] = x + 0.1f;
            key[index][4] = y - 0.15f;
            key[index][6] = x - 0.1f;
            key[index][7] = y + 0.15f;
            key[index][9] = x + 0.1f;
            key[index][10] = y + 0.15f;
            key[index][2] = key[index][5] = key[index][8] = key[index][11] = 0.0f;
        } 
    }
    const char keyboard[] = 
    {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
        'Y', 'Z', '0', '1', '2', '3', '4', '5',
        '6', '7', '8', '9', ' ', ',', '.', '<',
    };
    const float frequency[] =
    {
        8.0f, 9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f,
        8.2f, 9.2f, 10.2f, 11.2f, 12.2f, 13.2f, 14.2f, 15.2f,
        8.4f, 9.4f, 10.4f, 11.4f, 12.4f, 13.4f, 14.4f, 15.4f,
        8.6f, 9.6f, 10.6f, 11.6f, 12.6f, 13.6f, 14.6f, 15.6f,
        8.8f, 9.8f, 10.8f, 11.8f, 12.8f, 13.8f, 14.8f, 15.8f,
    };
    const float phase[] =
    {
        0.0f, 0.5 * PI, PI, 1.5 * PI, 0.0f, 0.5 * PI, PI, 1.5 * PI,
        0.5 * PI, PI, 1.5 * PI, 0.0f, 0.5 * PI, PI, 1.5 * PI, 0.0f,
        PI, 1.5 * PI, 0.0f, 0.5 * PI, PI, 1.5 * PI, 0.0f, 0.5 * PI,
        1.5 * PI, 0.0f, 0.5 * PI, PI, 1.5 * PI, 0.0f, 0.5 * PI, PI,
        0.0f, 0.5 * PI, PI, 1.5 * PI, 0.0f, 0.5 * PI, PI, 1.5 * PI,
    };
    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "SSVEP_demo", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,  // first Triangle
        1, 2, 3,   // second Triangle
    };
    
    unsigned int VAO[40];
    unsigned int VBO[40];
    unsigned int EBO;
    glGenVertexArrays(40, VAO);
    glGenBuffers(40, VBO);
    glGenBuffers(1, &EBO);
    
    //bind VAOs and VBOs
    for (int i = 0; i < 40; i++)
    {
        glBindVertexArray(VAO[i]);	// note that we bind to a different VAO now
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);	// and a different VBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(key[i]), key[i], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // because the vertex data is tightly packed we can also specify 0 as the vertex attribute's stride to let OpenGL figure it out
        glEnableVertexAttribArray(0);
    }
    // Set OpenGL options
    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Compile and setup the shader
    Shader charShader("shaders/text.vs", "shaders/text.fs");
    Shader keyShader("shaders/key.vs", "shaders/key.fs");
    charShader.use();

    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

    // Load font as face
    FT_Face face;
    if (FT_New_Face(ft, "fonts/ARIAL.TTF", 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load first 128 characters of ASCII set
    for (GLubyte c = 0; c < 128; c++)
    {
        // Load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);


    // Configure VAO/VBO for texture quads
    glGenVertexArrays(1, &VAO_char);
    glGenBuffers(1, &VBO_char);
    glBindVertexArray(VAO_char);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_char);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Check and call events
        glfwPollEvents();

        // Clear the colorbuffer
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //render keys changing grayscale with time
        float timeValue = glfwGetTime();
        for (int i = 0; i < 40; i++)
        {
            float stimulate = (1.0f + sin(2 * PI * frequency[i] * timeValue + phase[i])) / 2;
            keyShader.use();
            keyShader.setFloat("grayScale", stimulate);
            glBindVertexArray(VAO[i]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            RenderChar(charShader, keyboard[i], charPos[i][0], charPos[i][1], 3.0f, glm::vec3(0.0f, 0.0f, 0.0f));
        }
        RenderChar(charShader, '-', charPos[39][0] + 0.03f, charPos[39][1], 3.0f, glm::vec3(0.0f, 0.0f, 0.0f));
        glBindVertexArray(0);
        // Swap the buffers
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

void RenderChar(Shader& shader, char c, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    shader.use();
    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO_char);
    Character ch = Characters[c];

    GLfloat xpos = x;
    GLfloat ypos = y;

    GLfloat w = ch.Size.x * scale;
    GLfloat h = ch.Size.y * scale;
    // Update VBO for each character
    GLfloat vertices[6][4] = {
             { xpos - w / WIDTH / 2,     ypos + h / HEIGHT / 2,       0.0, 0.0 },
             { xpos - w / WIDTH / 2,     ypos - h / HEIGHT / 2,       0.0, 1.0 },
             { xpos + w / WIDTH / 2,     ypos - h / HEIGHT / 2,       1.0, 1.0 },

             { xpos - w / WIDTH / 2,     ypos + h / HEIGHT / 2,       0.0, 0.0 },
             { xpos + w / WIDTH / 2,     ypos - h / HEIGHT / 2,       1.0, 1.0 },
             { xpos + w / WIDTH / 2,     ypos + h / HEIGHT / 2,       1.0, 0.0 }
    };
    // Render glyph texture over quad
    glBindTexture(GL_TEXTURE_2D, ch.TextureID);
    // Update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, VBO_char);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    WIDTH = width;
    HEIGHT = height;
}