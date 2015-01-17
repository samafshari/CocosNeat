#ifndef __NEATLIB_H__
#define __NEATLIB_H__

#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <functional>
#include <cmath>
#include <memory>

#include "cocos2d.h"

#define RANDF (static_cast<float>(rand()) / (float)RAND_MAX)
#define RANDF1(min) ( (min) + (1.0f-static_cast<float>((min))) * RANDF )
#define RANDF2(min,max) ( (min) + (static_cast<float>((max))-static_cast<float>((min))) * RANDF )
#define RANDINT(min, max) (min + rand() % (1 + max - min))

#define ITR_LAST(x) ((x)[static_cast<int>((x).size())-1])
#define ITR_SIZE(x) (static_cast<int>((x).size()))
#define ITR_PTR(x,i) (*(x))[i]
#define CHOOSE(vec) (vec[rand() % vec.size()])
#define CONTAINS(m, e) (m.find(e) != m.end())

#if defined(ENABLE_WIDE_CHAR)
#define NS(c) L(c)
#define _IFSTREAM_ wifstream
#define _OFSTREAM_ wofstream
#else
#define NS(c) (c)
#define _IFSTREAM_ ifstream
#define _OFSTREAM_ ofstream
#endif

#define GETVECTORX(v) (v.x)
#define GETVECTORY(v) (v.y)
#define SETVECTORX(v, x) v.x = (x)
#define SETVECTORY(v, y) v.y = (y)

//#define NAN (_Nan._Float)
#define NASSERT(expr) {auto ___e__ = (expr); assert(___e__);}

namespace Neat
{

#if defined(ENABLE_WIDE_CHAR)
	typedef std::wstring nstring;
	typedef wchar_t nchar;
#else
	typedef std::string nstring;
	typedef char nchar;
#endif

	typedef cocos2d::Vec2 FLOAT2;
	typedef cocos2d::Vec3 FLOAT3;
	typedef cocos2d::Vec4 FLOAT4;
	typedef cocos2d::Color4F COLOR4F;
	typedef cocos2d::Color4B COLOR4B;
	typedef cocos2d::Color3B COLOR3B;
	typedef GLubyte BYTE;
	FLOAT4 nColor(double, double, double, double);

	bool linesIntersect(FLOAT2 p1, FLOAT2 p2, FLOAT2 p3, FLOAT2 p4, FLOAT2* = nullptr);
	float length(float x1, float y1, float x2 = 0, float y2 = 0);
	float lengthsq(float x1, float y1, float x2 = 0, float y2 = 0);

	bool nToBool(const nstring source);

	FLOAT4 nToFloat4(const nstring&);
	FLOAT3 nToFloat3(const nstring&);
	FLOAT2 nToFloat2(const nstring&);
	float nToFloat(const nstring&);
	long nToLong(const nstring&);
	inline int nToInt(const nstring& strbuf) { return static_cast<int>(nToFloat(strbuf)); }

	nstring nToString(const float value);
	nstring nToString(const double value);
	nstring nToString(const bool value);
	nstring nToString(const int value);
	nstring nToString(const long value);
	nstring nToString(const char);
	nstring nToString(const FLOAT4& value);
	nstring nToString(const FLOAT3& value);
	nstring nToString(const FLOAT2& value);

	nstring nToString(std::vector<nstring>& args, const int& index);

	nstring ReadBlock(const nstring& data, int& pos, const char delim = NS('\n'));

	std::vector<nstring> ReadLinesFromFile(nstring fpath);
	std::vector<nstring> ReadLines(const nstring& str);
	std::vector<nstring> ReadLinesFromCstr(const nchar* cstr, const int len);

	std::vector<nstring> nSplit(const nstring& source, const nchar& delim = NS(' '));
	std::vector<nstring> nSplit(const nstring& source, const nstring& delim);
	std::vector<nstring> nSplit(const nstring& source, const int& size);

	nstring nReplace(nstring sourcestr, nstring findstr, nstring replacestr);
	nstring nReplace(nstring sourcestr, nstring findstr, nchar replacestr);
	nstring nReplaceAll(nstring sourcestr, nstring findstr, nstring replacestr);
	nstring nReplaceAll(nstring sourcestr, nstring findstr, nchar replacestr);

	nstring nLTrim(nstring sourcestr, const nstring& chars = NS(" \n\t"));
	nstring nRTrim(nstring sourcestr, const nstring& chars = NS(" \n\t"));
	nstring nTrim(nstring sourcestr, const nstring& chars = NS(" \n\t"));

	bool nIsWhiteSpace(const nstring& sourcestr);

	nstring nToLower(nstring sourcestr);
	nstring nToUpper(nstring sourcestr);

	bool nStartsWith(const nstring& sourcestr, const nstring& query);
	bool nEndsWith(const nstring& sourcestr, const nstring& query);

	nstring nSubString(const nstring& sourcestr, const int& start);
	nstring nSubString(const nstring& sourcestr, const int& start, int count);

	nstring nGetFileName(const nstring& sourcestr);
	nstring nGetFileNameWithoutExt(const nstring& sourcestr);

	bool nFileExists(const nstring& path);
	std::vector<nstring> nReadAllLines(const nstring& path);

	std::vector<nstring> nListFiles(nstring path);

	inline float clamp(float c, float a, float b) {
		if (c < a) return a;
		if (c > b) return b;
		return c;
	}

	inline float lerp(float x, float y, float t) {
		return x + t * (y - x);
	}

	inline static float contrast(float base)
	{
		int t = 128;
		return clamp(base + static_cast<float>(rand() % t - t / 2), 0.0f, 255.0f);
	}

	inline void nMinMax(const FLOAT2& p1, const FLOAT2& p2, FLOAT2& o_min, FLOAT2& o_max)
	{
		o_min.x = std::min(p1.x, p2.x);
		o_min.y = std::min(p1.y, p2.y);
		o_max.x = std::max(p1.x, p2.x);
		o_max.y = std::max(p1.y, p2.y);
	}

	float hue2Rgb(float p, float q, float t);
	COLOR3B hsl2Rgb(float h, float s, float l);

	COLOR3B lerp(COLOR3B _from, COLOR3B _to, float d);
	COLOR4B lerp(COLOR4B _from, COLOR4B _to, float d);
	COLOR4F lerp(COLOR4F _from, COLOR4F _to, float d);

	inline COLOR4B C4Fto4B(float r, float g, float b, float a)
	{
		return (COLOR4B((float) (r) * 255, (float) (g) * 255, (float) (b) * 255, (float) (a) * 255));
	}

	/**
	* formats a string
	* http://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
	*/
	std::string string_format(const std::string fmt_str, ...);
}
#endif