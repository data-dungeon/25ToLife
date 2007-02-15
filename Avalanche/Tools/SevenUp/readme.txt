File: Sevenup.exe
Date: December 18, 2003
Auth: Daniel Silver

Sevenup was designed to assist designers and programmers working
together to create front end screens and heads-up displays.

Here are some of the fundamental issues this tool address:

1. Give the artist the ability to design the screen using a WYSIWYG interface.

	Since the tool is built on top of existing engine capability, it exposes
	functionality native to the operating environment and gives the designer
	total freedom.  The designer does not need to use a 3rd party tool with a
	limited feature set, nor does he have to worry about differences in
	implemenation.  What you see is EXACTLY what you get.
	
2. Reduced memory usage in-game.

	a. The size of the executable is reduced because there is no need for
	   hard-coded initialization or layout.
	b. The memory footprint is reduced because screens can be selectively
	   loaded and unloaded.
	c. Pre-loaded screens are compressed.
	d. Textures are shared.
	
3. Vastly improved production time.

	a. Screens can be designed and modified far more quickly than before.
	b. Work load is distributed more evenly among the programmer and designer.

Starting up the application.

	Sevenup has 5 window panes;

		The file browser looks much like a Windows file explorer.  It is used
		to select the folder to view in the texture browser.  When you click
		on a folder, the contents of that folder are examined and suitable files
		are displayed in the texture browser.
		
		The texture browser, located at the bottom of the screen, displays a
		thumbnail of all suitable textures located in the current directory.
		A texture can be selected with the mouse.  A scroll bar appears when
		there are too many textures to display on screen at once.
		
		When a texture is selected, it may be placed on screen by clicking and
		holding the left mouse button on the layout pane.  While holding the
		mouse button, drag the cursor across the screen to resize the texture.
		When finished, release the mouse button.  A new sprite is created and
		placed on the screen and also placed within the project tree.
		
		Once a sprite is created, it can be manipulated using the mouse.  Place
		the mouse on top of the sprite and it will be highlighted.  Click the
		left mouse button and drag to reposition the sprite and release the
		mouse button when finished.  If you release the mouse button while the
		mouse cursor is hovering on another sprite, both sprites will be
		grouped together; the sprite being dragged becomes the child.
		
		You can duplicate a sprite by clicking on it with the right mouse button
		and dragging it across the screen.  The duplicated sprite will be
		independant of the original sprite, although they too can be grouped in
		any configuration.
		
		If you right-click on an object without dragging it, a context menu will
		appear.  The context menu displays actions specific for that type of
		sprite.  Currently, the actions available are:
		
			a.	Lock.  This locks a sprite into place.  Once a sprite is locked,
				it cannot be edited.  This allows you to work on the project
				without accidentally disturbing the state of that object.
				
			b.	Delete.  This deletes the sprite from the project.  A warning
				will be displayed if the action will delete more than 1 sprite.
				
			c.	Properties.  This will activate the properties window for that
				sprite.  The properties window is not available at this time;
				a message box is displayed as a place holder.
				
			d.  Text (text object only).  This allows you to change the text
				with a Text object.  This feature is not fully implemented,
				therefor the text cannot be changed (there is a bug due to MFC
				message routing preventing me from typing in the window.)
				
			More actions will be added as the project is completed.
			
		The project view, located on the right side of the screen, displays the
		current project tree.  The project tree is itself the project; it
		contains all of the information about every object and layout of the
		screens.
		
		The project bar, located immedately to the left of the project view,
		contains a tool bar with buttons (these buttons are place holders
		at the moment).  This is where you select the behavior of the mouse,
		i.e. dragging, sizing, rotations etc...

	Sevenup is still in an early stage of developmenet.
	
		MFC and Win32 extention libraries are more than 90% complete.
		Direct3D hardware acceleration is more than 90% complete.
		Application framework is about 50% complete.
		Application features is less than 10% complete.
		