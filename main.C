#include "main.h"


void mainLoop()
{
  handleInput();

  glfwGetFramebufferSize(window, &width, &height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // bottom-left: Euler Camera (float)
  drawQuarter(0, 0, width/2, height/2, rcamf);
  // bottom-right: Euler Camera (double)
  drawQuarter(width/2, 0, width/2, height/2, rcamd);

  // top-left: Quaternion Camera (float)
  drawQuarter(0, height/2, width/2, height/2, qcamf);
  // top-right: Quaternion Camera (double)
  drawQuarter(width/2, height/2, width/2, height/2, qcamd);

  double rdiff = getDifference(rcamf, rcamd);
  double qdiff = getDifference(qcamf, qcamd);

  cout.precision(15);
  cout << std::fixed << rdiff << ", " << qdiff << endl;

  /* Swap front and back buffers */
  glfwSwapBuffers(window);
  /* Poll for and process events */
  glfwPollEvents();
}

void handleInput()
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }

  double x, y;
  int key;
  if (is_reading)
  {
    if (!(input_file >> mouse_ready))
    {
      glfwSetWindowShouldClose(window, GL_TRUE);
      return;
    }
    input_file >> x >> y;

    input_file >> key;
    while (key != -1)
    {
      doKeyToCameras(key);
      input_file >> key;
    }
  }
  else
  {
    glfwGetCursorPos(window, &x, &y);

    if (is_writing)
      output_file << mouse_ready << " " << x << " " << y << " ";

    for (unsigned i = 0; i < sizeof(KEYS)/sizeof(int); i++)
    {
      if (glfwGetKey(window, KEYS[i]) == GLFW_PRESS)
      {
        doKeyToCameras(KEYS[i]);

        if (is_writing)
          output_file << KEYS[i] << " ";
      }
    }

    if (is_writing)
      output_file << "-1" << endl;
  }
  mouseMoved(x, y);
}

void doKeyToCameras(int key)
{
  rcamf->doKey(key);
  rcamd->doKey(key);
  qcamf->doKey(key);
  qcamd->doKey(key);
}

void mouseCheck(GLFWwindow*,double x,double y)
{
  if (!mouse_ready)
  {
    mouse_ready = true;
    mx = x; my = y;

    if (is_writing)
      output_file << "0 " << mx << " " << my << " -1 ";
  }
}

void mouseMoved(double x, double y)
{
  if (mouse_ready)
  {
    double dx = x - mx;
    double dy = y - my;

    rcamf->mouseLook((float)dx,(float)dy);
    qcamf->mouseLook((float)dx,(float)dy);

    rcamd->mouseLook(dx,dy);
    qcamd->mouseLook(dx,dy);
  }

  mx = x; my = y;
}

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

void init()
{
  initGlfw();
  initWindow();
  initGlew();


  if (!is_reading)
  {
// Because GLFW refuses to keep up with APPLE!
#ifndef __APPLE__
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#endif
    glfwSetCursorPosCallback(window, mouseCheck);
  }

  glEnable(GL_VERTEX_ARRAY);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  glClearColor(1,1,1,1);
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

double getDifference(CamF* camf, CamD* camd)
{
  CamD::mat4_type matf = (CamD::mat4_type) camf->getMat();
  CamD::mat4_type matd = camd->getMat();

  double diff = 0;

  for (int i = 0; i < 4; i++)
  {
    tvec4<double,highp> v = column(matf, i) - column(matd, i);

    diff += (v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
  }

  return diff/16.0;
}

void handleArguments(int argc, char ** argv)
{
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-i") == 0)
    {
      i++;
      input_file.open(argv[i]);
      is_reading = true;
    }
    else if (strcmp(argv[i], "-o") == 0)
    {
      i++;
      output_file.open(argv[i]);
      is_writing = true;
    }
  }
}

int main(int argc, char ** argv)
{
  handleArguments(argc, argv);

  init();

  loadShaders();

  loadGeometry();

  cout << ("Euler, Quaternion") << endl;

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window))
  {
    mainLoop();
  }

  glfwTerminate();

  input_file.close();
  output_file.close();

  return 0;
}
