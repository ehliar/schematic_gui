# Intro 

schematic_gui is a lightweight tool to draw circuit diagrams of
digital designs. It was written because I was not satisfied with the
tools I typically use to draw schematic diagrams for documentation
purposes.



# Selected Features

* Optimized for quick schematic entry. Wires will _always_ snap to the
  closest connector or wire (depending on if the middle or right mouse
  button is used), regardless of distance.
* Automatic routing of wires (with support for manual override)
* Automatically saves the design to $HOME/.schematic_gui/ in
  timestamped files. If the program crashes (which it probably will at
  some point), chances are that you can find a previous version of the
  design in this directory.
* It works for me<TM> (if you ignore the occasional crash and memory leak)
* Ugly source code (which is being cleaned up though)
* Save file format is (more or less) Verilog compatible. (The eventual
  goal is that a file saved with schematic_gui should be
  synthesizable, but that is not quite possible yet.)
* Exports to SVG file format
* Exports current view to clipboard


See the <a href="tutorial/tutorial.md">tutorial</a> for more
information.


# Preview

This is a screenshot when opening the simple_alu.v file in the
examples directory.

![Screenshot](/screenshot.png?raw=true)

## Features seen in screenshot:
* Small circle: Closest connector port, press middle button to connect
  a wire here.
* Larger circle: Closest point on a wire, press right mouse button to
  connect a wire here.
* Crossmark: Checkpoint, routing has been manually overridden here in
  order to look better.
* In addition, the component closest to the mouse cursor (the adder)
  is also highlighted. Whenever the left mouse button is pressed the
  highlighted component will be moved (regardless of how close the
  mouse cursor is).



# License

* Copyright (C) 2015 Andreas Ehliar <andreas.ehliar@liu.se>
* GPLv3 or later.
* Exception: The Verilog modules distributed with the program (all
  files ending in .v) are licensed as CC0 (public domain)
* Clarification: Files saved or exported from schematic_gui are not
  covered by any schematic_gui license. 



# Requirements

* libavoid (from the adaptagrams framework)
* gtk3



# Usage

Note that all of these operations are subject to change based on
feedback from users.

## List of mouse operations
* Left mouse button: To move the highlighted component or checkpoint,
  press the left mouse button once to enable move mode, press the left
  mouse button once again when you have moved it.
* Middle mouse button: Connect a wire to a highlighted port
* Right mouse button: To connect a wire you are drawing to the
  highlighted location on another wire, press right mouse
  button. Alternatively, start drawing a wire at the highlighted
  location. (Avoid cyclic graphs, they are not fully supported at this
  time.)
* Scroll wheel: Zoom in/out


## List of keyboard operations

* Arrow keys: Scroll left,right,up,down
* d: Deletes the closest wire (alternatively, abort wire drawing if
  you are currently in wire drawing mode (i.e. you have pressed the
  left or right mouse button once)
* D: Deletes the highlighted component or checkpoint (including any
  wires connected to it)
* A: Select all objects
* c: Adds a checkpoint to the highlighted line. (You can use this to
  manually control routing to some extent.)
* r: Rotate the highlighted component
* i: Toggle inversion on the highlighted port
* e: Edits the name of the highlighted wire
* E: Edits the name of the highlighted component or text field
* t: Adds a new text field
* T: Adds a new text field and connects it to the highlighted port
* s: Adds a component to the current selection
* S: Unselects all components
* p: Print preview (disables all highlighted components, checkpoints, etc)
* Ctrl-s: Saves the current design (hint, save often and using different
  filenames, this program is not really stable yet)

## List of supported component

These components are added to the schematic by pressing the corresponding key.
* 1: Create an adder
* 2: Create 2 input multiplexer
* 3: Create 3 input multiplexer
* 4: Create 4 input multiplexer
* 5: Create 5 input multiplexer
* 6: Create 6 input multiplexer
* 7: Create a register
* 8: Create a multiplier
* 9: Create a small black box component
* 0: Create a large black box component




# Todo

* Fix all memory leaks
* Make sure the design files that are saved are actually correct Verilog code
  * Right now they are almost correct, but some work is required to actually be able to
    compile these files. Most notably there are no input and outputs.
  * User should be able to set the name of the module
  * Make sure the user can set the width of wires
  * (Make sure the user can propagate the width of the wires in a clever fashion.
  * Add support for more Verilog primitives to allow schematic_gui to be used to
    document existing Verilog files.
* Create VPI code to render the design even if schematic_gui is not used.
* Integration with verilog-mode in emacs (perhaps using Joakim Verona's xwidgets patch for emacs)
* Better documentation
* Code cleanups (Right now the code doesn't even pretend to follow any
  decent design patterns for GUI:s such as MVC.)
* Junctions in libavoid are acting up. A workaround is included in
  clean_junction_positions().  This should be investigated more and if
  possible the workaround should be removed.
* Export to other formats than SVG
* Export current view to clipboard
* Allow component symbols to be specified using SVG (perhaps using
  librsvg to read these symbols?)
* Add a better way to add components
* Add a way to cycle through similar components without deleting all
  connected wires
* At the moment only acyclic graphs are allowed for wires in the
  design. There are, however, no checks that prohibit their creation
  in the GUI. (But they can't be saved correctly in some
  situations. This should probably be fixed (either by disallowing
  their creation or by allowing them to be saved as well).
* There is no way to undo an operation at this point unfortunately.
* Add a way to include optional wires in symbols. (E.g., if something
  is connected to this port the line will be included, otherwise it
  will be omitted.)
* Try to move some libavoid-related functionality into libavoid itself
  (e.g. find closest connector, etc)
* Move to an autoconf based configuration?
* Script to automatically download/compile libavoid if not detected by
  configure?
* Add status bar to make it easier for users to see what is going on.
* Add a tutorial of some sort.
