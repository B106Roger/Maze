#include "OpenGLWidget.h"
#include <iostream>
#include "MazeWidget.h"
#include <gl\gl.h>
#include <gl\GLU.h>
#include <assert.h>

OpenGLWidget::OpenGLWidget(QWidget *parent) : QGLWidget(parent)
{
	
	top_z = 1.5f;
	but_z = -1;

	QDir dir("Pic");
	if(dir.exists())
		pic_path = "Pic/";
	else
		pic_path = "../x64/Release/Pic/";

	matrix = vector<vector<float>>(4, vector<float>(4, 0));
	DrawCount = 0;
}
void OpenGLWidget::initializeGL()
{
	glClearColor(0,0,0,1);
	glEnable(GL_TEXTURE_2D);
	loadTexture2D(pic_path + "grass.png",grass_ID);
	loadTexture2D(pic_path + "sky.png",sky_ID);
}
void OpenGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(MazeWidget::maze!=NULL)
	{
		//View 1
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(0 , 0 , MazeWidget::w/2 , MazeWidget::h);
		glOrtho (-0.1, MazeWidget::maze->max_xp +0.1, -0.1 , MazeWidget::maze->max_yp +0.1, 0 , 10);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		Mini_Map();

		//View 2
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(MazeWidget::w / 2, 0, MazeWidget::w / 2, MazeWidget::h);
		//glOrtho(-MazeWidget::maze->max_xp / 2, MazeWidget::maze->max_xp / 2, -MazeWidget::maze->max_yp / 2, MazeWidget::maze->max_yp / 2, 0, 10);
		/*gluPerspective 定義透視
		//視野大小, nearplane, farplane, distance
		//Note: You shouldn't use this function to get view matrix, otherwise you will get 0.
		*/
		// gluPerspective(MazeWidget::maze->viewer_fov, 1 , 0.01 , 200);

		/* gluLookAt
		//原本相機位置
		//看的方向
		//哪邊是上面
		//Note: You shouldn't use this function to get view matrix, otherwise you will get 0.
		*/
		float viewerPosX = MazeWidget::maze->viewer_posn[Maze::X];
		float viewerPosY = MazeWidget::maze->viewer_posn[Maze::Y];
		float viewerPosZ = MazeWidget::maze->viewer_posn[Maze::Z];

		/*gluLookAt(viewerPosX, viewerPosZ, viewerPosY,
			viewerPosX + cos(degree_change(MazeWidget::maze->viewer_dir)), viewerPosZ, viewerPosY + sin(degree_change(MazeWidget::maze->viewer_dir)),
			0.0, -1.0, 0.0);*/
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		projectionMatrix();
		Map_3D();
	}
}
void OpenGLWidget::resizeGL(int w,int h)
{
}

//Draw Left Part
void OpenGLWidget::Mini_Map()	
{
	glBegin(GL_LINES);

		float viewerPosX = MazeWidget::maze->viewer_posn[Maze::X];
		float viewerPosY = MazeWidget::maze->viewer_posn[Maze::Y];
		float viewerPosZ = MazeWidget::maze->viewer_posn[Maze::Z];

		for(int i = 0 ; i < (int)MazeWidget::maze->num_edges; i++)
		{
			float edgeStartX = MazeWidget::maze->edges[i]->endpoints[Edge::START]->posn[Vertex::X];
			float edgeStartY = MazeWidget::maze->edges[i]->endpoints[Edge::START]->posn[Vertex::Y];
			float edgeEndX = MazeWidget::maze->edges[i]->endpoints[Edge::END]->posn[Vertex::X];
			float edgeEndY = MazeWidget::maze->edges[i]->endpoints[Edge::END]->posn[Vertex::Y];

			glColor3f(MazeWidget::maze->edges[i]->color[0] , MazeWidget::maze->edges[i]->color[1], MazeWidget::maze->edges[i]->color[2]);
			if(MazeWidget::maze->edges[i]->opaque)
			{
				glVertex2f(edgeStartX, edgeStartY);
				glVertex2f(edgeEndX, edgeEndY);
			}
		}

		//draw frustum
		float len = 0.1;
		glColor3f(1, 1, 1);
		glVertex2f(viewerPosX, viewerPosY);
		glVertex2f(viewerPosX + (MazeWidget::maze->max_xp) * len * cos(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov/2)) ,
			viewerPosY + (MazeWidget::maze->max_yp) * len * sin(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov/2)));

		glVertex2f(viewerPosX, viewerPosY);
		glVertex2f(viewerPosX + (MazeWidget::maze->max_xp) * len * cos(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov/2)) ,
			viewerPosY + (MazeWidget::maze->max_yp) * len *  sin(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov/2)));
	glEnd();
}


