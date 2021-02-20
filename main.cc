#include <GLFW/glfw3.h>
#include <thread>
#include <chrono>
#include <random>

void window_refresh_callback(GLFWwindow*);
int window_width {640};
int window_height {480};

void renderer(GLFWwindow*, bool const*);
void random_colour(GLfloat*);

int main(int argc, char* argv[]) {
	if (!glfwInit())
		return -1;

	GLFWwindow* window {glfwCreateWindow(window_width, window_height, "Hello, World!", NULL, NULL)};
	if (!window) {
		glfwTerminate();
		return -1;
	}
	glfwSetWindowRefreshCallback(window, window_refresh_callback);

	bool terminate {false};
	std::thread thread(renderer, window, &terminate);

	auto poll_limit_last {std::chrono::steady_clock::now()};
	std::chrono::microseconds poll_limit_time {33333}; /* 30Hz */

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		std::this_thread::sleep_for(poll_limit_time - (std::chrono::steady_clock::now() - poll_limit_last));
		poll_limit_last += poll_limit_time;
	}
	terminate = true;
	thread.join();

	glfwTerminate();
	return 0;
}

void window_refresh_callback(GLFWwindow* window) { glfwGetFramebufferSize(window, &window_width, &window_height); }

void renderer(GLFWwindow* window, bool const* terminate) {
	glfwMakeContextCurrent(window);
	GLsizei current_width {0};
	GLsizei current_height {0};

	auto fps_limit_last {std::chrono::steady_clock::now()};
	std::chrono::microseconds fps_limit_time {41666}; /* 24Hz */

	GLdouble vertex[12] {
		-1.0, -1.0,  0.0,
		 1.0, -1.0,  0.0,
		 1.0,  1.0,  0.0,
		-1.0,  1.0,  0.0
	};
	GLfloat colour[12];
	GLuint index[4] {0, 1, 2, 3};

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_DOUBLE, 0, vertex);
	glColorPointer(3, GL_FLOAT, 0, colour);

	while (!*terminate) {
		if (current_width != window_width || current_height != window_height) {
			/* WARNING: Data reading is not thread safe! */
			current_width = window_width;
			current_height = window_height;

			glViewport(0, 0, current_width, current_height);

			GLdouble w {1};
			GLdouble h {1};
			if (current_width > current_height)
				w = static_cast<GLdouble>(current_width) / current_height;
			else
				h = static_cast<GLdouble>(current_height) / current_width;

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(-w, w, -h, h, -1.0, 1.0);
		}

		random_colour(colour);

		glClear(GL_COLOR_BUFFER_BIT);
		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, index);
		glfwSwapBuffers(window);

		std::this_thread::sleep_for(fps_limit_time - (std::chrono::steady_clock::now() - fps_limit_last));
		fps_limit_last += fps_limit_time;
	}
}

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
