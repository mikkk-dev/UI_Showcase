## UI Showcase 
It's a project I made to hone my Unreal Engine UI and C++ skills.
Right now it features a Level called _KeyboardMap.umap_ and when you play it a speed typing test appears.

### Video Preview
[https://www.youtube.com/watch?v=fLOeLIo31PU](https://www.youtube.com/watch?v=fLOeLIo31PU)

### The functionality:
- Counting typed words and typos
- Calculating words per minute and accuracy
- Highlighting the expected key on a keyboard
- Delete functionality (deletes last letter)
- Ctrl + Delete functionality (deletes the whole word)
- Spacebar skipping to next word
- Binding to text to the current word untill a space is pressed
- Sound and key animation when typing

### The important files:
The code I wrote is in these 4 files:
- [Source/UI_Showcase/UKeyboardCanvasManager.h](https://github.com/mikkk-dev/UI_Showcase/blob/main/Source/UI_Showcase/UKeyboardCanvasManager.h)
- [Source/UI_Showcase/UKeyboardCanvasManager.cpp](https://github.com/mikkk-dev/UI_Showcase/blob/main/Source/UI_Showcase/UKeyboardCanvasManager.cpp)
- [Source/UI_Showcase/UKeyboardButtonWidget.h](https://github.com/mikkk-dev/UI_Showcase/blob/main/Source/UI_Showcase/UKeyboardButtonWidget.h)
- [Source/UI_Showcase/UKeyboardButtonWidget.cpp](https://github.com/mikkk-dev/UI_Showcase/blob/main/Source/UI_Showcase/UKeyboardButtonWidget.cpp)
