# Motion-sensing Game Control System Based On Bone Point Recognition

## Introduction

This system is a motion-sensing shooting game control system developed based on Kinect.

The system combines Kinect and traditional shooting game FPS, allowing users to have an immersive shooting experience in traditional shooting games. 

In this system, the Kinect V2 vision sensor can obtain the three-dimensional information of human bones, including the horizontal and vertical information of each joint and the depth information, and feed the information back to the server for interaction. 

The control system running on the server will link the obtained information with the virtual button, so as to realize the character walking and firing in FPS. While providing users with immersive experience of FPS games, this system can effectively enable users to exercise their joints while playing games. The spatial movement of users is linked with the left and right swing of the head, which can effectively relieve the tension of users' cervical vertebrae, and the left hand fist is linked with shooting and firing, which can effectively improve the flexibility of users' finger joints. Right hand movement exercises the right hand joint.

<img src="https://i.imgur.com/VhPqYo0.png" width="60%">

## Functions

WASD keyboard key control.
Mouse click control.
Mouse motion mapping.
Mouse tracking & control.

## Demonstration

**Mouse control:** Hands can be operated in the red box to control the mouse movement to cut the fruit game. At the same time, with the left hand trigger to press the left button, the right hand can smoothly control the mouse to write on the computer.

<img src="https://i.imgur.com/6NSq0nP.png" width="80%">

<img src="https://i.imgur.com/cKIP4NG.png" width="80%">

**keyboard control:** We use notepad typing to show the control of "W", "A", "S" and "D" (The figure shows the head leaning forward, so the bone information obtained is a straight line projected on the screen.)

<img src="https://i.imgur.com/zc4909F.png" width="80%">

**Mouse clicking control:** After the left hand makes a fist, the left hand status is updated to "fire! And successfully triggered the fire, the fire process can be continuous, that is, continue to maintain a fist state, fire without interruption, which is consistent with the results of continuous mouse press in actual FPS games.

<img src="https://i.imgur.com/DlH5ghI.png" width="50%">

Game: *AimLab*

<img src="https://i.imgur.com/f6SLkvb.png" width="50%">

## Algorithm Overview

### Acquisition of Bone Data

Kinect 2.0 can track up to 25 bone points in six people simultaneously.

<img src="https://i.imgur.com/HH6FCgy.png" width="70%">

### Character Control

After several rounds of tests, it was found that the bone data along the line from Head to SpineShoulder was relatively stable compared with other joints, so the two nodes of **3.Head** and **20.SpineShoulder** were chosen as the reference nodes for controlling the character.

### $Three-step Decision Method

In FPS, we need small, repetitive movements to move or fire. Taking forward movement as an example, I also set three steps to judge whether it is forward, but the actual judgment condition is only one. 

**The first step** is to set the result of pose recognition to succeed, so as to ensure that the following two steps are entered, and it is not necessary to put the head to the initial prescribed position each time, so that the action can be continuous. 

**The second step** is to set the condition to judge whether the head is leaning forward. If the condition is triggered, the defined key function is triggered to control the keyboard and achieve the function of advancing. 

**The third step** is the same as the second step, its purpose is to avoid the recognition error of Kinect, strengthen the recognition accuracy, make its recognition sensitivity higher, so that players have a better game experience.

### Control of Mouse Clicking

Clicking the mouse is a very common and frequent action in FPS games, so the simple gesture of clenching the fist or opening the palm is chosen for control, where the open palm indicates a cease fire and the window displays "cease fire!" . Clenched fists indicate fire, and the corresponding window displays "fire!" .

### Mouse Mapping Algorithm

After analysis, it is found that the actual motion range of the hand is not the whole huge visual space, and its actual motion range is very limited. On the other hand, we can define a range as the actual operation space of the hand, that is, only the points in this space and the pixels in the screen are mapped one-to-one. 

Therefore, we take the length from the neck to the head as the unit length, and draw a square box that is twice the length of the unit length and twice the width of the unit length, and the rectangular box is offset relative to the neck to facilitate hand operation. The reason why it is designed as a square is to match the number of pixels on the computer screen: in the operation space of the mouse, **the coordinates** **are divided into 65535 pixels in the horizontal and vertical**, so the number of pixels in the horizontal and vertical is 1:1, and the operation space can better establish a one-to-one mapping. 

Secondly, in order to make the follow-up mouse tracking process as stable as possible, the relatively stable point, namely the **HandRight** point, is identified for binding mapping with the mouse.

**In addition, the specific formula for establishing the mapping relationship is:**
$$
x1/XB=x2/XS
$$

$$
y1/YB=y2/YS
$$

<img src="https://i.imgur.com/ZDT89wy.png" width="80%">

### Mouse tracking control

This section uses the mouse_event function. mouse_event is a Windows API function that simulates mouse operations. It can trigger mouse events in Windows operating systems, such as clicking, moving, and scrolling. The mouse_event function is declared as follows:

```c++
void mouse_event (
    DWORD dwFlags,  // Specifies the flag for the mouse event
    DWORD dx,       // Horizontal position relative to the screen
    DWORD dy,       // Vertical position relative to the screen
    DWORD dwData,   // Mouse scrolls information or additional information
    ULONG_PTR dwExtraInfo  // extraneous information );
```

sample code:

```c++
#include <Windows.h>
void simulateMouseClick(int x, int y) {
    // Move the mouse to the specified position
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, x, y, 0, 0);
    // Simulate mouse pressing and releasing the left button
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);}
int main() {
    // Simulate the position of clicking on the screen (100, 100)
    simulateMouseClick(100, 100);
    return 0;
}
```

### Filtering Algorithm of Trajectory Smoothing

**Pseudocode:**

```
Algorithm: Discrete Gradient Descent Filter Optimizer
Input:
- handRight: The position of HandRight node in each frame
- filter: Filter coefficient
Output:
- Smoothed HandRight coordinate

Initialization:
sumx = 0
sumy = 0

For each frame cycle:
diffx = handRight.Position.X - x
diffy = handRight.Position.Y - y
sumx = sumx + diffx
sumy = sumy + diffy
x = x + sumx / filter
y = y + sumy / filter
sumx = 0
sumy = 0

Returns the smoothed HandRight coordinate.
```

## Get Started!

**Please refer to other documents in this Repositories for the use of Kinect equipment (Chinese version only).**

1. Open visual studio 2022 and open the project file.

<img src="https://i.imgur.com/bQaC1Dw.png" width="80%">

2. Click start execution (no debugging) (Ctrl+F5).

<img src="https://i.imgur.com/usqAakc.png" width="80%">

3. The program starts to run and the control interface pops up. Here take the game Aim Lab as an example. After entering the game, click "Game Motion controller" to start the Kinect device and obtain bone data.

   <img src="https://i.imgur.com/f2kmIUY.png" width="40%">