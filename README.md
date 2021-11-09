<h1 align="center" style="color:blue">
  Switcher
  <p align="center">
  <a href=""><img src="https://img.shields.io/static/v1?label=switcher&message=v1.15&color=red" alt="Version"></a>
  <a href=""><img src="https://img.shields.io/static/v1?label=size&message=2.16MB&color=blue" alt="Size"></a>
  <a href=""><img src="https://img.shields.io/github/downloads/back2Lobby/switcher/total.svg" alt="Total Downloads"></a>
  </p>
</h1>

<h3>About Switcher</h3>
Switch between multiple versions of PHP on windows. It basically just modifies the `USER PATH` Environmental Variable for you whenever you want to switch to a different PHP version. It saves all versions in a `database.txt` file in same folder.

> Make sure there is no path of any PHP version already added to `SYSTEM PATH` & `USER PATH` both Environmental Variables. Because this tool won't ever effect `SYSTEM PATH`. But it is recommended to keep a backup of `USER PATH`

![Switcher-min](https://user-images.githubusercontent.com/29447722/125731780-adeda02a-7643-4e61-b62a-53c6fa6836c0.gif)


## Usage:
Just copy and paste `switcher.exe` file somewhere where you can access it easily.

> Recommended: Put it inside a separate folder and add that folder's path to `PATH` Environmental Variable (So you can access it everywhere)

### Switching Versions

1. Add a version to switcher's local database 
```
switcher add --version=74 --path=C:\tools\php74
```
2. Activate (switch to) the version any time like this (Restart or Open a new terminal to see effects for CLI)
```
switcher set --version=74
```
### Resetting Versions
You can deactivate all activated versions this way
```
switcher unset
```
### List Available Versions
You can list all versions available in database
```
switcher list
```
### Get Help
```
switcher --help
```

### Build Instructions
If you want to build it then make sure to use -static-libgcc and -static-libstdc++ flags.
For example, 
```
g++ switcher.cpp -o switcher -static-libgcc -static-libstdc++
```
