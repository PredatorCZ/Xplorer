# Xplorer
Xplorer is set of utilities for 3ds max.
Builded with VS2015.

Supported 3ds max versions: **2010 - 2019**

Tested on 3ds max versions: **2017**

Current version utilities:

- ColorMask (Decompose) : is texmap class that will decompose one channel from RGBA input
- ColorVar (Animated Color) : is texmap class for constant color output

## Editing .vcxproj
All essential configurations are within **PropertyGroup Label="MAXConfigurations"** field.
- **MaxSDK**: changes path where is your MAX SDK installation.
If your MAX SDK installation is somewhere else than default path stated in this field, you can edit it here.
- **MaxDebugConfiguration**: changes 3ds max version and platform, so all necessary files are copied into plugin directory, this will enable post-build event.

## Post-build event
Change 3ds max installation path (Default is C:\Program Files\Autodesk\). Do not change anything else unless you know what you're doing!

### [Latest Release](https://github.com/PredatorCZ/Xplorer/releases/)
