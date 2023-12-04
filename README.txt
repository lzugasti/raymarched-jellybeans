TIP: My solution prints the x position of the current pixel being processed 
	 To keep track of progress when rendering slower scenes (shouldn't be needed for the ones given in class)

All additional features (and a significant portion of the required features) are available in:
	- 260919951-leopoldo-zugasti-competition.png Image 
	- 260919951-leopoldo-zugasti-competition.json scene.
However this scene can take a lot of time to render so I recommend running the smaller scenes first, or possible lowering the 
Resolution to 640x480 for a somewhat reasonable render time.

If you want to quickly look at the output images for the required features you can go into the "/out/required" folder 

Additional features are:
1. MetaBalls
	To test this you can try the scene called Metaballs.json, the output is in Metaballs.png
		- You can try changing the position of the metaballs from each other to see how they render depending on how close/far they are from eachother :D
		- If you don't want to do this manually you can run MetaballsPlane.json (output in MetaballsPlane.png) which is a simplified version of my competition submission

2. Depth of Field Motion Blur
	IMPORTANT: Depth of Field Blur comes with an additional "Jitter" Option in the json, with and without it can be found in:
	- MetaballsDOF.json, the output is in MetaballsDOF.png
	- MetaballsDOFJitter.json, the output is in MetaballsDOFJitter.png

	TIP: This is by far the most time consuming one, the other features execute much faster, 
	     I recommend lower resolutions if you want to try it quickly

3. Reflections
	Most Basic Test: You can try the MetaballsReflection.json, the output is in MetaballsReflection.json
	(obviously this works regardless of whether or not we use metaballs)
	Cooler Test: You can try MetaballsPlane.json which is the simplifed version of my comptetion submission (runs quite quickly)