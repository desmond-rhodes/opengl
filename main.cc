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
	constexpr std::chrono::microseconds poll_limit_time {33333}; /* 30Hz */

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
	std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
	auto chrono_now_init {std::chrono::steady_clock::now()};

	glfwMakeContextCurrent(window);
	GLsizei current_width {0};
	GLsizei current_height {0};

	auto fps_limit_last {chrono_now_init};
	constexpr std::chrono::microseconds fps_limit_time {41666}; /* 24Hz */

	constexpr GLsizei data_count {24};

	constexpr int vertex_size {3};
	constexpr int vertex_count {vertex_size * data_count};
	GLdouble vertex[vertex_count] {
		-1.0, -1.0, -1.0,     1.0, -1.0, -1.0,     1.0,  1.0, -1.0,    -1.0,  1.0, -1.0,
		-1.0, -1.0,  1.0,     1.0, -1.0,  1.0,     1.0,  1.0,  1.0,    -1.0,  1.0,  1.0,
		-1.0, -1.0, -1.0,     1.0, -1.0, -1.0,     1.0, -1.0,  1.0,    -1.0, -1.0,  1.0,
		-1.0,  1.0, -1.0,     1.0,  1.0, -1.0,     1.0,  1.0,  1.0,    -1.0,  1.0,  1.0,
		-1.0, -1.0, -1.0,    -1.0,  1.0, -1.0,    -1.0,  1.0,  1.0,    -1.0, -1.0,  1.0,
		 1.0, -1.0, -1.0,     1.0,  1.0, -1.0,     1.0,  1.0,  1.0,     1.0, -1.0,  1.0
	};
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(vertex_size, GL_DOUBLE, 0, vertex);

	std::uniform_real_distribution<double> colour_distribution(0.0, 1.0);
	auto colour_change_last {chrono_now_init};
	constexpr std::chrono::milliseconds colour_change_time {400};

	constexpr int colour_size {3};
	constexpr int colour_count {colour_size * data_count};

	GLdouble colour_prev[colour_count];
	GLdouble colour_next[colour_count];
	for (int i {0}; i < colour_count; ++i) {
		colour_prev[i] = colour_distribution(generator);
		colour_next[i] = colour_distribution(generator);
	}

	GLdouble colour[colour_count];
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(colour_size, GL_DOUBLE, 0, colour);

	std::uniform_real_distribution<double> rotation_distribution(0.0, 360.0);
	auto rotation_change_last {chrono_now_init};
	constexpr std::chrono::seconds rotation_change_time {3};

	constexpr int rotation_count {3};
	GLdouble rotation_prev[rotation_count];
	GLdouble rotation_next[rotation_count];
	for (int i {0}; i < rotation_count; ++i) {
		rotation_prev[i] = rotation_distribution(generator);
		rotation_next[i] = rotation_distribution(generator);
	}
	GLdouble rotation[rotation_count];

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.0, 0.0, -2.0);
	glScaled(0.5, 0.5, 0.5);
	glPushMatrix();
	glEnable(GL_DEPTH_TEST);

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
			glFrustum(-w, w, -h, h, 1.0, 3.0);
			glMatrixMode(GL_MODELVIEW);
		}

		auto time_now {std::chrono::steady_clock::now()};

		if (time_now >= colour_change_last + colour_change_time) {
			colour_change_last += colour_change_time;
			for (int i {0}; i < colour_count; ++i) {
				colour_prev[i] = colour_next[i];
				colour_next[i] = colour_distribution(generator);
			}
		}

		GLdouble colour_diff {static_cast<std::chrono::duration<double>>(time_now - colour_change_last) / colour_change_time};
		for (int i {0}; i < colour_count; ++i)
			colour[i] = colour_prev[i] + (colour_next[i] - colour_prev[i]) * colour_diff;

		if (time_now >= rotation_change_last + rotation_change_time) {
			rotation_change_last += rotation_change_time;
			for (int i {0}; i < rotation_count; ++i) {
				rotation_prev[i] = rotation_next[i];
				rotation_next[i] = rotation_distribution(generator);
			}
		}

		GLdouble rotation_diff {static_cast<std::chrono::duration<double>>(time_now - rotation_change_last) / rotation_change_time};
		if (rotation_diff < 0.5)
			rotation_diff = 4.0 * pow(rotation_diff, 3.0);
		else
			rotation_diff = 4.0 * pow(rotation_diff - 1.0, 3.0) + 1.0;
		for (int i {0}; i < rotation_count; ++i)
			rotation[i] = rotation_prev[i] + (rotation_next[i] - rotation_prev[i]) * rotation_diff;

		glPopMatrix(); glPushMatrix();
		glRotated(rotation[0], 1.0, 0.0, 0.0);
		glRotated(rotation[1], 0.0, 1.0, 0.0);
		glRotated(rotation[2], 0.0, 0.0, 1.0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_QUADS, 0, data_count);
		glfwSwapBuffers(window);

		std::this_thread::sleep_for(fps_limit_time - (std::chrono::steady_clock::now() - fps_limit_last));
		fps_limit_last += fps_limit_time;
	}
}
