#version 430 compatibility
// #version 430
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2DRect texture0;     //it can be named in any way, GLSL just links it
uniform float iTime;

#define N (256)
uniform float specArray[N];

//uniform float time;             //Uniform value "time"

#define WIDTH (1366)
#define HEIGHT (768)

#define TAU 6.28318530718

#define TILING_FACTOR 1.0
#define MAX_ITER 8
#define M_PI 3.14159265358979323846
#define PI 3.14159265358979323846

float waterHighlight(vec2 p, float time, float foaminess)
{
	vec2 i = vec2(p);
	float c = 0.0;
	float foaminess_factor = mix(1.0, 6.0, foaminess);
	float inten = .005 * foaminess_factor;

	for (int n = 0; n < MAX_ITER; n++) 
	{
		float t = time * (1.0 - (3.5 / float(n+1)));
		i = p + vec2(cos(t - i.x) + sin(t + i.y), sin(t - i.y) + cos(t + i.x));
		c += 1.0/length(vec2(p.x / (sin(i.x+t)),p.y / (cos(i.y+t))));
	}
	c = 0.2 + c / (inten * float(MAX_ITER));
	c = 1.17-pow(c, 1.4);
	c = pow(abs(c), 8.0);
	return c / sqrt(foaminess_factor);
}

float rayStrength(vec2 raySource, vec2 rayRefDirection, vec2 coord, float seedA, float seedB, float speed)
{
	vec2 sourceToCoord = coord - raySource;
	float cosAngle = dot(normalize(sourceToCoord), rayRefDirection);

	return clamp(
			(0.45 + 0.15 * sin(cosAngle * seedA + iTime * speed)) +
			(0.3 + 0.2 * cos(-cosAngle * seedB + iTime * speed)),
			0.0, 1.0) *
		clamp((WIDTH - length(sourceToCoord)) / WIDTH, 0.5, 1.0);
}

