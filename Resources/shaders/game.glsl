
varying vec4 v_fragmentColor;	
varying vec2 v_texCoord;	
uniform sampler2D CC_Texture0;	
//CC_Time[1] is time

void main()			
{
	//vec4 v_orColor = v_fragmentColor * texture2D(CC_Texture0, v_texCoord);
	//float gray = dot(v_orColor.rgb, vec3(sin(CC_Time[1]), 0.587, 0.114));
	//gl_FragColor = vec4(gray, gray, gray, v_orColor.a);

	vec4 c = v_fragmentColor * texture2D(CC_Texture0, v_texCoord);
	
	//vignette
	c = mix(c, vec4(0.0,0.0,0.0,1.0), clamp(length(vec2(0.5, 0.5)-v_texCoord)/0.95,0.0,1.0));

	//desaturate
	float gs = c.r * c.g * c.b;
	c = mix(c, vec4(gs, gs, gs, 1.0), 0.45);

	//brightness
	c = (c + vec4(0.07, 0.07, 0.07, 0.0)) * vec4(1.6, 1.6, 1.6, 1.0);

	gl_FragColor = c;
}				
