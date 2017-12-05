# File formats

## Assets folder

- maps
  + Map (.map)
  + Menu (.menu)
- objects
  + StaticObject: Pre-calculated move matrix (.sobj)
  + DynamicObject: Dynamic move matrix (.dobj)
  + ActiveObject: Multiple DynamicObjects (.aobj)
- fonts
  + Char: Character file (.char)
  + Font file (.font)
- shaders
  + GLSL vertex and fragment shader (v1.5)
- textures
  + Mostly png files (.png)
  
### Map and Menu

- Use # for comments
- Max line length: 255 char
- Strict orders: X->Y->T, O->I->(E, B)
- Whitespace start chars are not allowed

|Sign|Type|Format|
|:---:|----|-------|
|$|Meta|$ KEY VALUE...|
|X|Texture|X id filename (1) |
|Y|TextureBlock|Y id base top right bottom left (1) |
|T|Tile|T x y Y.id TileType (1) |
|S|Static Light|S id x y z strength rrggbb specular intensity reference visible (2) |
|L|Ligthing|L x y strength1 color1 strength2 color2 time (4) |
|O|Object Active|O id STATIC/DYNAMIC/ACTIVE filename|
|I|Object Instance|I id O.id STATIC/DYNAMIC/ACTIVE x y z alpha beta gamma SizeX SizeY SizeZ visible reference (2) |
|C|Coords|C id type x y|
|M|Message|M x y z rrggbb alpha FontSize message|
|R|Region|R xMin yMin xMax yMax Action.id maxUse itemRequired noSneek (5) (6) |
|A|TextComponent|A id x y z relativeX relativeY align message rrggbb alpha fontsize action (3) |
|B|ObjectComponent|A id x y z relativeX relativeY alignX I(ACTIVE).id action (3) |
|D|ImageComponent|D id path x y z height width action (3) |
|N|Action|N id ActionType value (7) |
|P|PhysicsArea|P id x y enable|
|E|Entity|E id aobjI.id ligth.id spellSpeed damage hp score floatFi0 hitboxRadius|

- (1) Texture, TextureBlock and Tile lines are only processed in maps and the first loaded menu file.
- (2) Reference: required and available only for dynamic objects.
- Meta:
  + `NAME` name
  + `AUTHOR` name
  + `DESCRIPTION` String (Use _ for space)
  + `AMBIENT` percent (0.0 - 1.0)
  + `SPAWN` x y z
  + `STATE` GameState (0: MENU/ 1: INGAME/ 2: PAUSE) *(optional, default: 0)*
  + `CURSOR` true/false *(optional, default: false)*
  + `SCROLL` true/false min max *(optional, default: false, 0, 0)*

- (3) Component action types:

|Action|Effect|
|------|------|
|0|(Nothing)|
|1|Open map selector|
|2|Open options|
|21|Set graphics setting|
|22|Set controls setting|
|3|Open credits|
|31|Open GitHub|
|4|Open main menu|
|5|Start game|
|-1|Quit game|

- (4) Unimplemented
- (5) Maximum use: n > 0 for n, -1 for infinity
- (6) Item reuired: item id or -1 for noting
- (7) Action lose id: -10000
- RelativeX, RelativeY and Align enums:

|Enum|Name|Value|Description|
|----|----|-----:|-----------|
|RelativeX|X_LEFT|-1|Relative to the left horizontal side of the screen|
|RelativeX|X_CENTER|0|Relative to the horizontal center of the screen|
|RelativeX|X_RIGHT|1|Relative to the right horizontal side of the screen|
|RelativeY|Y_TOP|-1|Relative to the top vertical side of the screen|
|RelativeY|Y_CENTER|0|Relative to the vertical center of the screen|
|RelativeY|Y_BOTTOM|1|Relative to the right vertical side of the screen|
|Align|ALIGN_LEFT|-1|The anchor point will be placed to the left side of the text|
|Align|ALIGN_CENTER|0|The anchor point will be placed to the center of the text|
|Align|ALIGN_RIGHT|1|The anchor point will be placed to the right side of the text|

