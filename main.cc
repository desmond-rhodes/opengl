#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <random>
#include <thread>

void render_scene(GLFWwindow*, bool const*);
int render_width {640};
int render_height {480};

void window_refresh_callback(GLFWwindow*);
void random_colour(void);

int main(int argc, char* argv[]) {
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	GLFWwindow* window;
	if (!glfwInit())
		return -1;
	window = glfwCreateWindow(render_width, render_height, "Hello, World!", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	glfwSetWindowRefreshCallback(window, window_refresh_callback);

	bool terminate {false};
	std::thread render_thread(render_scene, window, &terminate);

	while (!glfwWindowShouldClose(window))
		glfwPollEvents();
	terminate = true;
	render_thread.join();
	glfwTerminate();

	std::cout << "Hello, world!\n";
	return 0;
}

void render_scene(GLFWwindow* window, bool const* terminate) {
	glfwMakeContextCurrent(window);
	int width {0}, height {0};

	while (!*terminate) {
		if (width != render_width || height != render_height) {
			width = render_width;
			height = render_height;

			glViewport(0, 0, width, height);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(width/-2.0, width/2.0, height/-2.0, height/2.0, -1.0, 1.0);
		}

		glClear(GL_COLOR_BUFFER_BIT);
		glBegin(GL_QUADS);
			random_colour(); glVertex2f( 100,  100);
			random_colour(); glVertex2f(-100,  100);
			random_colour(); glVertex2f(-100, -100);
			random_colour(); glVertex2f( 100, -100);
		glEnd();
		glfwSwapBuffers(window);
	}
}

void window_refresh_callback(GLFWwindow* window) { glfwGetFramebufferSize(window, &render_width, &render_height); }

void random_colour() {
	static std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);
	glColor3f(distribution(generator), distribution(generator), distribution(generator));
}
