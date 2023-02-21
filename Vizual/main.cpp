/*
 * GLUT Shapes Demo
 *
 * Written by Nigel Stewart November 2003
 *
 * This program is test harness for the sphere, cone
 * and torus shapes in GLUT.
 *
 * Spinning wireframe and smooth shaded shapes are
 * displayed until the ESC or q key is pressed.  The
 * number of geometry stacks and slices can be adjusted
 * using the + and - keys.
 */

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <glew.h>
#include <windows.h>
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <time.h>
#include <Shader.h>
using namespace std;

//MY DEFINES AND INCLUDES

#define PI 3.14159265359

#include <Environment.h>
#include <Vector2d.h>

const double epsilon = 1e-7;
Environment* environment;
float* data;
int data_size = 0;
GLuint ssbo = 0;

static void resize(int width, int height)
{
    environment -> screen_width = width;
    environment -> screen_height = height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width - 1, height - 1, 0, 0, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

void store_data() {
    auto data_byte_size = sizeof(float) * 5 * environment -> num_of_points;
    if(data_size != environment -> num_of_points) {
        data = (float*) realloc(data, data_byte_size);
        data_size = environment -> num_of_points;
    }



    int index = 0;
    for(Field* f : environment -> fields) {
        for(Point& p : f -> getPoints()) {
            data[index++] = (float) p.getX() / environment -> screen_width * 2 - 1;
            data[index++] = (float) p.getY() / environment -> screen_height * 2 - 1;
            int color = f -> getColor();
            data[index++] = (float) ((color & 0xff0000) >> 16) / 255.0f;
            data[index++] = (float) ((color & 0x00ff00) >> 8) / 255.0f;
            data[index++] = (float) (color & 0x0000ff) / 255.0f;
        }
    }

    if(ssbo != 0) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
        memcpy(p, data, data_byte_size);
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }
}


double clamp(double low, double value, double high) {
    if(value < low) return low;
    if(value > high) return high;
    return value;
}


void repel(Point& first, Point& second, double coeff) {
    Vector2d dir = first.pos - second.pos;
    double r = dir.length() / coeff;
    double power = 1 / (r * r + epsilon);
    power = clamp(0, coeff * 10 * power - 1, coeff * 100);

    if(r == 0) {
        dir.x = rand() /(double) RAND_MAX * 2 - 1;
        dir.y = rand() /(double) RAND_MAX * 2 - 1;
    }
    dir.normalize();
    first.acc += dir * power;
    dir.rotate(PI);
    second.acc += dir * power;
}

void drag(Point& point) {
    Vector2d dir = point.vel;
    dir.normalize();
    dir.rotate(PI);
    point.acc += dir * 5;
}

void wall(Point& point) {
    double bounce_loss = 0.1;
    if(point.pos.x < 10) {
        point.pos.x = 10 + (10 - point.pos.x);
        point.vel.x = abs(point.vel.x * bounce_loss);
    }

    if(point.pos.y < 10) {
        point.pos.y = 10 + (10 - point.pos.y);
        point.vel.y = abs(point.vel.y * bounce_loss);
    }

    if(point.pos.x > environment -> screen_width - 10) {
        point.pos.x = environment -> screen_width - 10 - (point.pos.x - environment -> screen_width + 10);
        point.vel.x = -abs(point.vel.x * bounce_loss);
    }

    if(point.pos.y > environment -> screen_height - 10) {
        point.pos.y = environment -> screen_height - 10 - (point.pos.y - environment -> screen_height + 10);
        point.vel.y = -abs(point.vel.y * bounce_loss);
    }
}

void compute_forces() {

    for(Field* field : environment -> fields) {
        for(Point& point : field -> getPoints()) {
            point.acc.x = 0;
            point.acc.y = 0;
        }
    }

    for(unsigned int x = 0;x < environment -> fields.size();x++) {
        Field* field = environment -> fields[x];
        vector < Point >& points_x = field -> getPoints();
        for(unsigned int i = 0;i < points_x.size();i++) {
            for(unsigned int y = x;y < environment -> fields.size();y++) {
                unsigned int j;
                if(x == y) j = i + 1;
                else j = 0;
                vector < Point >& points_y = environment -> fields[y] -> getPoints();
                for(;j < points_y.size();j++) {
                    repel(points_x[i], points_y[j], (x != y) * 5 + 1);
                }
            }

            drag(points_x[i]);
        }
    }
}

