#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing.h>


#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <interaction_lib/InteractionLib.h>
#include <interaction_lib/misc/InteractionLibPtr.h>

#include "virtualMose.h"


using namespace dlib;
using namespace std;

//시선: 화면 매핑, 그리드 생성에 이용활 화면 크기
int SCREEN_WIDTH = GetSystemMetrics(SM_CXSCREEN);
int SCREEN_HEIGHT = GetSystemMetrics(SM_CYSCREEN);



//입의 거리를 판별하는데 쓰이는 thresh;
int TEMP_THRESH = 10;
int POINTER_SENSITIVE = 10;



//입이 벌려지기 전의 좌표;
Cordination bufferCordination;
//이전 좌표
Cordination precord;




int CAPWIDTH = 320;
int CAPHEIGHT = 240;

//###########################################################################################################UI창

std::vector<cv::Mat> getButtons() {//버튼 이미지를 가져오고, 리사이징을 해준다.
    std::vector<cv::Mat> buttons;
    string baseDir = "./Images/newColor/";
    for (int i = 0; i < 3; i++) {
        buttons.push_back(cv::imread(baseDir+"button"+ to_string(i + 1) + ".png", cv::IMREAD_COLOR));
        CV_Assert(buttons[i].data);

        cv::resize(buttons.at(i), buttons.at(i), cv::Size(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2));

    }
    //다른 크기의 버튼
    //up
    buttons.push_back(cv::imread(baseDir+"button4.png", cv::IMREAD_COLOR));
    cv::resize(buttons.at(3), buttons.at(3), cv::Size((SCREEN_WIDTH / 2) / 2, (SCREEN_HEIGHT / 2) / 2));
    //down
    buttons.push_back(cv::imread(baseDir+"button5.png", cv::IMREAD_COLOR));
    cv::resize(buttons.at(4), buttons.at(4), cv::Size((SCREEN_WIDTH / 2) / 2, (SCREEN_HEIGHT / 2) / 2));
    //back
    buttons.push_back(cv::imread(baseDir+"button6.png", cv::IMREAD_COLOR));
    cv::resize(buttons.at(5), buttons.at(5), cv::Size((SCREEN_WIDTH / 2) / 2, SCREEN_HEIGHT / 2));


    return buttons;
}



std::vector<cv::Rect> getRects() {//버튼의 크기에 맞는 rect를 생성해준다. 
    std::vector<cv::Rect> rects;


    rects.push_back(cv::Rect(cv::Point(0, 0), cv::Point(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2)));
    rects.push_back(cv::Rect(cv::Point(SCREEN_WIDTH / 2, 0), cv::Point(SCREEN_WIDTH, SCREEN_HEIGHT / 2)));
    rects.push_back(cv::Rect(cv::Point(0, SCREEN_HEIGHT / 2), cv::Point(SCREEN_WIDTH / 2, SCREEN_HEIGHT)));

    //up
    rects.push_back(cv::Rect(cv::Point(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2), cv::Point((SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 2) / 2, (SCREEN_HEIGHT / 2) + (SCREEN_HEIGHT / 2) / 2)));
    //down
    rects.push_back(cv::Rect(cv::Point(SCREEN_WIDTH / 2, (SCREEN_HEIGHT / 2) + (SCREEN_HEIGHT / 2) / 2), cv::Point((SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 2) / 2, SCREEN_HEIGHT)));
    //back
    rects.push_back(cv::Rect(cv::Point((SCREEN_WIDTH / 2) + (SCREEN_WIDTH / 2) / 2, SCREEN_HEIGHT / 2), cv::Point(SCREEN_WIDTH, SCREEN_HEIGHT)));




    return rects;
}

//버튼 Mat 이미지와 좌표
static cv::Mat UI(cv::Size(SCREEN_WIDTH, SCREEN_HEIGHT), CV_8UC3);
static std::vector<cv::Mat> buttons = getButtons();
static std::vector<cv::Rect> buttonRects = getRects();
static int SELECTED_BUTTON_ID = -1;



