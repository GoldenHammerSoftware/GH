attribute mediump vec4 position;
attribute lowp vec2 uv1;

uniform highp float Time;

mat4 orthoProj = mat4(	2, 0, 0, 0,
						0, -2, 0, 0,
						0, 0, -1, 0,
						-1, 1, 0, 1 );

varying lowp vec2 v_uv1;


//Two variables to tweak here. They are synergistic, meaning that
// changing one will affect the impact of the other:
// 
// Change the value multiplied by Time at the beginning.
//  a bigger number will make the hourglass spin faster overall.
//
// Change the "eps" value. A bigger number will make the hourglass 
//  spend more time at its 180 degree stops.

void main()
{
    highp float pi = 3.14159;

    highp float angle = mod(Time*2.0, 2.0*pi);
    
    highp float eps = 0.5;
    highp float a = step(pi - eps, angle);  //if angle is above our lower threshold around 180
    highp float b = step(pi + eps, angle); //if angle is above our upper threshold around 180
    highp float c = step(0.5, a - a*b); //if angle is within our threshold around 180
    
  highp float x = step(eps, angle); //if angle is above our upper threshold around 360
  highp float y = step(pi*2.0 - eps, angle); //if angle is above our lower threshold around 360
  highp float z = 1.0 - (y - x); // if angle is within our threshold around 360  
    
    
  //speed up the angle in the case where we are not within either threshold
  // so that we appear to smoothly rotate into and out of the stopping points
  angle = (1.0-a)*(eps + (angle-eps)*(pi-eps)/(pi-2.0*eps)) + a*(pi+eps + ((angle-(pi+eps))*(pi-eps)/(pi-2.0*eps)));
    
  //either pick 180 or 360 degrees if we are within the 
  // appropriate threshold or use our sped-up angle
    angle = c*pi + z*2.0*pi + (1.0-c)*(1.0-z)*angle;

// rotating pos instead of uv causes stretching at different aspect ratios.
//  mat4 rotProj = mat4( cos(angle), -sin(angle), 0, 0,
//                       sin(angle), cos(angle), 0, 0,
//                       0, 0, 1, 0,
//                       0, 0, 0, 1);

//    gl_Position = rotProj * orthoProj * position;
//    v_uv1 = uv1;

  mat2 rotProj = mat2( cos(angle), -sin(angle),
                       sin(angle), cos(angle));

    gl_Position = orthoProj * position;
    v_uv1 = (rotProj * (uv1-0.5))+0.5;

}
