# Desired schematic

The goal of this tutorial is to learn enough to create the schematic
seen below.

![Screenshot1](/tutorial/goal.png?raw=true)

# Introduction

At this point there is no *easy* way to undo an operation in
schematic_gui. However, the following commands may be helpful if you
make a mistake.

* To delete the component closest to the mouse (highlighted in bright
  red), press the key *shift-d*. This will remove the component and
  all wire segments connected to it.
* To delete the wire segment closest to the mouse (highlighted in
  bright red), press the key *d*. (When drawing a new wire you can
  also use *d* to cancel the wire drawing operation.)


# First steps, create initial components

At first, when starting schematic_gui you will start a blank project
with a help panel visible to the right.

## Create register

To create the first component (a register), press the key *7*. The
register will appear near the mouse pointer in movement mode. Move the
cursor until you are satisfied with its placement and press the left
mouse key. Your screen should now look like this (the red ring in the
screenshot is the port that is closest to the mouse cursor and will
change depending on where you move your mouse cursor).

![Screenshot0](/tutorial/screen0.png?raw=true)

Note that a component that has been selected for movement by pressing
the left mouse button will turn blue to indicate this.


## Create 2-to-1 mux

To create the next component, (a 2-to-1 mux), press the key *2*. Once
again, the component will appear near the mouse pointer. Move the
cursor until the mux is placed roughly as shown below and press the
left mouse button.

![Screenshot1](/tutorial/screen1.png?raw=true)

If you are not satisfied with the placement of a component you can
move *the closest* component by pressing the left mouse button once
(and release it), moving the cursor to the desired location and press
the left mouse button once again. (The closest component is always
highlighted in bright red, regardless of the distance to the mouse
cursor.)


## Draw a wire between the components

To connect the two components, move the mouse cursor until the right
port of the mux is selected as shown below. Note that you don't need
to touch the port, or even be near it, as long as *the port is the
closest port to the mouse cursor*.

![Screenshot2](/tutorial/screen2.png?raw=true)

Press the middle mouse button once to connect a wire to this port. A
wire will appear as shown below.

![Screenshot3](/tutorial/screen3.png?raw=true)

Move the cursor until the left port of the register is highlighted and
press the middle mouse button again. Your screen should now look like
this:

![Screenshot4](/tutorial/screen4.png?raw=true)

(The red ring on the highlighted wire marks the spot on the wire that
is closest to the cursor.)


## Create and connect another multiplexer

Create another multiplexer by pressing the key *2*. Place and connect
it as shown below.

![Screenshot5](/tutorial/screen5.png?raw=true)


## Create and connect an adder

Create an adder by pressing the key *1*. Place and connect it as shown
below.

![Screenshot6](/tutorial/screen6.png?raw=true)


## Placing text on the schematic

We will now place the text _result__ on the schematic Press the key
*t*. Write the text _result_ in the dialog that appears. Move it and
place it as shown below.

![Screenshot7](/tutorial/screen7.png?raw=true)

## Connecting a wire to the text

Similar to the components created above a text string has four ports
available. Draw a wire from the register to the text string as shown
below. (Move the mouse cursor so that the leftmost port on the text
string is highlighted, press the middle mouse button. Move the cursor
so that the rightmost port on the register is highlighted and press
the middle mouse button again.)

![Screenshot8](/tutorial/screen8.png?raw=true)

