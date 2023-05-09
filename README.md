# spotlight
Spotlight lets you dim the entire screen except for a circular region around the mouse. Useful during Screen sharing and presentations when you want to bring the focus to a specific point.

The spotlight can be toggled through a global `Alt + S` hotkey.

## Images

![](https://cdn.discordapp.com/attachments/705749860615454821/1105558739030917273/Screenshot_16.png)

![](https://cdn.discordapp.com/attachments/705749860615454821/1105558739337093191/Screenshot_15.png)

![](https://cdn.discordapp.com/attachments/705749860615454821/1105560810954162308/chrome_COu2anpRRY.gif)

## Build Instructions

If you are only interested in the building the application
```
git clone https://github.com/StableSteady/spotlight.git
cd spotlight
mkdir install
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH="Path/to/Qt/ver/arch" -DCMAKE_INSTALL_PREFIX="..\install" ..
cmake --build . --config Release
cmake --install .
```
This will install the application to install/bin along with all the neccesary DLLs.

To build the application from Visual Studio, you either need to manually add the DLLs to the build output folder or add the Qt bin folder to the path.
You can just generate the VS solution and not install the application by stopping at
```
cmake -DCMAKE_PREFIX_PATH="Path/to/Qt/ver/arch" ..
```
