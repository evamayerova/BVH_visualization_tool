#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDoubleSpinBox>
#include <QMessageBox>
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

	void AddRender(SceneRender *r);
	void AddRender(TreeRender *r);
	void DisplayBVHPath(int nodeIndex);
	void PrintNodeInfo(int nodeIndex);

	virtual void keyPressEvent(QKeyEvent *e);
	virtual void keyReleaseEvent(QKeyEvent *e);

	QSet<int> pressedKeys;
	SceneRender *sRender;
	TreeRender *tRender;

protected:
	void contextMenuEvent(QContextMenuEvent *event);

private slots:
	void handleScalarButton(int index);
	void setSliders(int stepNr, QSlider *a, QSlider *b, QLabel *aLabel, QLabel *bLabel, int scalarSetIndex);
	void changeRange();
	void changeTab(int current);
	void changeTreeDepth(int newDepth);
	void resetViewTree();
	void switchCam();
	void openScene();
	void addBVH();
	void addScalars();
	void resetControlPanel();
	void updateNearPlane(const double &n);
	void updateFarPlane(const double &f);

private:
	QString cameraDialog();
	void connectControlPanelSignals(int index);
	void createActions();
	void createMenus();
	void fillStats(QVBoxLayout *w);
	float recalculateValue(const float &val);
	void setScalars(QWidget *parent);
	void setSceneStats();
	void setTreeDepthRange(QWidget *parent);
	void showControlPanel();
	void unconsistentBVHDialog();

	Ui::MainWindow *ui;
	vector<ControlPanel*> controlPanelTabs;
	QAction *openSceneAct, *openBVHAct, *exitAct;
	QDoubleSpinBox *nearPlane, *farPlane;
	QMenu *fileMenu;
	QPushButton *resViewT, *changeCam, *addScalarsButton;
	QTabWidget *tabWidget;
	QString sceneFile;
	QWidget *bvh, *widget3D;

	int mCurrentScalarSet;
	int mCurrentTab;
	float mScalarMax;
	float mSliderStepAdd;
	float mSliderStepMult;
};

#endif // MAINWINDOW_H
