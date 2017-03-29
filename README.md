# agrab

_Prints rectangle coordinates after drawing it on screen_

Small tool meant to make using [byzanz](https://github.com/GNOME/byzanz)
a little easier by allowing to select the area of recording, instead of trying
to guess the position or checking it in image editing software on a screenshot.

## Installation

```
make && make install
```

Uses the cairo library.

## How to use

Run the command `agrab`. The mouse cursor should change to a crosshair.
Draw a rectangle on screen with left mouse button. After releasing the button
the coordinates will be printed on the standard output.

The format is:

```
-x LEFT -y TOP -w WIDTH -h HEIGHT
```

with values in pixels. This makes it convenient to use agrab together with
byzanz-record like this:

```
byzanz-record -d TIME $(agrab) out.gif
```

Possible future improvements may include an option to output the coordinates
as variable assignment, more useful for shell scripting.
