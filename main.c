#ifdef __APPLE__
	#define GL_SILENCE_DEPRECATION
#endif

#include <stdio.h>
#include <GLFW/glfw3.h>
#include <time.h>
#include <stdlib.h>

void window_refresh_callback(GLFWwindow*);
void random_colour(void);
void window_redraw(GLFWwindow*);

int main(void)
{
	printf("Hello, world!\n");
	srand(time(NULL));

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

void random_colour() {
	GLfloat r, g, b;
	r = (float) rand() / RAND_MAX;
	g = (float) rand() / RAND_MAX;
	b = (float) rand() / RAND_MAX;
	glColor3f(r, g, b);
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
