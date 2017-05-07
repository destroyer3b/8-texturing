# Lab 8: Bump mapping

This branch shows a rotating cube with each side a different color.

The branch "texture" shows the cube rotating with a bunny image textured onto each side. Access by `git checkout texture`. Please note that absolute paths are no longer required for the texture.

The branch "bump" gives you a starting point for the hw that will still give you the same rotating bunny. In github you can use the "compare across branches" option to show the changes that have been made to main.cpp:

* A function for calculating tangents and bitangents
* Shader variables for tangents and bitangents, along with vertex attribute commands in main
* Lighting constants defined so you can get the same result as the target clip
* Todos indicate where you need to make changes

Create a new homework branch `git checkout -b bump_hw`. 

* Load the "brick_normals.png" into a new texture the same way the bunny image was loaded.
* See the last section of https://learnopengl.com/#!Getting-started/Textures for how to load and activate two separate textures. (Unlike that tutorial, we will use one texture for the diffuse color and the other for normals, but you can start by mixing the two of them as in the tutorial to make sure they have both been loading correcly.)
* See [this tutorial](http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/) for how to use the tangents, bitangents, and polygon normal (cross(bitangent, tangent)). The full code for the tutorial is over [here](https://github.com/opengl-tutorials/ogl/tree/master/tutorial13_normal_mapping) and will help you understand how it all fits together, but the lighting model is different from ours and should not be copied directly.
* Modify the provided code in the places indicated to produce the example clip [bump-hw.mov](http://www.sci.brooklyn.cuny.edu/~levitan/cisc3620/materials/brickbunny.mov)

## Due: Thurs. 5/18
