# Assets - File formats

- maps
  + Map (.map)
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
  
### Map

- Use # for comments
- Max line length: 255 char
- Strict orders: X->Y->T, O->I
- Whitespace start chars not allowed

|Sign|Type|Format|
|:---:|----|-------|
|$|Meta|$ KEY VALUE...|
|X|Texture|X id filename|
|Y|TextureBlock|Y id base top right bottom left|
|T|Tile|T x y Y.id TileType|
|S|Static Light|S x y strength color|
|L|Ligthing|L x y strength1 color1 strength2 color2 time|
|O|Object Active|O id STATIC/DYNAMIC/ACTIVE filename|
|I|Object Instance|I O.id STATIC/DYNAMIC/ACTIVE x y z alpha beta gamma SizeX SizeY SizeZ visible reference(1) |
|C|Coords|C id type x y|
|M|Message|M x1 y1 x2 y2 line1 (line2)|
|R|Region|R x1 y1 x2 y2 delatHP gravity velocity soundActivation activateAction|
|A|TextComponent|A id x y relativeX relativeY message rrggbb alpha|
|B|ObjectComponent|A id x y relativeX relativeY alignX I(ACTIVE).id |
|D|ImageComponent|D id path x y z height width|

- (1) reference required and available only for dynamic objects
- Meta:
  + `NAME` name
  + `AUTHOR` name
  + `DESCRIPTION` String (Use _ for space)
  + `AMBIENT` percent (0.0 - 1.0)
  + `SPAWN` x y z
  + `STATE` GameState (0: MENU/ 1: INGAME/ 2: PAUSE)

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

```
	character filename
	%c %s
```

> NOTE: No comments enabled

### Font v2 (.font32)

*Unimplemented*

```
	characterId filename
	%u %s
```