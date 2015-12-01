# Desired schematic

The goal of this tutorial is to learn enough to create the schematic
seen below.

![Screenshot1](/tutorial/goal.png?raw=true)

# Introduction

The following commands may be helpful if you make a mistake during the
tutorial:

* To delete the component closest to the mouse (highlighted in bright
  red), press the key **shift-d**. This will remove the component and
  all wire segments connected to it.
* To delete the wire segment closest to the mouse (highlighted in
  bright red), press the key **d**. (When drawing a new wire you can
  also use **d** to cancel the wire drawing operation.)
* To undo an operation, use the **ctrl-z** command. Note that this is
  very experimental at this point and you might have to press ctrl-z a
  few times to actually undo the change you want to undo (since
  schematic_gui saves its state a bit too often)


# First steps, create and place initial components

At first, when starting schematic_gui you will start a blank project
with a help panel visible to the right.

## Create register

To create the first component (a register), press the key **7**. The
register will appear near the mouse pointer in movement mode. Move the
cursor until you are satisfied with its placement and press the left
mouse key. Your screen should now look like this (the red circle in the
screenshot is the port that is closest to the mouse cursor and will
change depending on where you move your mouse cursor).

![Screenshot0](/tutorial/screen0.png?raw=true)

Note that a component that has been selected for movement by pressing
the left mouse button will turn blue to indicate this.


## Create 2-to-1 mux

To create the next component, (a 2-to-1 mux), press the key **2**. Once
again, the component will appear near the mouse pointer. Move the
cursor until the mux is placed roughly as shown below and press the
left mouse button.

![Screenshot1](/tutorial/screen1.png?raw=true)

If you are not satisfied with the placement of a component you can
move **the closest** component by pressing the left mouse button once
(and release it), moving the cursor to the desired location and press
the left mouse button once again. (The closest component is always
highlighted in bright red, regardless of the distance to the mouse
cursor.)


# Draw a wire between the components

To connect the two components, move the mouse cursor until the right
port of the mux is selected as shown below. Note that you don't need
to touch the port, or even be near it, as long as **the port is the
closest port to the mouse cursor**.

![Screenshot2](/tutorial/screen2.png?raw=true)

Press the middle mouse button once to connect a wire to this port. A
wire will appear as shown below.

![Screenshot3](/tutorial/screen3.png?raw=true)

Move the cursor until the left port of the register is highlighted and
press the middle mouse button again. Your window should now look like
this:

![Screenshot4](/tutorial/screen4.png?raw=true)

(The red circle on the highlighted wire marks the spot on the wire that
is closest to the cursor.)

# Creating the remaining components

## Create and connect another multiplexer

Create another multiplexer by pressing the key **2**. Place and connect
it as shown below.

![Screenshot5](/tutorial/screen5.png?raw=true)


## Create and connect an adder

Create an adder by pressing the key **1**. Place and connect it as shown
below.

![Screenshot6](/tutorial/screen6.png?raw=true)


# Placing a text string on the schematic

We will now place the text *result* on the schematic Press the key
**t**. Write the text *result* in the dialog that appears and press
enter. Move it and place it as shown below.

![Screenshot7](/tutorial/screen7.png?raw=true)

(If you change your mind and want to write something else you can
highlight the text and press the **shift-e**. Write the new text in
the dialog that appears.)

## Connecting a wire to the text

Similar to the components created above a text string has four ports
available. Draw a wire from the register to the text string as shown
below. (Move the mouse cursor so that the leftmost port on the text
string is highlighted, press the middle mouse button. Move the cursor
so that the rightmost port on the register is highlighted and press
the middle mouse button again.)

![Screenshot8](/tutorial/screen8.png?raw=true)

## Placing text and connecting it at the same time

It is often convenient to be able to place a text and connect it at
the same time. Move the cursor so that the select input of the
rightmost mux is selected as shown below:

![Screenshot9](/tutorial/screen9.png?raw=true)

(Make sure that the cursor is located more or less directly above the
port as shown in the image.)

Press **shift-t**. Write *rst* in the dialog that appears and press
enter. You will now get a text string that has automatically been
connected to the port. 

![Screenshot10](/tutorial/screen10.png?raw=true)

Note that the tool automatically figures out on what edge of the text
string the wire should be connected based on where the mouse cursor is
located in relation to the port when **shift-t** is pressed.

(If the wrong edge is connected, just highlight the wire and press the
key **d** to remove the wire and redraw it using the middle mouse
button.)

## Placing the remaining text strings

Move the cursor to the left of the rightmost mux as shown below.

![Screenshot11](/tutorial/screen11.png?raw=true)

(Make sure that the cursor is located more or less directly to the
left of the port, otherwise the wire may be connected to the wrong
edge of the text string as discussed above.)

Press **shift-t** and write *0* (a zero) in the dialog that
appears. Your window should now look like this:

![Screenshot12](/tutorial/screen12.png?raw=true)

