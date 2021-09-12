uniform float Opacity;

void main()
{
    vec4 color = vec4(0.5,0.5,0.5,Opacity);
    gl_FragColor = color; 
}
