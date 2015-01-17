
varying vec4 v_fragmentColor;	
varying vec2 v_texCoord;	
//uniform sampler2D CC_Texture0;	
//CC_Time[1] is time

void main()			
{
	vec4 c = v_fragmentColor * texture2D(CC_Texture0, v_texCoord);
	float gs = c.r * c.g * c.b;
	gl_FragColor = vec4(gs, gs, gs, c.a);
	//float gray = dot(v_orColor.rgb, vec3(0.299, 0.587, 0.114));
	//gl_FragColor = vec4(gray, gray, gray, v_orColor.a);
}				
