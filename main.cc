#include <thread>
#include <chrono>

class hertz_limit {
public:
	hertz_limit(int r) : rate {r}, last {std::chrono::steady_clock::now()} {}
	void sleep() { std::this_thread::sleep_for(rate-(std::chrono::steady_clock::now()-last)); last = (drop) ? std::chrono::steady_clock::now() : last+rate; }
	void skip(bool d) { drop = d; };
private:
	std::chrono::microseconds rate;
	std::chrono::time_point<std::chrono::steady_clock> last;
	bool drop {true};
};

#include <GLFW/glfw3.h>
#include <random>
#include <shared_mutex>

struct {
	char const* title {"OpenGL"};
	int w {640};
	int h {480};
	std::shared_mutex l;
}
winfo;

void window_refresh_callback(GLFWwindow*);

struct {
	bool terminate {false};
	std::shared_mutex l;
}
tinfo;

void renderer(GLFWwindow*);
void random_colour(GLfloat*);

int main(int argc, char* argv[]) {
	if (!glfwInit())
		return -1;

	GLFWwindow* window {glfwCreateWindow(winfo.w, winfo.h, winfo.title, nullptr, nullptr)};
	if (!window) {
		glfwTerminate();
		return -1;
	}
	glfwSetWindowRefreshCallback(window, window_refresh_callback);

	std::thread thread(renderer, window);

	hertz_limit r {33333};
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		r.sleep();
	}
	std::unique_lock tw_lock {tinfo.l};
	tinfo.terminate = true;
	tw_lock.unlock();
	thread.join();

	glfwTerminate();
	return 0;
}

void window_refresh_callback(GLFWwindow* window) {
	std::unique_lock ww_lock {winfo.l};
	glfwGetFramebufferSize(window, &winfo.w, &winfo.h);
}

void renderer(GLFWwindow* window) {
	std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
	auto chrono_now_init {std::chrono::steady_clock::now()};

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	struct {
		GLsizei w {0};
		GLsizei h {0};
	}
	finfo;

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

	std::shared_lock tr_lock {tinfo.l, std::defer_lock};
	std::shared_lock wr_lock {winfo.l, std::defer_lock};

	for (;;) {
		tr_lock.lock();
		if (tinfo.terminate)
			break;
		tr_lock.unlock();

		wr_lock.lock();
		if (finfo.w != winfo.w || finfo.h != winfo.h) {
			finfo.w = winfo.w;
			finfo.h = winfo.h;

			glViewport(0, 0, finfo.w, finfo.h);

			GLdouble w {1};
			GLdouble h {1};
			if (finfo.w > finfo.h)
				w = static_cast<GLdouble>(finfo.w) / finfo.h;
			else
				h = static_cast<GLdouble>(finfo.h) / finfo.w;

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glFrustum(-w, w, -h, h, 1.0, 3.0);
			glMatrixMode(GL_MODELVIEW);
		}
		wr_lock.unlock();

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
	}
}
