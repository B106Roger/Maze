#ifndef MAZE_H
#define MAZE_H

#include <QtWidgets/QMainWindow>
#include <QFileDialog>
#include <QKeyEvent>
#include <QTimer>
#include <QDir>
#include "ui_maze.h"
#include <Maze.h>

class MazeWidget : public QMainWindow
{
	Q_OBJECT
public:
	MazeWidget(QWidget *parent = 0);
	static Maze *maze;
	static const int w = 1200;
	static const int h = 600;
private:
	Ui::MazeClass ui;

	QString filename;
	QTimer *timer;
	void keyPressEvent(QKeyEvent *);
	void keyReleaseEvent(QKeyEvent *);
	
	float move_speed;
	//LB �e���᭱ LR���k
	float move_FB,move_LR, move_Dir;
private slots:
	void Read_Maze();
	void Refrush_Widget();

	void CollisionDetection(float,float);
	int Check_Same_X_or_Y(int,int,int,int,int,int);		//0�N���O��ɡA1�N���PX�A2�N���PY�A3�N���׽u
	void Exit();
};

#endif // MAZE_H