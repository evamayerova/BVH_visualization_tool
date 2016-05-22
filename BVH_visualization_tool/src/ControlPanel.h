#pragma once

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QScrollArea>
#include <QDoubleSpinBox>

struct CurrentNodeStats {
	CurrentNodeStats(QWidget *parent)
	{
		container = new QGroupBox("Selected node", parent);
		layout = new QVBoxLayout(container);
		index = new QLabel(container);
		bounds = new QLabel(container);
		triangleNr = new QLabel(container);
		
		layout->addWidget(index);
		layout->addWidget(bounds);
		layout->addWidget(triangleNr);

		container->setLayout(layout);
	}
	QGroupBox *container;
	QVBoxLayout *layout;
	QLabel *index, *bounds, *triangleNr;
};

struct CurrentTreeStats {
	CurrentTreeStats(QWidget *parent)
	{
		container = new QGroupBox("Tree and scene stats", parent);
		layout = new QVBoxLayout(container);
		triangleCountLabel = new QLabel(container);
		trianglesPerLeaf = new QLabel(container);
		treeDepth = new QLabel(container);
		importedBVHnodeCount = new QLabel(container);
		realBVHnodeCount = new QLabel(container);

		layout->addWidget(triangleCountLabel);
		layout->addWidget(realBVHnodeCount);
		layout->addWidget(importedBVHnodeCount);
		layout->addWidget(trianglesPerLeaf);
		layout->addWidget(treeDepth);

		container->setLayout(layout);
	}
	QGroupBox *container;
	QVBoxLayout *layout;
	QLabel *importedBVHnodeCount, *realBVHnodeCount, *triangleCountLabel, *trianglesPerLeaf, *treeDepth;
};

struct ScalarValuesGUI {
	ScalarValuesGUI(QWidget *parent);
	QGroupBox *container;
	QVBoxLayout *layout;
	QLabel *localMin, *localMax;
	QComboBox *scalars;
	QSlider *first, *second;
	QLabel *polynomDegreeLabel;
	QDoubleSpinBox *polynomDegree;
	QPushButton *addScalarsButton;
};

struct TreeDepth {
	TreeDepth(QWidget *parent)
	{
		container = new QWidget(parent);
		QHBoxLayout *l = new QHBoxLayout(container);
		label = new QLabel("Maximal tree depth", container);
		depthHolder = new QSpinBox(container);
		l->addWidget(label);
		l->addWidget(depthHolder);

	}
	QWidget *container;
	QLabel *label;
	QSpinBox *depthHolder;
};

struct BlendingType {

	BlendingType(QWidget *parent) {
		groupBox = new QGroupBox("When nodes overlap, show:", parent);

		maxVal = new QRadioButton("M&aximum value");
		minVal = new QRadioButton("M&inimum value");
		aveVal = new QRadioButton("A&verage value");
		topVal = new QRadioButton("T&op value");

		maxVal->setChecked(true);

		QVBoxLayout *l = new QVBoxLayout();
		l->addWidget(maxVal);
		l->addWidget(minVal);
		l->addWidget(aveVal);
		l->addWidget(topVal);

		groupBox->setLayout(l);
	}

	QGroupBox *groupBox;
	QRadioButton *maxVal;
	QRadioButton *minVal;
	QRadioButton *aveVal;
	QRadioButton *topVal;
};

struct DisplayMode {

	DisplayMode(QWidget *parent)
	{
		container = new QGroupBox("Display mode", parent);
		QVBoxLayout *l = new QVBoxLayout();
		displayModes = new QComboBox(container);
		l->addWidget(displayModes);

		container->setLayout(l);
	}

	QGroupBox *container;
	QComboBox *displayModes;
};

struct ScreenShots {
	ScreenShots(QWidget *parent);
	QWidget *container;
	QPushButton *topScreen;
	QPushButton *bottomScreen;
};

class ControlPanel {
public:
	ControlPanel();

	QScrollArea *scrollArea;
	QWidget *container;
	CurrentTreeStats *treeStats;
	CurrentNodeStats *currNodeStats;
	ScalarValuesGUI *scalars;
	TreeDepth *treeDepth;
	BlendingType *blendingType;
	DisplayMode *displayMode;
	ScreenShots *screenShots;
};
