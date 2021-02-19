#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>
#include <random>
#include <thread>

void render_scene(GLFWwindow*, bool const*);
int render_width {640};
int render_height {480};

void window_refresh_callback(GLFWwindow*);
void random_colour(GLfloat*);

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

	auto last = std::chrono::steady_clock::now();
	int fps {0};
	std::chrono::seconds second {1};

	while (!*terminate) {
		auto now = std::chrono::steady_clock::now();
		if (now >= last + second) {
			std::cout
				<< (now - last).count() << " : "
				<< fps << "\t ["
				<< last.time_since_epoch().count() << " -> "
				<<  now.time_since_epoch().count() << "]"
				<< std::endl;
			fps = 0;
			last += second;
		}
		++fps;

		if (width != render_width || height != render_height) {
			width = render_width;
			height = render_height;

			glViewport(0, 0, width, height);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(width/-2.0, width/2.0, height/-2.0, height/2.0, -1.0, 1.0);
		}

		glClear(GL_COLOR_BUFFER_BIT);

		GLfloat c[12]; random_colour(c);
		glBegin(GL_QUADS);
			glColor3f(c[ 0], c[ 1], c[ 2]); glVertex2f( 100,  100);
			glColor3f(c[ 3], c[ 4], c[ 5]); glVertex2f(-100,  100);
			glColor3f(c[ 6], c[ 7], c[ 8]); glVertex2f(-100, -100);
			glColor3f(c[ 9], c[10], c[11]); glVertex2f( 100, -100);
		glEnd();

		glfwSwapBuffers(window);
	}
}

void window_refresh_callback(GLFWwindow* window) { glfwGetFramebufferSize(window, &render_width, &render_height); }

void random_colour(GLfloat* color) {
	static std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);

	static double prev[12];
	static double next[12];
	static auto anchor = std::chrono::steady_clock::now();
	static std::chrono::milliseconds delay {200};

	auto offset = std::chrono::steady_clock::now();
	if (offset >= anchor + delay) {
		anchor += delay;
		for (int i = 0; i < 12; ++i) {
			prev[i] = next[i];
			next[i] = distribution(generator);
		}
	}

	double diff = (std::chrono::duration<double>) (offset - anchor) / delay;
	for (int i = 0; i < 12; ++i)
		color[i] = prev[i] + (next[i] - prev[i]) * diff;
}
