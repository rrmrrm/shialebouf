
# Description
A Desktop app which lets the user create UML classdiagrams with a drawing tablet or the mouse.

Uses a NeuralNetwork to substitute the last drawing with the UML element represented by it.
Only able to recognise canvas-elements one by one.

Provides simple options to edit, save and load the diagram.

## build & run instructions for ubuntu users:
### Download py and qt components
download  pip:
```
sudo apt install python3-pip
```

install with pip numpy and tensorflow
```
sudo pip install numpy
sudo pip install tensorflow
```

install qt
TODO

### Build

standing in UmlDrawer
```
mkdir build && cd build
qmake6 ../painter.pro
```

Copy the executable from UmlDrawer/buid to UmlDrawer/ with the following command:
(this step is needed, because the executable has to be in UmlDrawer becvause the executable expects the configuration files to be in the same folder as itself)
```
cp ./project ../
```
### Run
standing in UmlDrawer folder:
```
./project
```



