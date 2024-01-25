//this var comes from the vertex shader
//they are baricentric interpolated by pixel according to the distance to every vertex
varying vec3 v_wPos;
varying vec3 v_wNormal;
varying vec2 v_uvs;

//here create uniforms for all the data we need here
uniform vec3 light;
uniform vec3 camera;
uniform float alpha;
uniform vec3 Ia;
uniform vec3 Id;
uniform vec3 Is;
uniform sampler2D u_texture;

//variables auxiliars
vec3 N;
vec3 L;
vec3 V;
vec3 R;
vec3 ip;
vec4 color;
vec3 aux;


void main()
{

    N = normalize(v_wNormal);
    L = normalize(light - v_wPos);
    V = normalize(camera - v_wPos);
    R = normalize(reflect(-L,N));
    color = texture2D(u_texture, v_uvs);

    aux = color.xyz;

    vec3 spec_factor = color.xyz * color.w;

    ip = (aux * Ia) + (aux * clamp(dot(L,N),0.0,1.0)*Id) + (spec_factor * pow(clamp(dot(R,V),0.0,1.0), alpha)*Is);


    //project the vertex by the model view projection
    gl_FragColor = vec4(ip,1.0)*1.0; //output of the vertex shader
}