void update_points(double dt) {
    dt /= 500;

    for(Field* field : environment -> fields) {
        vector< Point >& points = field -> getPoints();
        for(Point& point : points) {
            point.vel += point.acc * dt;
            point.pos += point.acc * (dt * dt / 2) + point.vel * dt;
 

            wall(point);
        }
    }

   store_data();
}

void update_scene(double dt) {
    compute_forces();
    update_points(dt);
}

void draw_circle(double x, double y, double r) {
    double twicePi = 2.0 * PI;
    glBegin(GL_TRIANGLE_FAN);
        glVertex2d(x, y);
        const int resolution = 20;
        for (int i = 0; i <= resolution; i++)   {
            glVertex2d (
                x + (r * cos(i * twicePi / resolution)), y + (r * sin(i * twicePi / resolution))
                );
        }
    glEnd();
}

void render_scene() {
    environment -> shader -> setInt("dataSize", data_size);
    glDrawArrays(GL_QUADS, 0, 4);
}

static void display(void) {
    if(environment -> last_time == -1) {
        environment -> last_time = glutGet(GLUT_ELAPSED_TIME);
        environment -> t = 0;
    } else {
        const double curr_t = glutGet(GLUT_ELAPSED_TIME);
        environment -> t = curr_t - environment -> last_time;
        environment -> last_time = curr_t;
    }
    environment -> build_up_time += environment -> t;
    const int ms_between_updates = 50;
    if(environment -> build_up_time > ms_between_updates) {
        update_scene(ms_between_updates);
        environment -> build_up_time -= ms_between_updates;
    }

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    render_scene();

    glutSwapBuffers();
}



static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27 :
        case 'q':
            exit(0);
            break;
    }

    glutPostRedisplay();
}

static void idle(void)
{
    glutPostRedisplay();
}

float vertices[] = {
    // positions         // colors
    -1.0f, -1.0f,  0.0f,  1.0f, 0.0f, 0.0f,
     1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
     1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
     -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
};

void calc_num_of_points() {
    environment -> num_of_points = 0;
    for(Field* field : environment -> fields)
        environment -> num_of_points += field -> getPoints().size();
}

static void mouse(int button, int state, int x, int y) {
    if(button == GLUT_LEFT_BUTTON) {
        if(state == GLUT_DOWN) {
            environment -> fields.push_back(new Field(rand() % 0xffffff, rand() % 5 + 1, Vector2d(x, environment -> screen_height - y)));
        }
    } else if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        environment -> fields.pop_back();
    }

    calc_num_of_points();
}


int main(int argc, char *argv[]) {
    srand(time(0));

    environment = new Environment();
    environment -> screen_width = 860;
    environment -> screen_height = 640;
    environment -> last_time = -1;
    environment -> fields.push_back(new Field(0x00ffff, 100, Vector2d(300, 300)));
    environment -> fields.push_back(new Field(0xf00000, 22, Vector2d(310, 310)));
    environment -> fields.push_back(new Field(0x1002f5, 22, Vector2d(410, 110)));
    environment -> fields.push_back(new Field(0x00f2f5, 5, Vector2d(20, 310)));
    environment -> fields.push_back(new Field(0x10f200, 5, Vector2d(10, 10)));
    environment -> fields.push_back(new Field(0x0000ff, 5, Vector2d(14, 13)));
    calc_num_of_points();
    data = (float*) malloc(sizeof(float) * 5 * environment -> num_of_points);
    data_size = environment -> num_of_points;
    store_data();



    //GLUT

    glutInit(&argc, argv);
    glutInitWindowSize(environment -> screen_width, environment -> screen_height);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("Vizualizacija");

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutMouseFunc(mouse);
    glutIdleFunc(idle);

    glClearColor(1, 1, 1, 1);
    glDepthFunc(GL_LESS);
    //SHADERS INIT
    auto err = glewInit();
    if(err != GLEW_OK) cout << "NOT OK" << endl;
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * 20 * environment -> num_of_points, data, GL_DYNAMIC_COPY);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    store_data();

    environment -> shader = new Shader("shader.vs",
              "shader.fs");

    environment -> shader -> use();
    //DONE SHADERS INIT

    glutMainLoop();
    return EXIT_SUCCESS;
}
