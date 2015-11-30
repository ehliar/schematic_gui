# Desired schematic

The goal of this tutorial is to learn enough to create the schematic
seen below.

![Screenshot1](/tutorial/goal.png?raw=true)

# Introduction

At this point there is no *easy* way to undo an operation in
schematic_gui. However, the following commands may be helpful if you
make a mistake.

* To delete the component closest to the mouse (highlighted in bright red),
  press the key *D*
* To delete the wire segment closest to the mouse (highlighted in
  bright red), press the key *d*.


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



## Create 2-to-1 mux

To create the next component, (a 2-to-1 mux), press the key *2*. Once
again, the component will appear near the mouse pointer. Move the
cursor until the mux is placed roughly as shown below and press the
left mouse button.

![Screenshot1](/tutorial/screen1.png?raw=true)

If you are not satisfied with the placement of a component you can
move *the closest* component by pressing the left mouse button once
(and release it), moving the component to the desired location and
press the left mouse button once again. (The closest component is
always highlighted in schematic_gui as seen in the screenshot above
where the mux is highlighted since it is closest to the cursor.)



## Connect the components.

To connect the two components, move the mouse cursor until the right
port of the mux is selected as shown below. Note that you don't need
to touch the port, or even be near it, as long as *the port is the
closest port to the mouse cursor*.

![Screenshot2](/tutorial/screen2.png?raw=true)

Press the middle mouse button once to connect a wire to this port. A
wire will appear as shown below.

![Screenshot3](/tutorial/screen3.png?raw=true)

