# Xplorer
Xplorer is set of utilities for 3ds max.
Builded with VS2015.

Supported 3ds max versions: **2010 - 2019**

Tested on 3ds max versions: **2017**

Current version utilities:

- ColorMask (Decompose) : is texmap class that will decompose one channel from RGBA input
- ColorVar (Animated Color) : is texmap class for constant color output

# Building
Requires [PreCore](https://github.com/PredatorCZ/PreCore) to be placed alongside this repo folder clone.

Building will create _ObjDump and _BinDump folders alongside this repo folder clone.

## Editing .vcxproj
At **PropertyGroup Label=Globals** group, locate **MaxSDK** field, change path where is your MAX SDK installation.
If your MAX SDK installation is somewhere else than default path stated in this field, you can edit path here.

At **PropertyGroup Label=Configuration** group, locate both **OutDir** fields, there is conditional between 3ds max plugins path or _BinBump folders, I'm debugging plugins for 3ds max 2017, but you can change it to any version you're using.

Example for version 2017 (default):

```
<OutDir Condition="'$(Configuration)|$(Platform)'=='2017|x64'">C:\Program Files\Autodesk\3ds Max 2017\Plugins\</OutDir>
<OutDir Condition="'$(Configuration)|$(Platform)'!='2017|x64'">$(SolutionDir)..\_BinDump\$(SolutionName)_$(Platform)_$(Configuration)\</OutDir>
```

Example for version 2019:


```
<OutDir Condition="'$(Configuration)|$(Platform)'=='2019|x64'">C:\Program Files\Autodesk\3ds Max 2019\Plugins\</OutDir>
<OutDir Condition="'$(Configuration)|$(Platform)'!='2019|x64'">$(SolutionDir)..\_BinDump\$(SolutionName)_$(Platform)_$(Configuration)\</OutDir>
```