//**********************************************************************
//
// * Draws the first-person view of the maze.
//   THIS IS THE FUINCTION YOU SHOULD MODIFY.
//
//Note: You must not use any openGL build-in function to set model matrix, view matrix and projection matrix.
//		ex: gluPerspective, gluLookAt, glTraslatef, glRotatef... etc.
//		Otherwise, You will get 0 !
//======================================================================
void OpenGLWidget::Map_3D()
{
	glLoadIdentity();


	// 畫右邊區塊的所有東西
	//glPolygonMode(GL_FRONT, GL_FILL);

	// 取得當前玩家所在cell
	Cell *viewCell = MazeWidget::maze->getViewCell();

	// 取得當前玩家可視頂點
	float len = 0.1;
	float 
		frustum1X = MazeWidget::maze->viewer_posn[Maze::X] + len * cos(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2)),
		frustum1Y = MazeWidget::maze->viewer_posn[Maze::Y] + len * sin(degree_change(MazeWidget::maze->viewer_dir + MazeWidget::maze->viewer_fov / 2)),
		frustum2X = MazeWidget::maze->viewer_posn[Maze::X] + len * cos(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2)),
		frustum2Y = MazeWidget::maze->viewer_posn[Maze::Y] + len * sin(degree_change(MazeWidget::maze->viewer_dir - MazeWidget::maze->viewer_fov / 2));
	
	LineSeg f1 = LineSeg(MazeWidget::maze->viewer_posn[Maze::X], MazeWidget::maze->viewer_posn[Maze::Y],frustum1X, frustum1Y);
	LineSeg f2 = LineSeg(MazeWidget::maze->viewer_posn[Maze::X], MazeWidget::maze->viewer_posn[Maze::Y], frustum2X, frustum2Y);
	DrawCount = (DrawCount + 1) % 20000000;
	drawMaze(viewCell, f1, f2);
	/*若有興趣的話, 可以為地板或迷宮上貼圖, 此項目不影響評分*/
	glBindTexture(GL_TEXTURE_2D, sky_ID);
	
	// 畫貼圖 & 算 UV
	glDisable(GL_TEXTURE_2D);
}
void OpenGLWidget::loadTexture2D(QString str,GLuint &textureID)
{
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	
	QImage img(str);
	QImage opengl_grass = QGLWidget::convertToGLFormat(img);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, opengl_grass.width(), opengl_grass.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, opengl_grass.bits());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glDisable(GL_TEXTURE_2D);
}
float OpenGLWidget::degree_change(float num)
{
	return num /180.0f * 3.14159f;
}