Continue like this until you have also placed the *D* and *load*
signals connected to the left mux and the *count* and *0* signals
connected to the adder. Your schematic should look like the following
when done:

![Screenshot13](/tutorial/screen13.png?raw=true)

# Connecting a wire to another wire

Now we need to connect the output from the register to the remaining
input on the adder. Place the mouse cursor as shown in the image
below:

![Screenshot14](/tutorial/screen14.png?raw=true)

The red circle on the highlighted wire marks the wire segment where we
will connect our new wire. Press the right mouse button to connect a
new wire to this wire segment. Your screen should now look like this:

![Screenshot15](/tutorial/screen15.png?raw=true)

Connect the wire to the remaining input of the adder as usual
(i.e. press the middle mouse button once the remaining input port of
the adder has been highlighted. Your window should now look like this:

![Screenshot16](/tutorial/screen16.png?raw=true)

(Note that you can also connect to an existing wire rather than an
input port by pressing the right mouse button when the appropriate
wire segment is highlighted.)


# Overriding the automatic routing by using checkpoints

Sometimes the decisions made by the automatic wire router used by
schematic_gui are not optimal. You will now manually influence the
routing of a wire by adding a checkpoint to it. Move the mouse cursor
so that it is located as shown below:

![Screenshot17](/tutorial/screen17.png?raw=true)

Place a checkpoint on the selected wire segment by pressing the key
**c**. Move the checkpoint until it is located as shown below and
press the left mouse button. The checkpoint is marked with a
cross. The circle around the checkpoint indicates that the checkpoint
is highlighted.

![Screenshot18](/tutorial/screen18.png?raw=true)

If you want to delete a checkpoint, highlight the checkpoint by moving
the cursor close to it and press the **shift-d** command. If you want
to move a checkpoint you can move it in the same way as other
components (i.e., highlight it and press the left mouse button).

## Place another checkpoint

If you wish to you can place another checkpoint on the wire you just
created so that the connection to the adder is more aesthestically
pleasing as shown below:

![Screenshot19](/tutorial/screen19.png?raw=true)


# Print preview, saving, and exporting

## Print preview

The design window contains a number of features that are desirable
when editing the text but are undesirable when exporting a schematic
for documentation purposes:

* Highlighted components and wires
* Highlighted ports
* Crossmarks indicating checkpoints
* Gray rectangles behind text strings

When exporting the schematic to SVG format (using the Export command
under the File menu) none of these features are included in the
exported schematic. To show the schematic without these features, you
can toggle between normal editing mode and print preview mode using
the key **p**. Your schematic should now look as below:

![GoalScreenshot](/tutorial/goal.png?raw=true)

## Saving
When saving (using the save command in the file menu), the design is
saved in a Verilog-like syntax where system task functions are used
for the actual schematic commands. (At the moment the file is not
guaranteed to be compilable using a Verilog compiler however, although
this is intended to be fixed in the future.) (It is also possible to
save the schematic using the **ctrl-s** command.)

## Exporting
Right now, schematic_gui only supports exporting to the SVG file
format using the Export command in the File menu and exporting the
current view of the schematic to the clipboard (may be useful if you
want to copy/paste into OpenOffice while writing documentation for
example).

# FAQ

## My favourite component is not included. What do I do?

* Simple answer: Add a small box (key **9**) or large box (key **0**)
  to the schematic and add a text inside the box to describe the
  component (e.g. *shifter*)
* Long answer: Look at the verilog files distributed with
  schematic_gui (e.g. *add.v*, *mux2.v*, etc). Create your own
  component using the same system task commands to define the look of
  your component. Unfortunately there is currently no support in
  schematic_gui for actually adding another component type (this will
  change in future versions), but you can of course modify design.cc
  to include another command to instantiate your favourite component
  (search for add_gate).

## I pressed the wrong key or schematic_gui crashed, how do I undo/recover my work?

At the moment, no undo functionality is available in
schematic_gui. Similarly, there are most likely bugs in schematic_gui
that can cause it to crash. To avoid situations where a lot of work is
lost schematic_gui will automatically save your work whenever you
modify the design significantly. These backup files are saved to
$HOME/.schematic_gui and you can access them from within schematic_gui
by using the *Recover previous work* command under the File
menu. (Make sure to use *Save As* the first time you save your
recovered file though to save the design somewhere else.)

Oh, and of course, if you happen to find a bug, please report it to
the author, either via email or via github.



# Recap

You should now know the following commands

* Left mouse button: Select/move a component or checkpoint
* Middle mouse button: Connect a wire to a port
* Right mouse button: Connect a wire to another wire
* Number keys: Instantiate component from component library
* **d**: Delete the highlighted wire/cancel wire drawing
* **D**: Delete the highlighted component/checkpoint
* **t**: Place text
* **shift-t**: Place text and connect it to closest checkpoint
* **shift-e**: Edit text
* **c**: Place checkpoint
* **p**: Toggle print preview mode
* **ctrl-z**: Undo
