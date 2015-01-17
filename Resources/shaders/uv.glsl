precision highp float;
precision highp int;

varying vec4 v_fragmentColor;	
varying vec2 v_texCoord;
//CC_Time[1] is time

void main()			
{
	vec2 new_uv = v_texCoord;
	new_uv.x = fract(new_uv.x + CC_Time[1] * 0.03);
	new_uv.y = fract(new_uv.y + CC_Time[1] * 0.01);
	gl_FragColor = v_fragmentColor * texture2D(CC_Texture0, new_uv);	
}				
