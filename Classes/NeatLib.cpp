#include "NeatLib.h"

using namespace std;
USING_NS_CC;

namespace Neat
{
	bool linesIntersect(FLOAT2 l1s, FLOAT2 l1e, FLOAT2 l2s, FLOAT2 l2e, FLOAT2 *intersection) {
		double Ua, Ub;

		Ua = ((l2e.x - l2s.x) * (l1s.y - l2s.y) - (l2e.y - l2s.y) * (l1s.x - l2s.x)) /
			((l2e.y - l2s.y) * (l1e.x - l1s.x) - (l2e.x - l2s.x) * (l1e.y - l1s.y));

		Ub = ((l1e.x - l1s.x) * (l1s.y - l2s.y) - (l1e.y - l1s.y) * (l1s.x - l2s.x)) /
			((l2e.y - l2s.y) * (l1e.x - l1s.x) - (l2e.x - l2s.x) * (l1e.y - l1s.y));

		if ((Ua >= 0.0f && Ua <= 1.0f) &&
			(Ub >= 0.0f && Ub <= 1.0f))
		{
			if (intersection != nullptr)
			{
				intersection->x = l1s.x + Ua * (l1e.x - l1s.x);
				intersection->y = l1s.y + Ua * (l1e.y - l1s.y);
			}
			return true;
		}

		return false;
	}

	float length(float x1, float y1, float x2, float y2)
	{
		return sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
	}

	float lengthsq(float x1, float y1, float x2, float y2)
	{
		return (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2);
	}

	bool nToBool(nstring source)
	{
		source = nToLower(nTrim(source));
		if (source == NS("true") || source == NS("1")) return true;
		return false;
	}

	FLOAT4 nToFloat4(const nstring& source)
	{
		auto split = nSplit(source, NS(','));
		int count = static_cast<int>(split.size());
		FLOAT4 result(0, 0, 0, 0);
		if (count > 0)
		{
			result.x = nToFloat(split[0]);
			if (count > 1)
			{
				result.y = nToFloat(split[1]);
				if (count > 2)
				{
					result.z = nToFloat(split[2]);
					if (count > 3)
						result.w = nToFloat(split[3]);
				}
			}
		}
		return result;
	}

	FLOAT3 nToFloat3(const nstring& source)
	{
		auto split = nSplit(source, NS(','));
		int count = static_cast<int>(split.size());
		FLOAT3 result(0, 0, 0);
		if (count > 0)
		{
			result.x = nToFloat(split[0]);
			if (count > 1)
			{
				result.y = nToFloat(split[1]);
				if (count > 2)
				{
					result.z = nToFloat(split[2]);
				}
			}
		}
		return result;
	}

	FLOAT2 nToFloat2(const nstring& source)
	{
		auto split = nSplit(source, NS(','));
		int count = static_cast<int>(split.size());
		FLOAT2 result(0, 0);
		if (count > 0)
		{
			result.x = nToFloat(split[0]);
			if (count > 1)
			{
				result.y = nToFloat(split[1]);
			}
		}
		return result;
	}

	float nToFloat(const nstring& strbuf)
	{
		std::stringstream converter;
		float value = 0;

		converter.precision(4);
		converter.fill('0');
		converter.setf(std::ios::fixed, std::ios::floatfield);

		converter << strbuf;
		converter >> value;
		return value;
	}

	long nToLong(const nstring& strbuf)
	{
		std::stringstream converter;
		long value = 0;

		converter.fill('0');
		converter.setf(std::ios::fixed, std::ios::floatfield);

		converter << strbuf;
		converter >> value;
		return value;
	}

	nstring nToString(const float value)
	{
		stringstream converter;
		nstring wstr;

		converter << value;
		converter >> wstr;
		return wstr;
	}

	std::string nToString(const long value)
	{
		std::stringstream converter;
		std::string wstr;

		converter << value;
		converter >> wstr;
		return wstr;
	}

	std::string ReadBlock(const string& data, int& pos, const char delim)
	{
		string r = "";
		for (; data[pos] != delim && pos < (int) data.length(); r += data[pos++]);
		pos++;
		return r;
	}