void OpenGLWidget::drawMaze(Cell *cell, LineSeg f1, LineSeg f2)
{
	
	/*glBegin(GL_POLYGON);
	glVertex2f(3.39, 0);
	glVertex2f(5.29, 0);
	glVertex2f(2.8, -2.07);
	glVertex2f(0.9, -2.07);
	glEnd();*/

	/*glBegin(GL_POLYGON);
	glVertex2f(3.39, 0);
	glVertex2f(5.29, 0);
	glVertex2f(2.8, -2.07);
	glVertex2f(0.9, -2.07);
	glEnd(); */

	//glBegin(GL_POLYGON);
	//glVertex2f(-1,-1);
	//glVertex2f(-1, 1);
	//glVertex2f(1, 1);
	//glVertex2f(1, -1);
	//glEnd();
	//return;
	if (f1.end[Maze::X] == f2.end[Maze::X] && f1.end[Maze::Y] == f2.end[Maze::Y]) return;

	float viewerX = MazeWidget::maze->viewer_posn[Maze::X];
	float viewerY = MazeWidget::maze->viewer_posn[Maze::Y];
	// 玩家可視面對的方向
	LineSeg viewerDir(
		MazeWidget::maze->viewer_posn[Maze::X], 
		MazeWidget::maze->viewer_posn[Maze::Y],
		MazeWidget::maze->viewer_posn[Maze::X] + cos(degree_change(MazeWidget::maze->viewer_dir)),
		MazeWidget::maze->viewer_posn[Maze::Y] + sin(degree_change(MazeWidget::maze->viewer_dir))
	);
	for (int i = 0; i < 4; i++)
	{
		Edge *e = cell->edges[i];
		cout << "Edge: " << i << endl;
		if (e->opaque)
		{
			float crossParameterF1 = f1.Cross_Param(e);
			float crossParameterF2 = f2.Cross_Param(e);
			bool clipF1 = false;
			if (0.f < crossParameterF1) {
				f1.end[Maze::X] = f1.start[Maze::X] + (f1.end[Maze::X] - f1.start[Maze::X]) * crossParameterF1;
				f1.end[Maze::Y] = f1.start[Maze::Y] + (f1.end[Maze::Y] - f1.start[Maze::Y]) * crossParameterF1;
			}
			if (0.f < crossParameterF2) {
				f2.end[Maze::X] = f2.start[Maze::X] + (f2.end[Maze::X] - f2.start[Maze::X]) * crossParameterF2;
				f2.end[Maze::Y] = f2.start[Maze::Y] + (f2.end[Maze::Y] - f2.start[Maze::Y]) * crossParameterF2;
			}

			// 如果Clip過後發現視椎都落在同一個Edge上，就直接畫了
			if (e->WithinEdge(f1.end[Maze::X],f1.end[Maze::Y]) && e->WithinEdge(f2.end[Maze::X],f2.end[Maze::Y]))
			{
				glColor3d(e->color[0] * 255, e->color[1], e->color[2]);
				drawWall(f1.end[Maze::X], f1.end[Maze::Y], f2.end[Maze::X], f2.end[Maze::Y]);
				continue;
			}
			// 否則要對兩邊的frustum進行縮小frustum
			else
			{
				// 玩家座標與Edge一邊的向量
				LineSeg edge1(viewerX, viewerY, e->endpoints[Edge::START]->posn[Vertex::X], e->endpoints[Edge::START]->posn[Vertex::Y]);
				// 玩家座標與Edge另一邊的向量
				LineSeg edge2(viewerX, viewerY, e->endpoints[Edge::END]->posn[Vertex::X], e->endpoints[Edge::END]->posn[Vertex::Y]);
				if (e->WithinEdge(f1.end[Maze::X], f1.end[Maze::Y]))
				{
					//當視線與不透明牆壁交會時，在該牆壁取角度靠近viewDir的edge當在drawWall的參數
					// 並且更新frustum
					if (edge1.innerProduct(viewerDir) < edge2.innerProduct(viewerDir))
					{
						glColor3d(e->color[0], e->color[1], e->color[2]);
						drawWall(edge2.end[Maze::X], edge2.end[Maze::Y], f1.end[Maze::X], f1.end[Maze::Y]);
						f1.end[Maze::X] = edge2.end[Maze::X];
						f1.end[Maze::Y] = edge2.end[Maze::Y];
					}
					else
					{
						glColor3d(e->color[0], e->color[1], e->color[2]);
						drawWall(edge1.end[Maze::X], edge1.end[Maze::Y], f1.end[Maze::X], f1.end[Maze::Y]);
						f1.end[Maze::X] = edge1.end[Maze::X];
						f1.end[Maze::Y] = edge1.end[Maze::Y];
					}
				}
				else if (e->WithinEdge(f2.end[Maze::X], f2.end[Maze::Y]))
				{
					//當視線與不透明牆壁交會時，在該牆壁取角度靠近viewDir的edge當在drawWall的參數
					// 並且更新frustum
					if (edge1.innerProduct(viewerDir) < edge2.innerProduct(viewerDir))
					{
						glColor3d(e->color[0], e->color[1], e->color[2]);
						drawWall(edge2.end[Maze::X], edge2.end[Maze::Y], f2.end[Maze::X], f2.end[Maze::Y]);
						f2.end[Maze::X] = edge2.end[Maze::X];
						f2.end[Maze::Y] = edge2.end[Maze::Y];
					}
					else
					{
						glColor3d(e->color[0], e->color[1], e->color[2]);
						drawWall(edge1.end[Maze::X], edge1.end[Maze::Y], f2.end[Maze::X], f2.end[Maze::Y]);
						f2.end[Maze::X] = edge1.end[Maze::X];
						f2.end[Maze::Y] = edge1.end[Maze::Y];
					}
				}
			}


			
			
			
			 

			// old one
			//Cell * clipedCell1 = cell->Clip_To_Cell(f1.start[Maze::X], f1.start[Maze::Y], f1.end[Maze::X], f1.end[Maze::Y], 0);
			//Cell * clipedCell2 = cell->Clip_To_Cell(f2.start[Maze::X], f2.start[Maze::Y], f2.end[Maze::X], f2.end[Maze::Y], 0);

			// 如果Clip過後發現視椎都落在同一個Edge上，就直接畫了
			if (e->Point_Side(f1.start[Maze::X], f1.start[Maze::Y]) == Edge::ON && e->Point_Side(f2.start[Maze::X], f2.start[Maze::Y]) == Edge::ON)
			{
				glColor3d(e->color[0] * 255, e->color[1], e->color[2]);
				drawWall(f1.start[Maze::X], f1.start[Maze::Y], f2.start[Maze::X], f2.start[Maze::Y]);
				continue;
			}

			//// 玩家座標與Edge一邊的向量
			//LineSeg edge1(viewerX, viewerY, e->endpoints[Edge::START]->posn[Vertex::X], e->endpoints[Edge::START]->posn[Vertex::Y]);;
			//// 玩家座標與Edge另一邊的向量
			//LineSeg edge2(viewerX, viewerY, e->endpoints[Edge::END]->posn[Vertex::X], e->endpoints[Edge::END]->posn[Vertex::Y]);;

			//// 處理視椎1---Frustum f1
			//if (clipedCell1 == NULL) {
			//	// 當視線與不透明牆壁交會時，在該牆壁取角度靠近viewDir的edge當在drawWall的參數
			//	// 並且更新frustum
			//	if (edge1.innerProduct(viewerDir) < edge2.innerProduct(viewerDir))
			//	{
			//		glColor3d(e->color[0], e->color[1], e->color[2]);
			//		drawWall(edge1.end[Maze::X], edge1.end[Maze::Y], f1.end[Maze::X], f1.end[Maze::Y]);
			//		f1.end[Maze::X] = edge1.end[Maze::X];
			//		f1.end[Maze::Y] = edge1.end[Maze::Y];
			//	}
			//	else
			//	{
			//		glColor3d(e->color[0], e->color[1], e->color[2]);
			//		drawWall(edge2.end[Maze::X], edge2.end[Maze::Y], f1.end[Maze::X], f1.end[Maze::Y]);
			//		f1.end[Maze::X] = edge2.end[Maze::X];
			//		f1.end[Maze::Y] = edge2.end[Maze::Y];
			//	}
			//}

			//// 處理視椎2---Frustum f2
			//if (clipedCell2 == NULL) {
			//	// 當視線與不透明牆壁交會時，在該牆壁取角度靠近viewDir的edge當在drawWall的參數
			//	// 並且更新frustum
			//	if (edge1.innerProduct(viewerDir) < edge2.innerProduct(viewerDir))
			//	{
			//		glColor3d(e->color[0], e->color[1], e->color[2]);
			//		drawWall(edge1.end[Maze::X], edge1.end[Maze::Y], f2.end[Maze::X], f2.end[Maze::Y]);
			//		f2.end[Maze::X] = edge1.end[Maze::X];
			//		f2.end[Maze::Y] = edge1.end[Maze::Y];
			//	}
			//	else
			//	{
			//		glColor3d(e->color[0], e->color[1], e->color[2]);
			//		drawWall(edge2.end[Maze::X], edge2.end[Maze::Y], f2.end[Maze::X], f2.end[Maze::Y]);
			//		f2.end[Maze::X] = edge2.end[Maze::X];
			//		f2.end[Maze::Y] = edge2.end[Maze::Y];
			//	}
			//}
		}
		else
		{

		}
	}
}