void uiMouseCallback(int event, int x, int y, int flag, void*) {

    cv::Point currentPoint(x, y);

    for (int i = 0; i < buttons.size(); i++) {
        if (buttonRects.at(i).contains(currentPoint)) {
            cv::Mat temp = buttons.at(i) + cv::Scalar(50, 50, 50);
            temp.copyTo(UI(buttonRects.at(i)));
            SELECTED_BUTTON_ID = i;

        }
        else {

            buttons[i].copyTo(UI(buttonRects.at(i)));
        }
    }


    cv::namedWindow("UI", cv::WINDOW_FULLSCREEN);
    cv::setMouseCallback("UI", uiMouseCallback);
    cv::imshow("UI", UI);

}
//##############################################################################################################################################State 창을 위한 UI
std::vector<cv::Mat> getStateButtons() {//버튼 이미지를 가져오고, 리사이징을 해준다.
    std::vector<cv::Mat> buttons;
    string baseDir = "./Images/stateUI/";
    for (int i = 0; i < 4; i++) {
        buttons.push_back(cv::imread(baseDir + "button" + to_string(i + 1) + ".png", cv::IMREAD_COLOR));
        CV_Assert(buttons[i].data);

        cv::resize(buttons.at(i), buttons.at(i), cv::Size(CAPWIDTH/2, 50));
    }
    return buttons;
}

std::vector<cv::Rect> getStateRects() {//버튼의 크기에 맞는 rect를 생성해준다. 
    std::vector<cv::Rect> rects;
    rects.push_back(cv::Rect(cv::Point(0, CAPHEIGHT), cv::Point(CAPWIDTH / 2, CAPHEIGHT + 50)));
    rects.push_back(cv::Rect(cv::Point(CAPWIDTH / 2, CAPHEIGHT), cv::Point(CAPWIDTH, CAPHEIGHT + 50)));
    rects.push_back(cv::Rect(cv::Point(0, CAPHEIGHT + 50), cv::Point(CAPWIDTH / 2, CAPHEIGHT + 100)));
    rects.push_back(cv::Rect(cv::Point(CAPWIDTH / 2, CAPHEIGHT + 50), cv::Point(CAPWIDTH, CAPHEIGHT + 100)));
    return rects;
}




cv::Mat State = cv::Mat(cv::Size(CAPWIDTH, CAPHEIGHT + 100), CV_8UC3, cv::Scalar(0, 0, 0));
static std::vector<cv::Mat> Statebuttons = getStateButtons();
static std::vector<cv::Rect> StatebuttonRects = getStateRects();
static int SELECTED_State_BUTTON_ID = -1;