	std::string nToString(const char c)
	{
		return string(1, c);
		/*std::stringstream ss;
		std::string s;
		ss << c;
		ss >> s;
		return s;*/
	}

	nstring nToString(const double value)
	{
		stringstream converter;
		nstring wstr;

		converter << value;
		converter >> wstr;
		return wstr;
	}

	nstring nToString(const bool value)
	{
		return (value ? NS("true") : NS("false"));
	}

	nstring nToString(const int value)
	{
		std::stringstream converter;
		std::string wstr;

		converter << value;
		converter >> wstr;
		return wstr;
	}

	nstring nToString(const FLOAT4& value)
	{
		return nToString(value.x) + NS(",") +
			nToString(value.y) + NS(",") +
			nToString(value.z) + NS(",") +
			nToString(value.w);
	}

	nstring nToString(const FLOAT3& value)
	{
		return nToString(value.x) + NS(',') +
			nToString(value.y) + NS(',') +
			nToString(value.z);
	}

	nstring nToString(const FLOAT2& value)
	{
		return nToString(value.x) + NS(',') +
			nToString(value.y);
	}

	nstring nToString(std::vector<nstring>& args, const int& index)
	{
		if (ITR_SIZE(args) <= index) return "";
		nstring result = args[index];
		for (int i = index + 1; i<ITR_SIZE(args); i++)
			result += NS(" ") + nToString(result[i]);
		return result;
	}

	std::vector<nstring> nSplit(const nstring& source, const nchar& delim)
	{
		vector<nstring> result;
		nstring job;

		for (const nchar& c : source)
		{
			if (c == delim)
			{
				result.push_back(job);
				job = NS("");
			}
			else
				job += c;
		}

		result.push_back(job);
		return result;
	}

	std::vector<nstring> nSplit(const nstring& source, const nstring& delim)
	{
		vector<nstring> result;
		nstring job;

		bool found = false;
		for (const nchar& c : source)
		{
			found = false;
			for (const nchar& d : delim)
			{
				if (c == d)
				{
					found = true;
					break;
				}
			}

			if (found)
			{
				result.push_back(job);
				job = NS("");
			}
			else
				job += c;
		}

		result.push_back(job);
		return result;
	}

	std::vector<nstring> nSplit(const nstring& source, const int& size)
	{
		vector<nstring> result;
		nstring job;
		int count = 0;
		for (const nchar& c : source)
		{
			if (count++ == size)
			{
				result.push_back(job);
				count = 0;
				job = NS("");
			}
			job += c;
		}

		result.push_back(job);
		return result;
	}

	nstring nReplace(nstring sourcestr, nstring findstr, nchar replacestr)
	{
		auto pos = sourcestr.find(findstr);

		if (pos == nstring::npos)
			return sourcestr;

		return sourcestr.replace(
			sourcestr.begin() + pos,
			sourcestr.begin() + pos + findstr.size(),
			1, replacestr);

	}

	nstring nReplaceAll(nstring sourcestr, nstring findstr, nchar replacestr)
	{
		auto pos = sourcestr.find(findstr);

		while (pos != nstring::npos)
		{
			sourcestr = sourcestr.replace(
				sourcestr.begin() + pos,
				sourcestr.begin() + pos + findstr.size(),
				1, replacestr);

			pos = sourcestr.find(findstr);
		}

		return sourcestr;
	}

	nstring nReplace(nstring sourcestr, nstring findstr, nstring replacestr)
	{
		auto pos = sourcestr.find(findstr);

		if (pos == nstring::npos)
			return sourcestr;

		return sourcestr.replace(
			sourcestr.begin() + pos,
			sourcestr.begin() + pos + findstr.size(),
			replacestr.begin(),
			replacestr.end());

	}

	nstring nReplaceAll(nstring sourcestr, nstring findstr, nstring replacestr)
	{
		auto pos = sourcestr.find(findstr);

		while (pos != nstring::npos)
		{
			sourcestr = sourcestr.replace(
				sourcestr.begin() + pos,
				sourcestr.begin() + pos + findstr.size(),
				replacestr.begin(),
				replacestr.end());

			pos = sourcestr.find(findstr);
		}

		return sourcestr;
	}