void OpenGLWidget::drawWall(float xs,float ys,float xe, float ye)
{
	vector<vector<float>> wall = {
		{xs,xs,xe,xe},
		{ 1,-1,-1, 1},
		{ys,ys,ye,ye},
		{ 1, 1, 1, 1}
	};

	cout << '(' << xs << ',' << ys << ')' << '(' << xe << ',' << ye << ')' << endl;

	vector<vector<float>> result = Multiply(matrix, wall);
	glBegin(GL_POLYGON);
	for (int j = 0; j < 4; j++)
	{
		glVertex2f(result[0][j] / result[3][j], result[1][j] / result[3][j]);
	}
	glEnd();
}

void OpenGLWidget::projectionMatrix()
{
	// d數值: 玩家眼睛與成象位置距離
	float d = 0.1f;
	// e座標: 玩家所在位置
	vector<vector<float>> e(1, vector<float>(3, 0.f));
	e[0][0] = MazeWidget::maze->viewer_posn[Maze::X];
	e[0][1] = MazeWidget::maze->viewer_posn[Maze::Y];
	e[0][2] = MazeWidget::maze->viewer_posn[Maze::Z];

	// t向量: 玩家頭頂向量(normalize)
	vector<vector<float>> t(1, vector<float>(3, 0.f));
	t[0][0] = 0.f;
	t[0][1] = 1.f;
	t[0][2] = 0.f;

	// g向量: 玩家視線方向(normalize)
	vector<vector<float>> g(1, vector<float>(3, 0.f));
	g[0][0] = cos(MazeWidget::maze->viewer_dir);
	g[0][1] = sin(MazeWidget::maze->viewer_dir);
	g[0][2] = 0.f;

	// w向量: 玩家視線方向相反(normalize)
	vector<vector<float>> w(1, vector<float>(3, 0.f));
	w[0][0] = -g[0][0];
	w[0][1] = -g[0][1];
	w[0][2] = -g[0][2];

	// u向量: 
	vector<vector<float>> u = OuterProduct(g, t);

	// v向量
	vector<vector<float>> v = OuterProduct(w, u);

	// 建構M world->view 
	vector<vector<float>> worldToView(4, vector<float>(4.0));
	for (int i = 0; i < 4; i++)
	{
		if (i == 3)
		{
			worldToView[0][i] = 0.f;
			worldToView[1][i] = 0.f;
			worldToView[2][i] = 0.f;
			worldToView[3][i] = 1.f;
		}
		else
		{
			worldToView[0][i] = u[0][i];
			worldToView[1][i] = v[0][i];
			worldToView[2][i] = w[0][i];
			worldToView[3][i] = 0.f;
		}
	}
	vector<vector<float>> viewTranslate =
	{
		{1,0,0,-e[0][0]},
		{0,1,0,-e[0][1]},
		{0,0,1,-e[0][2]},
		{0,0,0,       1}
	};
	worldToView = Multiply(worldToView, viewTranslate);


	float nearPos = 0.01f,
		farPos = 200.f,
		fovDeg = this->degree_change(MazeWidget::maze->viewer_fov),
		aspect = 1.f;

	// 建構M view->screen
	vector<vector<float>> viewToScreen =
	{
		{1 / (tan(fovDeg / 2) * aspect),0,0,0},
		{0,1 /tan(fovDeg / 2),            0,0},
		{0,0,-(farPos + nearPos) / (farPos -nearPos) ,-2 * farPos * nearPos / (farPos - nearPos)},
		{0,0,-1,0}
	};
	matrix = Multiply(viewToScreen,worldToView);

	perspective = viewToScreen;
	/*vector<vector<float>> vertex = { {1.5},{1.3},{1.8},{1.f} };
	vector<vector<float>> test = Multiply(perspective, vertex);*/
}

