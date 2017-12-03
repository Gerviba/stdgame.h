# Help for evaluation [HUN]

### Kb. 500 soros C program...
Ellenőrzés:	
```
$ make countlines
```

### ..., amely a C nyelv lehetőségeit kihasználja: strukturált felépítés, több modulra bontás, 

 - src/ *
 - Kedvenc structjaim: 
   + Component
   + GameInstance
   + Map 
   + ActiveObject
 - Structok száma:
 - Enumok száma: 
 - Forrásfájlok száma: 22 (11 .c és 11 .h)

### ... dinamikus memóriakezelés,

 - Általános célú láncolt lista: src/ linkedlist.c (Futás alatt 100 fölötti fej példánnyal rendelkezik, pályától függően)
 - Több mint 95 kölönböző helyen van malloc

### ... fájlkezelés.

 - src/ 
   + shader.c (text)
   + map.c (text)
   + object.c (text)
   + font.c (text)
   + menu.c (binary)
   
### A program mellé el kell készüljön a programozói és a felhasználói dokumentáció.

Generlás:	
```
$ make gendocs
```

 - Html dokumentáció: `devdocs/html/index.php`
 - Manual dokuumentáció: `devdocs/man/`
 
### A feladatválasztás jóvá lett hagyva

### Bemutatás megtörtént: 
	Dec 1-én 10:15 óra után