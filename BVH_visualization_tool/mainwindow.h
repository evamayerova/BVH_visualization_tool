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
#include <QProgressDialog>
#include <QElapsedTimer>
#include <string>
#include "src\TreeRender.h"
#include "src\SceneRender.h"
#include "src\ControlPanel.h"

#include "defines.h"

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
	void handleDisplayMode(int index);
	void setSliders(int stepNr, QSlider *a, QSlider *b, QLabel *aLabel, QLabel *bLabel, int scalarSetIndex);
	void changePolynomDegree(double value);
	void changeRange();
	void changeTab(int current);
	void changeTreeDepth(int newDepth);
	void initTabWidget();
	void resetViewTree();
	void switchCam();
	void switchBlendType(int type);
	void openScene();
	void addBVH();
	void addScalars();
	void resetControlPanel();
	void updateNearPlane(const double &n);
	void updateFarPlane(const double &f);
	void topScreenshot();
	void closeEvent(QCloseEvent *event);

private:
	QString cameraDialog();
	void connectControlPanelSignals(int index);
	void createActions();
	void createMenus();
	float recalculateValue(const float &val);
	void setScalars();
	void setSceneStats();
	void setTreeDepthRange();
	void setDisplayModes();
	void showControlPanel(const QString &builderName = "tab");
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
	std::vector<float> mSliderStepAdd;
	std::vector<float> mSliderStepMult;
	float polynomDegree;

	// measuring
	QElapsedTimer timer;
};

#endif // MAINWINDOW_H