vector<vector<float>> Multiply(const vector<vector<float>> &m1, const vector<vector<float>> &m2)
{
	assert(m1[0].size() == m2.size());
	vector<vector<float>> result(m1.size(), vector<float>(m2[0].size(), 0));
	for (int i = 0; i < result.size(); i++)
	{
		for (int j = 0; j < result[0].size(); j++)
		{
			for (int k = 0; k < m1[0].size(); k++)
			{
				result[i][j] += m1[i][k] * m2[k][j];
			}
		}
	}
	return result;
}

float InnerPrduct(const vector<vector<float>> &v1, const vector<vector<float>> &v2)
{
	assert(v1.size() == 1 && v2.size() == 1);
	assert(v1[0].size() == v2[0].size());
	float sum = 0.f;
	for (int i = 0; i < v1[0].size(); i++)
	{
		sum += v1[0][i] * v2[0][i];
	}
	return sum;
}

vector<vector<float>> OuterProduct(const vector<vector<float>> &v1, const vector<vector<float>> &v2)
{
	assert(v1.size() == 1 && v2.size() == 1);
	assert(v1[0].size() == 3 && v2[0].size() == 3);
	vector<vector<float>> result(1, vector<float>(3, 0.f));
	for (int i = 0; i < v1[0].size(); i++)
	{
		int index1 = (i + 1) % 3;
		int index2 = (i + 2) % 3;
		result[0][i] = v1[0][index1] * v2[0][index2] - v1[0][index2] * v2[0][index1];
	}
	assert(result[0][0] != 0.f || result[0][1] != 0.f || result[0][2] != 0.f);
	return result;
}

void Normalize(vector<vector<float>> &v1)
{
	assert(v1.size() == 1 );
	float total = 0.f;
	for (float t : v1[0])
	{
		total += t;
	}
	for (float &t : v1[0])
	{
		t /= total;
	}
}















