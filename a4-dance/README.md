Jeremy Wood CSCI 4611 Assignment 4 - Dance

The purpose of this assignment was to create dancing characters through transformations composed in a heirarchy. The dancing element was accomplished using motion capture data from the Carnegie Mellon Motion Capture Database. 

A current transformation matrix was created by using the parent transform matrix, transforming it into Rotational Axes space, rotating the joint, and transforming back into Bone Space. The bone is drawn as a line segment from (0, 0, 0) in bone space to (0, 0, 0) + the bone direction and length (which is a vector). 

A series of if statements checks for which bone is currently being drawn, and a part of the body is drawn depending on the bone. The body parts are drawn around the bones using other shapes such as a spheres.

Once each if statement has been checked, the child root transform matrix is created by multiplying the current transform matrix by a translation matrix of the bone direction and length. The process is then repeated over from the beginning with the child root transform being used as the next parent transform matrix. This happens for each bone.

To create different dances, clips were loaded from .amc files, with parts of the beginning and end cut off so that one of the characters would just do the interesting part of the clip, and the clip was looped if need be. The parts to be cut off and looped were determined experimentally. These different dance clips are run when the associated button is pressed, which calls the OverlayClip method.
