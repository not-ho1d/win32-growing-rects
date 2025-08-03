#include <windows.h>
#include <windowsx.h>
#include <iostream>
#include <random>
#include <chrono>
using namespace std;

LRESULT CALLBACK handleEvents(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

struct Shape{
    int id;
    COLORREF penColor;
    int left;
    int top;
    int right;
    int bottom;
};

struct Node{
    Shape rect;
    Node *next;
};

int i=0;
Node *start = nullptr,*head=nullptr;
HBRUSH winBg = CreateSolidBrush(RGB(255,255,255));


template<typename T>
void print(T value){
    cout << value << endl;
}

int generateRandomNumber(int start,int end){
    unsigned int seed = chrono::high_resolution_clock::now().time_since_epoch().count();

    mt19937 gen(seed);
    uniform_int_distribution<> dist(start, end);

    //cout << dist(gen) << endl;

    return dist(gen);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdArgs,int cmdShowFlag){
    
    //define window class
    WNDCLASS wc = {};
    wc.lpfnWndProc = handleEvents;
    wc.hInstance = instance;
    wc.lpszClassName = "window";
    wc.hbrBackground = winBg;
    wc.hCursor = LoadCursor(nullptr,IDC_ARROW);

    //register window class
    if(RegisterClass(&wc)){
        print("registerd window class");
    }

    HWND hwnd = CreateWindowEx(
        0,
        "window",
        "click inside!",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,CW_USEDEFAULT,800,600,
        nullptr,nullptr,instance,nullptr
    );

    SetTimer(hwnd,1,100,nullptr);
    ShowWindow(hwnd,cmdShowFlag);
    UpdateWindow(hwnd);

    //mainloop
    MSG msg;
    while(GetMessage(&msg,nullptr,0,0)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
LRESULT CALLBACK handleEvents(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam){
    switch(msg){
        case WM_PAINT:{
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd,&ps);

            Node *ptr = start;

            while(ptr!= nullptr){
                Shape &currentRect = ptr->rect;
                HPEN newPen = CreatePen(PS_SOLID,1,currentRect.penColor);
                HPEN oldPen = (HPEN) SelectObject(hdc,newPen);
                

                Rectangle(
                    hdc,
                    currentRect.left,
                    currentRect.top,
                    currentRect.right,
                    currentRect.bottom
                );

                SelectObject(hdc,oldPen);
                DeleteObject(newPen);

                ptr = ptr->next;
            }
            EndPaint(hwnd,&ps);
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            DeleteObject(winBg);
            break;
        case WM_LBUTTONDOWN:{
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            
            COLORREF penColor;
            switch(generateRandomNumber(1,10)){
                case 1:
                    penColor = RGB(255, 0, 0);
                    break;
                case 2:
                    penColor = RGB(0, 0, 255);
                    break;
                case 3:
                    penColor = RGB(255, 255, 0);
                    break;
                case 4:
                    penColor = RGB(0, 255, 255);
                    break;
                case 5:
                    penColor = RGB(255, 165, 0);
                    break;
                case 6:
                    penColor = RGB(255, 0, 255);
                    break;
                case 7:
                    penColor = RGB(128, 0, 128);
                    break;                
                case 8:
                    penColor = RGB(0, 128, 128);
                    break;
                case 9:
                    penColor = RGB(32, 248, 128);
                    break;
                default:
                    penColor = RGB(0, 255, 0);
                    break;
            }
            i++;
            Shape rect = {i,penColor,x,y,x+50,y+50};

            Node *newNode = new Node({});
            newNode -> rect = rect;
            newNode -> next = nullptr;

            if(start == nullptr){
                start = newNode;
                head = newNode;
            }else{
                head -> next = newNode;
                head = newNode;
            }
            RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOERASE);
            break;
        }
        case WM_TIMER:{

            Node *ptr = start;
            while(ptr != nullptr){
                Shape &currentRect = ptr->rect;
                currentRect.top-=2;
                currentRect.left-=2;
                currentRect.right+=2;
                currentRect.bottom+=2;

                RECT rect;
                GetClientRect(hwnd,&rect);
                int width = rect.right-rect.left;
                int height = rect.bottom-rect.top;

                if(currentRect.top < 0 && currentRect.left < 0 &&
                currentRect.right > width && currentRect.bottom > height
                ){
                    Node *temp = start;
                    start = start -> next;
                    ptr = start;
                    print("removed rect");
                    delete temp;
                }else{
                    ptr = ptr -> next;
                }
            }
        }
            RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOERASE);
            break;
        default:
            return DefWindowProc(hwnd,msg,wParam,lParam);
    }
}