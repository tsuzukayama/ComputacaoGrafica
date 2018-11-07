#version 410

 //(in)put variables processed in parallel
layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 vColors;

 //(out)put variable interpolated at fragment shader raster
 out vec4 v2fcolor;

 uniform mat4 transform;

 void main()
 {
     //gl_Position->out builtin variable
     gl_Position = transform * vPosition;
     v2fcolor = vColors;
 }
