#include "main.h"

void initGlfw()
{
  if (!glfwInit())
      exit(-1);

// Because Apple refuses to keep up with standards!
#ifdef __APPLE__
  glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

void initWindow()
{
  window = glfwCreateWindow(640, 480, "Euler vs Hamilton", NULL, NULL);
  if (!window)
  {
      glfwTerminate();
      exit(-1);
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);
}

void initGlew()
{
  glewExperimental = GL_TRUE;
  glewInit();
}

void loadShaders()
{
  shader_program = glCreateProgram();

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_glsl, NULL);
  glCompileShader(vertex_shader);
  shaderLog(vertex_shader);

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_glsl, NULL);
  glCompileShader(fragment_shader);
  shaderLog(fragment_shader);

  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);

  glLinkProgram(shader_program);
  programLog(shader_program);

  glUseProgram(shader_program);

  view_location = glGetUniformLocation(shader_program, "view");
  projection_location = glGetUniformLocation(shader_program, "projection");
  world_location = glGetUniformLocation(shader_program, "world");
}

void loadGeometry()
{
  int i = 0;
  verts[i++] = -1.0f;
  verts[i++] = 0.0f;
  verts[i++] = -1.0f;

  verts[i++] = 1.0f;
  verts[i++] = 0.0f;
  verts[i++] = -1.0f;

  verts[i++] = 1.0f;
  verts[i++] = 0.0f;
  verts[i++] = 1.0f;

  verts[i++] = -1.0f;
  verts[i++] = 0.0f;
  verts[i++] = 1.0f;

  i = 0;
  faces[i++] = 0;
  faces[i++] = 2;
  faces[i++] = 1;

  faces[i++] = 0;
  faces[i++] = 3;
  faces[i++] = 2;

  GLuint vptr, iptr;

  glGenVertexArrays(1, &quad);
  glBindVertexArray(quad);

  glGenBuffers(1, &vptr);
  glBindBuffer(GL_ARRAY_BUFFER, vptr);

  glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(VERTEX_ARRAY_SIZE *
    sizeof(GLfloat)), verts, GL_STATIC_DRAW);

  glGenBuffers(1, &iptr);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iptr);

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(INDEX_ARRAY_SIZE *
    sizeof(GLuint)), faces, GL_STATIC_DRAW);

  glEnableVertexAttribArray(POSITION_LOC);
  glVertexAttribPointer(POSITION_LOC, VERTEX_SIZE, GL_FLOAT, GL_FALSE,
          (GLsizei)(VERTEX_SIZE * sizeof(GLfloat)), 0);

  world = scale(world, vec3(10,10,10));

  glUniformMatrix4fv(world_location, 1, GL_FALSE, value_ptr(world));
}

void mouseMoved(GLFWwindow* window, double x, double y)
{
  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
  {
    if (mx != -1 && my != -1)
    {
      double dx = x - mx;
      double dy = y - my;

      ecamf->mouseLook(dx,dy);
      qcamf->mouseLook(dx,dy);

      ecamd->mouseLook(dx,dy);
      qcamd->mouseLook(dx,dy);
    }

    mx = x; my = y;
  }
  else mx = my = -1;
}

template <typename T>
void drawQuarter(int top, int left, int width, int height, Camera<T>* cam)
{
  glViewport(top, left, width, height);

  float ratio = (float) width / (float) height;
  mat4 view = cam->getView();
  mat4 projection = perspective(fovy, ratio, near, far);

  glUniformMatrix4fv(view_location, 1, GL_FALSE, value_ptr(view));
  glUniformMatrix4fv(projection_location, 1, GL_FALSE, value_ptr(projection));

  glDrawElements(GL_TRIANGLES, (GLsizei)INDEX_ARRAY_SIZE, GL_UNSIGNED_INT, 0);

  cam->doKeys(window);
}

double getDifference(Camera<float>* camf, Camera<double>* camd)
{
  tmat4x4<double,highp> matf = (tmat4x4<double,highp>) camf->getMat();
  tmat4x4<double,highp> matd = camd->getMat();

  double diff = 0;

  for (int i = 0; i < 4; i++)
  {
    tvec4<double,highp> v = column(matf, i) - column(matd, i);

    diff += (v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
  }

  return diff/16.0;
}

int main(/*int argc, char ** argv*/)
{

  initGlfw();
  initWindow();
  initGlew();

  glfwSetCursorPosCallback(window, mouseMoved);

  glEnable(GL_VERTEX_ARRAY);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  glClearColor(1,1,1,1);

  loadShaders();

  loadGeometry();

  cout << ("Euler, Quaternion") << endl;

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window))
  {
    glfwGetFramebufferSize(window, &width, &height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // top-left: Euler Camera (float)
    drawQuarter(0, 0, width/2, height/2, ecamf);
    // top-right: Euler Camera (double)
    drawQuarter(width/2, 0, width/2, height/2, ecamd);

    // bottom-left: Quaternion Camera (float)
    drawQuarter(0, height/2, width/2, height/2, qcamf);
    // bottom-right: Quaternion Camera (double)
    drawQuarter(width/2, height/2, width/2, height/2, qcamd);

    double ediff = getDifference(ecamf, ecamd);
    double qdiff = getDifference(qcamf, qcamd);

    cout.precision(15);
    cout << std::fixed << ediff << ", " << qdiff << endl;

    /* Swap front and back buffers */
    glfwSwapBuffers(window);
    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}
