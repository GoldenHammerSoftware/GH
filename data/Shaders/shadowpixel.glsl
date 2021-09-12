varying vec4 v_pos;

void main()
{
    gl_FragColor = vec4(v_pos.z / v_pos.w, 0.0, 0.0, 1.0);
    //gl_FragColor = vec4(0.25,0.25,0.25,1.0);
}
