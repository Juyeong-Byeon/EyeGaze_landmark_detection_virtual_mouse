
#include "virtualMose.h"
#include <dlib/image_processing.h>
#include <opencv2/opencv.hpp>

using namespace dlib;


void doSelectedmouseAction(int SELECTED_BUTTON_ID, int x, int y) {//���õ� ���콺 �̺�Ʈ�� �߻������ش�.
    SetCursorPos(x, y);

    switch (SELECTED_BUTTON_ID)
    {
    case 0:
        mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
        mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);
        dlib::sleep(10);
        break;
    case 1:
        mouse_event(MOUSEEVENTF_RIGHTDOWN, x, y, 0, 0);
        mouse_event(MOUSEEVENTF_RIGHTUP, x, y, 0, 0);
        dlib::sleep(10);
        break;
    case 2:
        mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
        mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);
        dlib::sleep(10);
        mouse_event(MOUSEEVENTF_LEFTDOWN, x, y, 0, 0);
        mouse_event(MOUSEEVENTF_LEFTUP, x, y, 0, 0);
        break;
    case 3:
        mouse_event(MOUSEEVENTF_WHEEL, x, y, 800, 0);
        break;
    case 4:
        mouse_event(MOUSEEVENTF_WHEEL, x, y, -800, 0);
        break;
    case 5:
        std::cout << "back";
        break;
    default:
        break;
    }

    std::cout << x<< ":"<<y << std::endl;
}


void moveCursor(double newX, double newY,Cordination* precord, int POINTER_SENSITIVE) {
    //todo Grid ���·� ������

    //������ǥ�� �⺻���� ����
    int x = (int)precord->X;
    int y = (int)precord->Y;
    //10px �̻� �� �Ϸ� ���������� ���ο� ������ update
    if (abs(x - newX) > POINTER_SENSITIVE || POINTER_SENSITIVE < abs(y - newY)) {
        x = (int)newX;
        y = (int)newY;

        precord->X = (int)newX;
        precord->Y = (int)newY;
    }

    SetCursorPos(x, y);
    dlib::sleep(5);

    
}


bool isPosExceededThresh(std::vector<cv::Point> upperLipPoints, std::vector<cv::Point> LowerLipPoints, double thresh) {
    bool exceeded = false;

    //�� �Լ��� �α�
    int upperLipThickness = sqrt(pow(upperLipPoints.at(0).x - upperLipPoints.at(1).x, 2) + pow(upperLipPoints.at(0).y - upperLipPoints.at(1).y, 2));
    //�Ʒ� �Լ��� �α�
    int lowerLipThickness = sqrt(pow(LowerLipPoints.at(0).x - LowerLipPoints.at(1).x, 2) + pow(LowerLipPoints.at(0).y - LowerLipPoints.at(1).y, 2));
    //�Լ� �α��� ���
    double thicknessAverage = float(upperLipThickness + lowerLipThickness) / (float)2;
    //�� �Լ��� �Ʒ� �Լ��� �Ÿ�
    int distOfLips = sqrt(pow(upperLipPoints.at(1).x - LowerLipPoints.at(0).x, 2) + pow(upperLipPoints.at(1).y - LowerLipPoints.at(0).y, 2));


    double ratioOfTicknessAndDist = (float)distOfLips / (float)thicknessAverage;



    if ((thresh * 0.1) < ratioOfTicknessAndDist) {
        exceeded = true;
    }

    return exceeded;
}



bool isyPixelExceededThresh(std::vector<cv::Point> upperLipPoints, std::vector<cv::Point> LowerLipPoints, double thresh) {
    bool exceeded = false;

    int ydist = abs(upperLipPoints.at(1).y - LowerLipPoints.at(0).y);

    if (ydist > thresh) {
        exceeded = true;
    }


    return exceeded;
}



//void moveCursorGrid(double newX, double newY) {
//    //todo Grid ���·� ������
//
//
//    //������ǥ�� ���� static
//    preXcord = 0;
//    preYcord = 0;
//    //������ǥ�� �⺻���� ����
//    int x = preXcord;
//    int y = preYcord;
//    //10px �̻� �� �Ϸ� ���������� ���ο� ������ update
//
//    int gridWidth = 10;
//    int gridHeight = 10;
//
//
//
//
//    x = ((int)newX / gridWidth) * gridWidth - (gridWidth / 2);
//    y = ((int)newY / gridHeight) * gridHeight - (gridHeight / 2);
//
//
//
//    SetCursorPos(x, y);
//
//}