float rand(vec2 c){
	return fract(sin(dot(c.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float noise(vec2 p, float freq ){
	float unit = 1366/freq;
	vec2 ij = floor(p/unit);
	vec2 xy = mod(p,unit)/unit;
	//xy = 3.*xy*xy-2.*xy*xy*xy;
	xy = .5*(1.-cos(PI*xy));
	float a = rand((ij+vec2(0.,0.)));
	float b = rand((ij+vec2(1.,0.)));
	float c = rand((ij+vec2(0.,1.)));
	float d = rand((ij+vec2(1.,1.)));
	float x1 = mix(a, b, xy.x);
	float x2 = mix(c, d, xy.x);
	return mix(x1, x2, xy.y);
}

float pNoise(vec2 p, int res){
	float persistance = .5;
	float n = 0.;
	float normK = 0.;
	float f = 4.;
	float amp = 1.;
	int iCount = 0;
	for (int i = 0; i<50; i++){
		n+=amp*noise(p, f);
		f*=2.;
		normK+=amp;
		amp*=persistance;
		if (iCount == res) break;
		iCount++;
	}
	float nf = n/normK;
	return nf*nf*nf*nf;
}


float rand (vec2 co, float l) {return rand(vec2(rand(co), l));}
float rand (vec2 co, float l, float t) {return rand(vec2(rand(co, l), t));}

float perlin(vec2 p, float dim, float time) {
	vec2 pos = floor(p * dim);
	vec2 posx = pos + vec2(1.0, 0.0);
	vec2 posy = pos + vec2(0.0, 1.0);
	vec2 posxy = pos + vec2(1.0);
	
	float c = rand(pos, dim, time);
	float cx = rand(posx, dim, time);
	float cy = rand(posy, dim, time);
	float cxy = rand(posxy, dim, time);
	
	vec2 d = fract(p * dim);
	d = -0.5 * cos(d * M_PI) + 0.5;
	
	float ccx = mix(c, cx, d.x);
	float cycxy = mix(cy, cxy, d.x);
	float center = mix(ccx, cycxy, d.y);
	
	return center * 2.0 - 1.0;
}

// p must be normalized!
float perlin(vec2 p, float dim) {
	
	/*vec2 pos = floor(p * dim);
	vec2 posx = pos + vec2(1.0, 0.0);
	vec2 posy = pos + vec2(0.0, 1.0);
	vec2 posxy = pos + vec2(1.0);
	
	// For exclusively black/white noise
	/*float c = step(rand(pos, dim), 0.5);
	float cx = step(rand(posx, dim), 0.5);
	float cy = step(rand(posy, dim), 0.5);
	float cxy = step(rand(posxy, dim), 0.5);*/
	
	/*float c = rand(pos, dim);
	float cx = rand(posx, dim);
	float cy = rand(posy, dim);
	float cxy = rand(posxy, dim);
	
	vec2 d = fract(p * dim);
	d = -0.5 * cos(d * M_PI) + 0.5;
	
	float ccx = mix(c, cx, d.x);
	float cycxy = mix(cy, cxy, d.x);
	float center = mix(ccx, cycxy, d.y);
	
	return center * 2.0 - 1.0;*/
	return perlin(p, dim, 0.0);
}

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
/*
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
  */

	// Set the parameters of the sun rays
	vec2 rayPos1 = vec2(WIDTH * 0.3, HEIGHT * -0.2);
	vec2 rayRefDir1 = normalize(vec2(1.0, -0.116));
	float raySeedA1 = 36.2214;
	float raySeedB1 = 21.11349;
	float raySpeed1 = 1.5;

	vec2 rayPos2 = vec2(WIDTH * sin(iTime/100), HEIGHT * -0.6);
	vec2 rayRefDir2 = normalize(vec2(1.0, 0.241));
	const float raySeedA2 = 22.39910;
	const float raySeedB2 = 18.0234;
	const float raySpeed2 = 1.1;

	vec2 coord = pos;
	// Calculate the colour of the sun rays on the current fragment
	vec4 rays1 =
		vec4(1.0, 1.0, 1.0, 1.0) *
		rayStrength(rayPos1, rayRefDir1, coord, raySeedA1, raySeedB1, raySpeed1);

	vec4 rays2 =
		vec4(1.0, 1.0, 1.0, 1.0) *
		rayStrength(rayPos2, rayRefDir2, coord, raySeedA2, raySeedB2, raySpeed2);

	gl_FragColor = rays1 * 0.5 + rays2 * 0.4;

	// Attenuate brightness towards the bottom, simulating light-loss due to depth.
	// Give the whole thing a blue-green tinge as well.
	float brightness = 1.0 - (coord.y / HEIGHT);
	gl_FragColor.r *= 0.1 + (brightness * 0.8);
	gl_FragColor.g *= 0.3 + (brightness * 0.6);
	gl_FragColor.b *= 0.7 + (brightness * 0.5);


	gl_FragColor = ( gl_FragColor + color ) / 1.2;

  if ( pos.x < 100 ) {
    float alpha =  ((pos.x )/110);
    if( gl_FragColor.a > alpha ) {
      gl_FragColor.a = alpha;
    }
  }
 float amplitude = 1.;
float frequency = 1.;
float x = pos.x/166;
float y = sin(x * frequency);
float t = 0.01*(-iTime*130.0);
y += sin(x*frequency*2.1 + t)*4.5;
y += sin(x*frequency*1.72 + t*1.121)*4.0;
y += sin(x*frequency*2.221 + t*0.437)*5.0;
y += sin(x*frequency*3.1122+ t*4.269)*2.5;
y *= amplitude*0.06;
  float treshold =(y+11)*45;
  if ( pos.y > treshold ) {
    float alpha = 1 - ( pos.y - treshold )/190;
    if( gl_FragColor.a > alpha ) {
      gl_FragColor.a = alpha;
    }
  }

}
