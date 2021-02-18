#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <random>

void window_refresh_callback(GLFWwindow*);
void window_redraw(GLFWwindow*);

void random_colour(void);

int main(int argc, char* argv[]) {
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	std::cout << "Hello, world!\n";

	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwSetWindowRefreshCallback(window, window_refresh_callback);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		window_redraw(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void window_refresh_callback(GLFWwindow* window) {
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(width/-2.0, width/2.0, height/-2.0, height/2.0, -1.0, 1.0);

	window_redraw(window);
}

void window_redraw(GLFWwindow* window) {
	/* Render here */
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_QUADS);
		random_colour(); glVertex2f( 100,  100);
		random_colour(); glVertex2f(-100,  100);
		random_colour(); glVertex2f(-100, -100);
		random_colour(); glVertex2f( 100, -100);
	glEnd();

	/* Swap front and back buffers */
	glfwSwapBuffers(window);
}

void random_colour() {
	static std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);
	glColor3f(distribution(generator), distribution(generator), distribution(generator));
}
