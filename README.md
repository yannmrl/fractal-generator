# fractal-generator
 A Julia Sets Generator coded in C.
 
 A Julia Set is the fractal defined by the sequence z²+c where c is a complex number.
 
 Compile it with gcc and parameters -lpng for lib png.h and -lm for math.h (`gcc fgen.c -o fgen -lpng -lm`)

 # Usage:
 
  `./fgen <side of square image> <number of iterations> <power p of z in the sequence z^p+c defining the Julia Set (usually 2)> <Re(c)> <Im(c)>`

# Exemple:

![b](https://user-images.githubusercontent.com/82156580/199240102-44bed839-d8dd-4dab-815a-52be75a34afb.png
