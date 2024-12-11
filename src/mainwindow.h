#pragma once

#include <QMainWindow>
#include <QCheckBox>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include "realtime.h"
#include "utils/aspectratiowidget/aspectratiowidget.hpp"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    void initialize();
    void finish();

private:
    void connectUIElements();
    void connectParam1();
    void connectParam2();
    void connectParam3();
    void connectNear();
    void connectFar();
    void connectPerPixelFilter();
    void connectKernelBasedFilter();
    void connectBumpmapping();
    void connectUploadFile();
    void connectSaveImage();
    void connectExtraCredit();

    Realtime *realtime;
    AspectRatioWidget *aspectRatioWidget;
    QCheckBox *filter1;
    QCheckBox *filter2;
    QPushButton *uploadFile;
    QPushButton *saveImage;
    QSlider *p1Slider;
    QSlider *p2Slider;
    QSlider *p3Slider;
    QSpinBox *p1Box;
    QSpinBox *p2Box;
    QSpinBox *p3Box;
    QSlider *nearSlider;
    QSlider *farSlider;
    QDoubleSpinBox *nearBox;
    QDoubleSpinBox *farBox;

    //bumpmapping
    QCheckBox *bumpmapping;

    //Bezier:
    QButtonGroup* modeGroup;
    // Extra Credit:
    QCheckBox *ec1;
    QCheckBox *ec2;
    QCheckBox *ec3;
    QCheckBox *ec4;

private slots:
    void onPerPixelFilter();
    void onKernelBasedFilter();
    void onBumpmapping();
    void onUploadFile();
    void onSaveImage();
    void onValChangeP1(int newValue);
    void onValChangeP2(int newValue);
    // void onValChangeP3(float newValue);
    void onSpinBoxChangeP3(int boxValue);
    void onValChangeNearSlider(int newValue);
    void onValChangeFarSlider(int newValue);
    void onValChangeNearBox(double newValue);
    void onValChangeFarBox(double newValue);

    // Extra Credit:
    void onBezierCircle(int id);
    void onExtraCredit1();
    void onExtraCredit2();
    void onExtraCredit3();
    void onExtraCredit4();
};
