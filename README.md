# fractal-generator
 A Julia Sets Generator coded in C.
 
 A Julia Set is the fractal defined by the sequence z²+c where c is a complex number.
 
 Compile it with gcc and parameters -lpng for lib png.h and -lm for math.h (`gcc fgen.c -o fgen -lpng -lm`)

 # Usage:
 
  `./fgen <side of square image> <number of iterations> <power p of z in the sequence z^p+c defining the Julia Set (usually 2)> <Re(c)> <Im(c)>`

# Exemple:

![f_upload](https://user-images.githubusercontent.com/82156580/199241403-dadf4aa7-ba93-48f2-8199-4965bf924500.png)

 This one is given by 
  `./fgen 400 400 1 .355534 -.337292`
  
 (and yes this is where I took my profile picture ;)
