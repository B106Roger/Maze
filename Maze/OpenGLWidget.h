#pragma once
#include <QGLWidget>
#include <QString>
#include <QDir>
#include "Cell.h"
#include "LineSeg.h"
#include <vector>
using namespace std;
class OpenGLWidget :public QGLWidget
{
	Q_OBJECT
public:
	explicit OpenGLWidget(QWidget *parent = 0);

	void initializeGL();
	void paintGL();
	void resizeGL(int ,int );

	//Maze Setting
	void Mini_Map();
	void Map_3D();
	void loadTexture2D(QString, GLuint &);
	float degree_change(float );
private:
	GLuint grass_ID;
	GLuint sky_ID;
	QString pic_path;

	float top_z;
	float but_z;

/*	  
Recursively draw wall
*/
	void drawMaze(Cell *cell, LineSeg f1, LineSeg f2);
	void drawWall(float xs, float ys, float xe, float ye);
	void projectionMatrix();
	vector<vector<float>> matrix;
	vector<vector<float>> perspective;
	int DrawCount;

	float nearDistance;
	float farDistance;
};
vector<vector<float>> Multiply(const vector<vector<float>> &m1, const vector<vector<float>> &m2);
float InnerPrduct(const vector<vector<float>> &v1, const vector<vector<float>> &v2);
vector<vector<float>> OuterProduct(const vector<vector<float>> &v1, const vector<vector<float>> &v2);
void Normalize(vector<vector<float>> &v1);
void PrintMatrix(vector<vector<float>>);
float StableNumber(float num, float precision = 0.00001f);
float Distance(float x1, float y1, float x2, float y2);
float Degree(const LineSeg& line1, const LineSeg& line2);
