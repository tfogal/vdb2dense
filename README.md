Converts a VDB (openvdb.org) file into a simple dense grid.  I needed
this or something similar and couldn't find it in the cookbook.

This is actually hardcoded to work with explosion.vdb right now.  To
fix it you'll need to figure out the offsets from the VDB file and
properly account for them in the write_raw function.

You can then load the resulting file in (e.g.) ImageVis3D
(www.ImageVis3D.org/) and render the data in real time.
