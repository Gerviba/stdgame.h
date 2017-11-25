# Assets - File formats

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
- Strict orders: X->Y->T, O->I
- Whitespace start chars not allowed

|Sign|Type|Format|
|:---:|----|-------|
|$|Meta|$ KEY VALUE...|
|X|Texture|X id filename (1) |
|Y|TextureBlock|Y id base top right bottom left (1) |
|T|Tile|T x y Y.id TileType (1) |
|S|Static Light|S x y strength rrggbb specular intensity reference (2)|
|L|Ligthing|L x y strength1 color1 strength2 color2 time|
|O|Object Active|O id STATIC/DYNAMIC/ACTIVE filename|
|I|Object Instance|I O.id STATIC/DYNAMIC/ACTIVE x y z alpha beta gamma SizeX SizeY SizeZ visible reference (2) |
|C|Coords|C id type x y|
|M|Message|M x1 y1 x2 y2 line1 (line2)|
|R|Region|R x1 y1 x2 y2 delatHP gravity velocity soundActivation activateAction|
|A|TextComponent|A id x y relativeX relativeY align message rrggbb alpha fontsize action (3) |
|B|ObjectComponent|A id x y relativeX relativeY alignX I(ACTIVE).id action (3) |
|D|ImageComponent|D id path x y z height width action (3) |

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

- (3) Action types:
|Action|Effect|
|----|------|
|0|(Nothing)|
|1|Open map selector|
|2|Open options|
|3|Open credits|
|31|Open GitHub|
|4|Open main menu|
|5|Start game|
|-1|Quit game|

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
|2|Player position|

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

|Argument name|Type|Description|
|-------------|----|-----------|
|ligthColor|uniform vec3 [28]|RGB color (0.0 - 1.0) |
|ligthInfo|uniform vec3 [28]|Specular 1:on/0:off, distFactor, lightIntensity|