void stateMouseCallback(int event, int x, int y, int flag, void*) {

    cv::Point currentPoint(x, y);

    for (int i = 0; i < Statebuttons.size(); i++) {
        if (StatebuttonRects.at(i).contains(currentPoint)) {
            cv::Mat temp = Statebuttons.at(i) + cv::Scalar(50, 50, 50);
            temp.copyTo(State(StatebuttonRects.at(i)));
            SELECTED_State_BUTTON_ID = i;
        }
        else {

            Statebuttons[i].copyTo(State(StatebuttonRects.at(i)));
        }
    }

    if (event == cv::EVENT_LBUTTONDOWN) {
        switch (SELECTED_State_BUTTON_ID)
        {
        case 0:
            if(5<POINTER_SENSITIVE)
            POINTER_SENSITIVE -= 5;

            SELECTED_State_BUTTON_ID = -1;
            break;
        case 1:
            POINTER_SENSITIVE += 5;
            SELECTED_State_BUTTON_ID = -1;
            break;
        case 2:
            if (5 < TEMP_THRESH)
                TEMP_THRESH -= 5;
            SELECTED_State_BUTTON_ID = -1;
            break;
        case 3:
            TEMP_THRESH += 5;
            SELECTED_State_BUTTON_ID = -1;
            break;
        default:
            break;
        }
       
        cout << "thresh: " << TEMP_THRESH << " sensitive: " << POINTER_SENSITIVE << endl;
    }
   
    cv::namedWindow("State", cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback("State", stateMouseCallback);
    cv::imshow("State", State);

}



//##########################################################################################################################################################

int main()
{
    //main loop에서 입이 열렸는지 확인해주는 변수
    bool IS_MOUTH_OPENED = false;
  
    HWND winFocus = NULL;//focus를 저장하기위한 윈도우핸들변수
  
    boolean UIFirstFrame = true;//UI창의 첫 프레임임을 알려준다.
    boolean FaceFirstFrame = true;

    // create the interaction library
    IL::UniqueInteractionLibPtr intlib(IL::CreateInteractionLib(IL::FieldOfUse::Interactive));

    constexpr float offset = 0.0f;
    intlib->CoordinateTransformAddOrUpdateDisplayArea(SCREEN_WIDTH, SCREEN_HEIGHT);
    intlib->CoordinateTransformSetOriginOffset(offset, offset);
    // subscribe to gaze point data; print data to stdout when called

   
    intlib->SubscribeGazePointData([](IL::GazePointData evt, void* context)
        {

            moveCursor(evt.x, evt.y, &precord,POINTER_SENSITIVE);

        }, nullptr);

 
    ///////////////////////////////////////////////////facial landmark detection


    cv::VideoCapture cap(0);
    cv::Mat detectResult;
    cv::Mat temp;
   
    
    //

    for (int i = 0; i < 4; i++) {
        buttons[i].copyTo(UI(buttonRects.at(i)));
        Statebuttons[i].copyTo(State(StatebuttonRects.at(i)));
    }
    
    
  


    cap.set(cv::CAP_PROP_FRAME_HEIGHT, CAPHEIGHT);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, CAPWIDTH);
    cap.read(detectResult);
   

   
    
   

    
    
    if (!cap.isOpened())
    {
        cerr << "Unable to connect to camera" << endl;
        return 1;
    }


    try
    {
        

      

        // Load face detection and pose estimation models.
        frontal_face_detector detector = get_frontal_face_detector();
        shape_predictor pose_model;
        deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;

        // Grab and process frames until the main window is closed by the user.
        int keyValue = 0;
        while (keyValue!=27)
        {
            
            
            if (keyValue==13) {
                keyValue = cv::waitKey(0);
                continue;
            }
            //눈 좌표 업데이트
            intlib->WaitAndUpdate();


            keyValue = cv::waitKey(30);
            
            // Grab a frame
            //cout << "key" << keyValue << endl;
            cap.read(temp);
            //입술을 찾은 결과를 보여줄 Mat
            temp.copyTo(detectResult);
           
            
           


            // Turn OpenCV's Mat into something dlib can deal with.  Note that this just
            // wraps the Mat object, it doesn't copy anything.  So cimg is only valid as
            // long as temp is valid.  Also don't do anything to temp that would cause it
            // to reallocate the memory which stores the image as that will make cimg
            // contain dangling pointers.  This basically means you shouldn't modify temp
            // while using cimg.
            cv_image<bgr_pixel> cimg(temp);

            // Detect faces 
            std::vector<rectangle> faces = detector(cimg);//이미지를 읽어온 뒤 dtector 내부에서 얼굴의 영역을 박싱해준다.
            // Find the pose of each face.

            std::vector<full_object_detection> shapes;
           //얼굴의 개수만큼 반복하며 pose_model을 통해 얼굴마다 pose를 찾아준다.

            std::vector<cv::Point> mouthPoints;//입 랜드마크의 좌표를 담고있을 vector


            if (0 < faces.size()) {//하나 이상일때 하나만 처리(마우스는 한사람만 이용함)
              
              
                shapes.push_back(pose_model(cimg, faces[0]));
              
                for (int i = 49;i<shapes.at(0).num_parts();i++) {
                    //cout << shapes.at(0).part(i);

                    cv::Point mouthPoint(shapes.at(0).part(i).x(), shapes.at(0).part(i).y());
                    mouthPoints.push_back(mouthPoint);
                    cv::circle(detectResult, mouthPoint,1,cv::Scalar(100,255,50,255),-1,cv::LINE_AA);

                    
                }
                

                //todo: mouthPoint를 이용해 입술두깨:윗입술과 아랫 입술의 거리>thresh mouthOpen flag 켜주기
                
                cv::Point LipUpperPoint1(shapes.at(0).part(50).x(), shapes.at(0).part(50).y());
                cv::Point LipUpperPoint2(shapes.at(0).part(61).x(), shapes.at(0).part(61).y());

                std::vector<cv::Point> upperLipPoints;
                upperLipPoints.push_back(LipUpperPoint1);
                upperLipPoints.push_back(LipUpperPoint2);

                cv::Point LipLowerPoint1(shapes.at(0).part(65).x(), shapes.at(0).part(65).y());
                cv::Point LipLowerPoint2(shapes.at(0).part(56).x(), shapes.at(0).part(56).y());
                std::vector<cv::Point> LowerLipPoints;
                LowerLipPoints.push_back(LipLowerPoint1);
                LowerLipPoints.push_back(LipLowerPoint2);

                //입술의 거리에따라 입이 열렸는지 열리지 않았는지 판별,
                bool isExeeded = isyPixelExceededThresh(upperLipPoints,LowerLipPoints,TEMP_THRESH);
               
               // cout << " diffedY: " << diffedY << endl;

                if (isExeeded) {
                    IS_MOUTH_OPENED = true;

                }
                else {
                    IS_MOUTH_OPENED = false;
                }

                //현재 상태
                //
                cv::putText(detectResult, "( S:" + to_string(POINTER_SENSITIVE) + " ) " + "(T: " + to_string(TEMP_THRESH) + " px)", cv::Point(10,30), cv::FONT_ITALIC, 0.6, cv::Scalar(30, 255, 50, 255), 1, cv::LINE_AA);

                if (IS_MOUTH_OPENED) {

                    if (UIFirstFrame) {
                        bufferCordination = precord;
                        //std::cout << preXcord << "::" << preYcord << std::endl;
                        cv::destroyWindow("State");
                        cv::namedWindow("UI", cv::WINDOW_NORMAL);
                        cv::setWindowProperty("UI", cv::WND_PROP_FULLSCREEN, cv::WINDOW_FULLSCREEN);
                        cv::setMouseCallback("UI", uiMouseCallback);
                        cv::moveWindow("UI", 0, 0);
                        cv::imshow("UI", UI);
                        winFocus = FindWindow(NULL, TEXT("UI"));//UI윈도우의 핸들을 받아온다.
                        SetWindowPos(winFocus,HWND_TOPMOST,0,0,0,0, SWP_NOSIZE |SWP_NOMOVE);//UI윈도우를 최상위로 배치한다.
                        UIFirstFrame = false;
                        FaceFirstFrame = true;
                    }
                    
                    sleep(10);
                }
                else {
                    if (FaceFirstFrame) {
                      

                        cv::destroyWindow("UI");
                        cv::namedWindow("State", cv::WINDOW_AUTOSIZE);
                        cv::setMouseCallback("State", stateMouseCallback);
 

                        winFocus = FindWindow(NULL, TEXT("State"));//State윈도우의 주소를 받아온다.
                        SetWindowPos(winFocus, HWND_TOPMOST, SCREEN_WIDTH - 300, SCREEN_HEIGHT - 340, 0, 0, SWP_NOSIZE|SWP_NOMOVE);//State 윈도우를 최상단으로 배치한다.
                        DWORD style = ::GetWindowLong(winFocus, GWL_STYLE);
                        style &= ~WS_OVERLAPPEDWINDOW;
                        style |= WS_POPUP;
                        ::SetWindowLong(winFocus, GWL_STYLE, style);
                        doSelectedmouseAction(SELECTED_BUTTON_ID, bufferCordination.X, bufferCordination.Y);
                        FaceFirstFrame = false;
                        UIFirstFrame = true;

                    }

                        
                   
                    cv::moveWindow("State", SCREEN_WIDTH - 320, SCREEN_HEIGHT -340);
                    
                      
                       cv::resize(detectResult, detectResult, cv::Size(CAPWIDTH, CAPHEIGHT));
                       detectResult.copyTo(State(cv::Rect(cv::Point(0, 0), cv::Size(CAPWIDTH, CAPHEIGHT))));
                       cv::imshow("State", State);
                 
                    
              
                  
                  

                    
                }
              
            }
            else {
                cv::moveWindow("State", SCREEN_WIDTH - 320, SCREEN_HEIGHT - 340);
                State(cv::Rect(cv::Point(0, 0), cv::Size(CAPWIDTH, CAPHEIGHT))) = cv::Scalar(89,64,2);
                cv::putText(State,"No face detected",cv::Point(10,CAPHEIGHT/2),cv::FONT_HERSHEY_COMPLEX,1,cv::Scalar(255,255,255),1,cv::LINE_AA);
               // cv::resize(detectResult, detectResult, cv::Size(280, 150));
                cv::imshow("State", State);
            
            }
            

        }

       
    }
    catch (serialization_error& e)
    {
        cout << "You need dlib's default face landmarking model file to run this example." << endl;
        cout << "You can get it from the following URL: " << endl;
        cout << "   http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << endl;
        cout << endl << e.what() << endl;
    }
    catch (exception& e)
    {
        cout << e.what() << endl;
    }


    cap.release();
    cv::destroyAllWindows();
    return 0;
}