- Reference points:

|Id|Name|
|:---:|----|
|0|(Nothing)|
|1|Floating item|
|2|Left hand|
|3|Coin floating|
|4|Left weapon light|
|5|Right hand|
|6|Top spikes|
|7|Player center|
|8|Entity floating|

- ActionType values: (-65536 = don't set)

|ActionType|Id|Arguments|
|:--------:|:---:|---------|
|TELEPORT|0|x y|
|DAMAGE|1|count|
|ADD_SCORE|2|count|
|SET_DOBJ|3|dobj.id x y z alpha beta gamma SizeX SizeY SizeZ visible reference|
|SET_AOBJ|4|aobj.id x y z alpha beta gamma SizeX SizeY SizeZ visible|
|SET_ITEM|5|itemId|
|SET_LIGHT|6|light.id x y z strength rrggbb specular intensity visible reference|
|OBJECT_PSX|7|id x y enabled|
|FINISH|8|Finish (Win)|
|LOSE|9|Lose|

### StaticObject and DynamicObject

|Sign|Type|Format|
|:---:|----|-------|
|$|Meta|$ KEY VALUE...|
|C|ColorPart|C id rrggbb alpha|
|B|ObjectPart|B x y z PartType C.id|

- Meta:
  + `POSITION` x y z
  + `ROTATION` alpha beta gamma
  + `SCALE` x y z
- Instances:
  + StaticObject -> StaticObjectInstance
  + DynamicObject -> DynamicObjectInstance

### ActiveObject

|Sign|Type|Format|
|:---:|----|-------|
|$|Meta|$ KEY VALUE...|
|C|ColorPart|C id rrggbb alpha|
|B|ObjectPart|B stateId x y z PartType C.id|

- Meta:
  + `POSITION` x y z
  + `ROTATION` alpha beta gamma
  + `SCALE` x y z
  + `SIZE` stateCount (It must be under the POSITION, ROTATION and the SCALE meta line.)
- Instance:
  + ActiveObject -> ActiveObjectInstance
- The first active object will be the player.

### Char

|Sign|Type|Format|
|:---:|----|-------|
|$|Meta|$ KEY VALUE...|
|B|ObjectPart|B x y z PartType C.id|

- Meta:
  + `WIDTH` width
  + `Y` deltaY
  
> Color = 0 means default color

### Font v1 (.font)

Format: 

```
	character filename
	%c %s
```

> NOTE: No comments enabled

### Font v2 (.font32)

*Unimplemented*

Format:

```
	characterId filename
	%u %s
```

### Vertex Shader

Special arguments:

|Argument name|Type|Description|
|-------------|----|-----------|
|ligthColor|uniform vec3 [28]|RGB color (0.0 - 1.0) |
|ligthInfo|uniform vec3 [28]|Specular 1:on/0:off, distFactor, lightIntensity|

## Data folder

> The program won't be able to run without an existing `data` folder.

### Options (data/options.dat)

 - Type: Binary file
 - Format: (default, -1.0 = nothing)
   + version (1)
   + msaa (16)
   + fullscreen (true)
   + windowedHeight (0 = auto)
   + windowedWidth (0 = auto)
   + cameraMovement (true)
   + moveLeft (A, LEFT)
   + moveRight (D, RIGHT)
   + jump (SPACE, W, UP)
   + sneek (SHIFT, S, DOWN)
   + attack (MOSE_LEFT, CONTROL_LEFT, CONTROL_RIGHT)
   + use (F)
   + spell1 (F1, 1)
   + spell2 (F2, 2)
   + spell3 (F3, 3)
   + menu (ESC, PAUSE)
 - See: menu.c for more info
 
### Records (data/records.dat)

 - Type: Text file
 - Format per line:
   + Map name
   + Record type (0: time, 1: score)
   + Record value (time: sec, score: count)
 - Separator: whitespace
 