	nstring nLTrim(nstring sourcestr, const nstring& chars)
	{
		if (sourcestr.size() == 0) return NS("");

		auto itr = sourcestr.begin();
		int pos = 0;
		bool found;
		for (; itr != sourcestr.end(); itr++, pos++)
		{
			found = false;
			for (const nchar& ch : chars)
			{
				if (sourcestr[pos] == ch)
				{
					found = true;
					break;
				}
			}

			if (!found)
			{
				sourcestr = sourcestr.erase(0, pos);
				return sourcestr;
			}
		}

		return NS("");
	}

	nstring nRTrim(nstring sourcestr, const nstring& chars)
	{
		if (sourcestr.size() == 0) return NS("");

		auto itr = sourcestr.rbegin();
		int pos = sourcestr.size() - 1;
		bool found;
		for (; itr != sourcestr.rend(); itr++, pos--)
		{
			found = false;
			for (const nchar& ch : chars)
			{
				if (sourcestr[pos] == ch)
				{
					found = true;
					break;
				}
			}

			if (!found)
			{
				sourcestr = sourcestr.erase(pos + 1, sourcestr.size() - 1);
				return sourcestr;
			}
		}

		return NS("");
	}

	nstring nTrim(nstring sourcestr, const nstring& chars)
	{
		return nRTrim(nLTrim(sourcestr, chars), chars);
	}

	bool nIsWhiteSpace(const nstring& sourcestr)
	{
		return nTrim(sourcestr).size() == 0;
	}

	nstring nToLower(nstring sourcestr)
	{
		const auto diff = L'a' - L'A';
		for (nchar& ch : sourcestr)
			if (ch >= L'A' && ch <= L'Z')
				ch += diff;

		return sourcestr;
	}

	nstring nToUpper(nstring sourcestr)
	{
		const auto diff = L'a' - L'A';
		for (nchar& ch : sourcestr)
			if (ch >= L'a' && ch <= L'z')
				ch -= diff;

		return sourcestr;
	}

	bool nStartsWith(const nstring& sourcestr, const nstring& query)
	{
		if (sourcestr.size() < query.size()) return false;
		for (int i = 0; i < static_cast<int>(query.size()); i++)
			if (sourcestr[i] != query[i]) return false;
		return true;
	}

	bool nEndsWith(const nstring& sourcestr, const nstring& query)
	{
		if (sourcestr.size() < query.size()) return false;
		for (int i = sourcestr.size() - 1, c = 0; i >= 0 && c < static_cast<int>(query.size()); i--, c++)
			if (sourcestr[i] != query[query.size() - 1 - c]) return false;
		return true;
	}

	nstring nSubString(const nstring& sourcestr, const int& start)
	{
		nstring result;
		for (auto itr = start; itr < static_cast<int>(sourcestr.size()); itr++)
			result += sourcestr[itr];
		return result;
	}

	nstring nSubString(const nstring& sourcestr, const int& start, int count)
	{
		if (count == -1)
			return nSubString(sourcestr, start);

		nstring result;
		for (auto itr = start; itr < static_cast<int>(sourcestr.size()), count > 0; itr++, count--)
			result += sourcestr[itr];
		return result;
	}

	nstring nGetFileName(const nstring& sourcestr)
	{
		auto tokens = nSplit(sourcestr, NS("\\/"));
		return *(tokens.end() - 1);
	}

	nstring nGetFileNameWithoutExt(const nstring& sourcestr)
	{
		nstring fname = nGetFileName(sourcestr);
		return nSubString(fname, 0, static_cast<int>(fname.find_last_of(L'.')));
	}

	bool nFileExists(const nstring& path)
	{
		return static_cast<bool>(ifstream(path));
	}

	std::vector<nstring> nReadAllLines(const nstring& path)
	{
		vector<nstring> result;
		_IFSTREAM_ ifile(path);
		if (ifile)
		{
			for (nstring line; getline(ifile, line);)
				result.push_back(line);
		}
		return result;
	}

