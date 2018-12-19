#include "GL.hpp"

using namespace std;
using namespace cv;

GL::GL() = default;

void GL::init() {
    //	Init the library
    if (!glfwInit()) {
        cerr << "Initialisation GLFW Failed" << endl;
        m_windows = NULL;
    }

    //	Create a windowed mode window and its OpenGL context
    m_windows = glfwCreateWindow(GL_WIDTH, GL_HEIGHT, "GL_window", NULL, NULL);

    if (!m_windows) {
        cerr << "Initialisation of GL window Failed" << endl;
        glfwTerminate();
        m_windows = NULL;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(m_windows);

    glfwSetKeyCallback(m_windows, GL::keyboard_callback);
    glfwSetWindowSizeCallback(m_windows, GL::resize_callback);

    // Get info of GPU and supported OpenGL version
    cout << "Renderer : " << glGetString(GL_RENDERER) << endl;
    cout << "OpenGL version supported : " << glGetString(GL_VERSION) << endl;

    glEnable(GL_DEPTH_TEST); // Depth Testing

    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void GL::initLight() {
	//	Definition des lumieres
	GLfloat lumiere_ambiant[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat lumiere_blanche[4] = { 1.0f, 0.2f, 0.2f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, lumiere_ambiant);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lumiere_blanche);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lumiere_blanche);

	GLfloat mat_ambiant[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat mat_diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat mat_specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat mat_shininess[1] = { 900.0f };

	//glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emission);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambiant);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

	GLfloat position_lumiere[4] = { 0.0f, 0.0f, 200.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, position_lumiere);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_COLOR_MATERIAL);
	//glDisable(GL_COLOR_MATERIAL);
}

void GL::keyboard_callback(GLFWwindow *windows, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(windows, GL_TRUE);
        }
    }
}

void GL::resize_callback(GLFWwindow *window, int new_width, int new_height) {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, new_width, new_height);

    //	Set PROJECTION
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(100, (double) new_width / (double) new_height, 0.1f, 10000.0f);

    //	Set MODELVIEW
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 250.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

GLFWwindow* GL::windows() {
    return this->m_windows;
}