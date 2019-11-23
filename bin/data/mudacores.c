//Specify compiler to use GLSL version 1.2
//Enable working with textures of any dimensions
//Declare texture texture0, which is linked when you use fbo.bind(), or any other texture.bind().

//#version 430 compatibility
#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2DRect texture0;     //it can be named in any way, GLSL just links it

#define N (256)
uniform float specArray[N];

//uniform float time;             //Uniform value "time"

//blurring
void main(){
  // Paleta de cores: aurora boreal
  
  // Rosa:        242  53  141
  // Azul escuro    2  40  115
  // Azul           3  76  140
  // Verde musgo    3 140  101 
  // Verde          3 166   28 



  vec2 pos = gl_TexCoord[0].st;
  
  // Imagem da camera
  vec4 color0 = texture2DRect(texture0, pos);

  vec4 color;

  color = color0;  
  if(color.r > 0.1 && color.g > 0.1 && color.b > 0.1) {
      color = vec4(color.r/2,color.g,color.b,1-color.r);
  }
  if (color.r < 0.6 && color.r > 0.4 && color.b < 0. && color.b > 0.3)
  {

  }

  if (color.r > 0.6 && color.b > 0.3)
  {
    float b = color.b;
    color.r = b/4;
    color.b = color.r*3;
    color.a = color.g;
    color.g = 1;
  }
  if (color.b > 0.6) {
    color.g = color.b;
  }
  gl_FragColor = color;
}