	vector<string> ReadLinesFromFile(string fpath)
	{
		fpath = FileUtils::getInstance()->fullPathForFilename(fpath);
		auto fileData = FileUtils::getInstance()->getStringFromFile(fpath);
		return ReadLines(fileData);
	}

	vector<string> ReadLines(const string& str)
	{
		return ReadLinesFromCstr(str.c_str(), str.length());
	}

	std::vector<std::string> ReadLinesFromCstr(const char* cstr, const int len)
	{
		vector<string> lines;
		string line = "";
		for (int i = 0; i < len; i++)
		{
			char c = cstr[i];
			if (c == '\n')
			{
				if (static_cast<int>(line.size()) > 0)
				{
					lines.push_back(line);
					line = "";
				}
			}
			else if (c == ' ' || c == '\t')
				line += " ";
			else if (c == '\r');
			else line += nToString(c);
		}
		if (static_cast<int>(line.size()) > 0)
		{
			lines.push_back(line);
		}
		return lines;
	}

	float hue2Rgb(float p, float q, float t)
	{
		if (t < 0) t += 1.0f;
		else if (t > 1) t -= 1.0f;
		else if (t < 1.0f / 6.0f) return p + (q - p)*6.0f*t;
		else if (t < 1.0f / 2.0f) return q;
		else if (t < 2.0f / 3.0f) return p + (q - p)*(2.0f / 3.0f - t) * 6.0f;
		return p;
	}

	Color3B hsl2Rgb(float h, float s, float l)
	{
		float r, g, b;
		if (h > 1.0f) h = 1.0f;
		if (s > 1.0f) s = 1.0f;
		if (l > 1.0f) l = 1.0f;
		if (h < 0) h = 0;
		if (s < 0) s = 0;
		if (l < 0) l = 0;

		if (s == 0)
		{
			r = g = b = l; // achromatic
		}
		else
		{
			auto q = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
			auto p = 2.0f * l - q;
			r = hue2Rgb(p, q, h + 1.0f / 3.0f);
			g = hue2Rgb(p, q, h);
			b = hue2Rgb(p, q, h - 1.0f / 3.0f);
		}

		return Color3B((GLubyte) (r*255.0f + 0.5f), (GLubyte) (g*255.0f + 0.5f), (GLubyte) (b*255.0f + 0.5f));
	}

	Color3B lerp(Color3B _from, Color3B _to, float d)
	{
		return (Color3B(GLubyte(_from.r + (_to.r - _from.r) * d),
			(GLubyte) (_from.g + (_to.g - _from.g) * d),
			(GLubyte) (_from.b + (_to.b - _from.b) * d)));
	}

	Color4B lerp(Color4B _from, Color4B _to, float d)
	{
		return (Color4B(GLubyte(_from.r + (_to.r - _from.r) * d),
			(GLubyte) (_from.g + (_to.g - _from.g) * d),
			(GLubyte) (_from.b + (_to.b - _from.b) * d),
			(GLubyte) (_from.a + (_to.a - _from.a) * d)));
	}

	Color4F lerp(Color4F _from, Color4F _to, float d)
	{
		return (Color4F((_from.r + (_to.r - _from.r) * d),
			(_from.g + (_to.g - _from.g) * d),
			(_from.b + (_to.b - _from.b) * d),
			(_from.a + (_to.a - _from.a) * d)));
	}

	FLOAT4 nColor(double r, double g, double b, double a)
	{
		return FLOAT4(
			static_cast<float>(r),
			static_cast<float>(g),
			static_cast<float>(b),
			static_cast<float>(a));
	}

	std::string string_format(const std::string fmt_str, ...) {
		int final_n, n = ((int) fmt_str.size()) * 2; /* reserve 2 times as much as the length of the fmt_str */
		std::string str;
		std::unique_ptr<char []> formatted;
		va_list ap;
		while (1) {
			formatted.reset(new char[n]); /* wrap the plain char array into the unique_ptr */
			strcpy(&formatted[0], fmt_str.c_str());
			va_start(ap, fmt_str);
			final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
			va_end(ap);
			if (final_n < 0 || final_n >= n)
				n += abs(final_n - n + 1);
			else
				break;
		}
		return std::string(formatted.get());
	}
}