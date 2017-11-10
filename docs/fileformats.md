# Assets

- maps
  - Map (.map)
- objects
  - StaticObject: Pre-calculated move matrix (.sobj)
  - DynamicObject: Dynamic move matrix (.dobj)
  - ActiveObject: Multiple DynamicObjects (.aobj)
  
### Map

|Sign|Type|Format|
|---|---|---|
|$|Meta|$ KEY VALUE...|
|X|Texture|X id filename|
|Y|Texture Block|# Y id base top right bottom left|
|T|Tile|T x y Y.id TileType|
|S|Static Light|S x y strength color|
|L|Ligthing|L x y strength1 color1 strength2 color2 time|
|O|Object Active|O id STATIC/DYNAMIC/ACTIVE filename|
|I|Object Instance|I O.id STATIC/DYNAMIC/ACTIVE x y z alpha beta gamma SizeX SizeY SizeZ visible (reference) |
|C|Coords|C id type x y|
|M|Message|M x1 y1 x2 y2 LANG_KEY|
|R|Region|R x1 y1 x2 y2 delatHP gravity velocity|
|A|TextComponent|A x y message |