#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QPushButton>
#include <QSignalMapper>
#include <QComboBox>
#include <QMenu>
#include <string>
#include "src\TreeRender.h"
#include "src\SceneRender.h"
#include "src\ControlPanel.h"

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
	
	void PrintNodeInfo(int nodeIndex);
	void DisplayBVHPath(int nodeIndex);
	void AddRender(SceneRender *r);
	void AddRender(TreeRender *r);

	virtual void keyPressEvent(QKeyEvent *e);
	virtual void keyReleaseEvent(QKeyEvent *e);

	TreeRender *tRender;
	SceneRender *sRender;
	QSet<int> pressedKeys;

protected:
	void contextMenuEvent(QContextMenuEvent *event);

private slots:
    void handleScalarButton(int index);
	void setSliders(int stepNr, QSlider *a, QSlider *b, int scalarSetIndex);
	void changeRange();
	void resetViewTree();
	void switchCam();
	void openScene();
	void addScalars();
	void resetControlPanel();
	void updateNearPlane(const double &n);
	void updateFarPlane(const double &f);

private:
	void createMenus();
	void createActions();
	void setScalars(QWidget *parent);
	void setCurrNodeStats(QWidget *parent);
	void setSceneStats();
	void showControlPanel();
	void fillStats(QVBoxLayout *w);
    Ui::MainWindow *ui;
	int mCurrentScalarSet;

	CurrentNodeStats currNodeStats;
	SceneStats sceneStats;
	CurrentTreeStats treeStats;
	ScalarValuesGUI scalarsGUI;
	QMenu *fileMenu;
	QAction *openAct;
	QAction *exitAct;
	QString sceneFile;
	QWidget *bvh, *widget3D;
	QPushButton *resViewT, *changeCam;
	QPushButton *addScalarsButton;
	QDoubleSpinBox *nearPlane, *farPlane;

	float sliderStepMult, sliderStepAdd;
};

#endif // MAINWINDOW_H
