# Switcher
Switch Between Multiple Version Of PHP on windows. It basically just modifies the `USER PATH` Environmental Variable for you whenever you want to switch to a different PHP version. It saves all versions in a `database.txt` file in same folder.

> Make sure there is no path of any PHP version already added to `SYSTEM PATH` & `USER PATH` both Environmental Variables. Because this tool won't ever effect `SYSTEM PATH`. But it is recommended to keep a backup of `USER PATH`

## Usage:
just copy and paste `switcher.exe` file somewhere where you can access it easily.

> Recommended: Put it inside a separate folder and add that folder's path to `PATH` Environmental Variable (So you can access it everywhere)

### Switching Versions

1. Add a version to switcher's local database 
```
switcher add --version=74 --path=C:\tools\php74
```
2. Activate (switch to) the version any time like this
```
switcher set --version=74
```
### Reseting Versions
You can deactivate all activate versions this way
```
switcher unset
```
### List Available Versions
You can list all version available in database
```
switcher list
```
### Get Help
```
switcher --help
```
