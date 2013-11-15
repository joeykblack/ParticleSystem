#ifndef DISPLAY_H
#define DISPLAY_H

#define W 1000
#define H 1000
#define FPS 50

void draw();
void creatDisplay();
void Reshape(int w, int h);
void update(int value);
void keypress (unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void mouseMotion(int x, int y);



#endif //DISPLAY_H
