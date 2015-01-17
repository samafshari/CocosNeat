#include "NeatScriptInterpreter.h"

using namespace std;
using namespace Neat;

const int ScriptInterpreter::OPR_SHIFT = 256;

void ScriptInterpreter::initExpressionEvaluation(void)
{
	m_operators.push_back(Operator(NS("^"), nEvaluatePow, 5));
	m_operators.push_back(Operator(NS("%"), nEvaluateMod, 4));
	m_operators.push_back(Operator(NS("/"), nEvaluateDiv, 4));
	m_operators.push_back(Operator(NS("*"), nEvaluateMul, 4));
	m_operators.push_back(Operator(NS("+"), nEvaluateAdd, 3));
	m_operators.push_back(Operator(NS("-"), nEvaluateSub, 3));
	minusOperatorIdx = ITR_SIZE(m_operators)-1;
	m_operators.push_back(Operator(NS("&"), nEvaluateAnd, 2));
	m_operators.push_back(Operator(NS("|"), nEvaluateOr, 2));
	m_operators.push_back(Operator(NS("#"), nEvaluateXor, 2));
	m_operators.push_back(Operator(NS("!"), nEvaluateNot, 2, true));
	m_operators.push_back(Operator(NS("=="), nEvaluateCmpEqual, 1));
	m_operators.push_back(Operator(NS("!="), nEvaluateCmpNotEqual, 1));
	m_operators.push_back(Operator(NS(">"), nEvaluateCmpGreaterThan, 1));
	m_operators.push_back(Operator(NS(">="), nEvaluateCmpGreaterEqualThan, 1));
	m_operators.push_back(Operator(NS("<"), nEvaluateCmpLessThan, 1));
	m_operators.push_back(Operator(NS("<="), nEvaluateCmpLessEqualThan, 1));
	sortOperators();
}

void ScriptInterpreter::sortOperators(void)
{
	m_sortedOperatorsByLength.clear();
	m_sortedOperatorsByOrder.clear();
	bool found = false;
	int c = 0;

	for (Operator& op : m_operators)
	{
		Operator* ptr = &op;

		if (c++ == 0)
		{
			m_sortedOperatorsByLength.push_back(ptr);
			m_sortedOperatorsByOrder.push_back(ptr);
		}
		else
		{
			found = false;
			for (auto itr = m_sortedOperatorsByLength.begin(); itr != m_sortedOperatorsByLength.end(); itr++)
			{
				if ((*itr)->m_sign.size() < op.m_sign.size())
				{
					m_sortedOperatorsByLength.insert(itr, ptr);
					found = true;
					break;
				}
			}
			if (!found)
				m_sortedOperatorsByLength.push_back(ptr);

			found = false;
			for (auto itr = m_sortedOperatorsByOrder.begin(); itr != m_sortedOperatorsByOrder.end(); itr++)
			{
				if ((*itr)->m_order >= static_cast<int>(op.m_sign.size()))
				{
					m_sortedOperatorsByOrder.insert(itr, ptr);
					found = true;
					break;
				}
			}
			if (!found)
				m_sortedOperatorsByOrder.push_back(ptr);
		}
	}

	int i = ScriptInterpreter::OPR_SHIFT;
	for (auto ptr : m_sortedOperatorsByLength)
		ptr->m_tag = (nchar)(i++);
}

void ScriptInterpreter::initColors(void)
{
	m_colors[NS("aliceblue")] = nColor(0.9411765,0.972549,1,1);
	m_colors[NS("antiquewhite")] = nColor(0.9803922,0.9215686,0.8431373,1);
	m_colors[NS("aqua")] = nColor(0,1,1,1);
	m_colors[NS("aquamarine")] = nColor(0.4980392,1,0.8313726,1);
	m_colors[NS("azure")] = nColor(0.9411765,1,1,1);
	m_colors[NS("beige")] = nColor(0.9607843,0.9607843,0.8627451,1);
	m_colors[NS("bisque")] = nColor(1,0.8941177,0.7686275,1);
	m_colors[NS("black")] = nColor(0,0,0,1);
	m_colors[NS("blanchedalmond")] = nColor(1,0.9215686,0.8039216,1);
	m_colors[NS("blue")] = nColor(0,0,1,1);
	m_colors[NS("blueviolet")] = nColor(0.5411765,0.1686275,0.8862745,1);
	m_colors[NS("brown")] = nColor(0.6470588,0.1647059,0.1647059,1);
	m_colors[NS("burlywood")] = nColor(0.8705882,0.7215686,0.5294118,1);
	m_colors[NS("cadetblue")] = nColor(0.372549,0.6196079,0.627451,1);
	m_colors[NS("chartreuse")] = nColor(0.4980392,1,0,1);
	m_colors[NS("chocolate")] = nColor(0.8235294,0.4117647,0.1176471,1);
	m_colors[NS("coral")] = nColor(1,0.4980392,0.3137255,1);
	m_colors[NS("cornflowerblue")] = nColor(0.3921569,0.5843138,0.9294118,1);
	m_colors[NS("cornsilk")] = nColor(1,0.972549,0.8627451,1);
	m_colors[NS("crimson")] = nColor(0.8627451,0.07843138,0.2352941,1);
	m_colors[NS("cyan")] = nColor(0,1,1,1);
	m_colors[NS("darkblue")] = nColor(0,0,0.5450981,1);
	m_colors[NS("darkcyan")] = nColor(0,0.5450981,0.5450981,1);
	m_colors[NS("darkgoldenrod")] = nColor(0.7215686,0.5254902,0.04313726,1);
	m_colors[NS("darkgray")] = nColor(0.6627451,0.6627451,0.6627451,1);
	m_colors[NS("darkgreen")] = nColor(0,0.3921569,0,1);
	m_colors[NS("darkkhaki")] = nColor(0.7411765,0.7176471,0.4196078,1);
	m_colors[NS("darkmagenta")] = nColor(0.5450981,0,0.5450981,1);
	m_colors[NS("darkolivegreen")] = nColor(0.3333333,0.4196078,0.1843137,1);
	m_colors[NS("darkorange")] = nColor(1,0.5490196,0,1);
	m_colors[NS("darkorchid")] = nColor(0.6,0.1960784,0.8,1);
	m_colors[NS("darkred")] = nColor(0.5450981,0,0,1);
	m_colors[NS("darksalmon")] = nColor(0.9137255,0.5882353,0.4784314,1);
	m_colors[NS("darkseagreen")] = nColor(0.5607843,0.7372549,0.5450981,1);
	m_colors[NS("darkslateblue")] = nColor(0.282353,0.2392157,0.5450981,1);
	m_colors[NS("darkslategray")] = nColor(0.1843137,0.3098039,0.3098039,1);
	m_colors[NS("darkturquoise")] = nColor(0,0.8078431,0.8196079,1);
	m_colors[NS("darkviolet")] = nColor(0.5803922,0,0.827451,1);
	m_colors[NS("deeppink")] = nColor(1,0.07843138,0.5764706,1);
	m_colors[NS("deepskyblue")] = nColor(0,0.7490196,1,1);
	m_colors[NS("dimgray")] = nColor(0.4117647,0.4117647,0.4117647,1);
	m_colors[NS("dodgerblue")] = nColor(0.1176471,0.5647059,1,1);
	m_colors[NS("firebrick")] = nColor(0.6980392,0.1333333,0.1333333,1);
	m_colors[NS("floralwhite")] = nColor(1,0.9803922,0.9411765,1);
	m_colors[NS("forestgreen")] = nColor(0.1333333,0.5450981,0.1333333,1);
	m_colors[NS("fuchsia")] = nColor(1,0,1,1);
	m_colors[NS("gainsboro")] = nColor(0.8627451,0.8627451,0.8627451,1);
	m_colors[NS("ghostwhite")] = nColor(0.972549,0.972549,1,1);
	m_colors[NS("gold")] = nColor(1,0.8431373,0,1);
	m_colors[NS("goldenrod")] = nColor(0.854902,0.6470588,0.1254902,1);
	m_colors[NS("gray")] = nColor(0.5019608,0.5019608,0.5019608,1);
	m_colors[NS("green")] = nColor(0,0.5019608,0,1);
	m_colors[NS("greenyellow")] = nColor(0.6784314,1,0.1843137,1);
	m_colors[NS("honeydew")] = nColor(0.9411765,1,0.9411765,1);
	m_colors[NS("hotpink")] = nColor(1,0.4117647,0.7058824,1);
	m_colors[NS("indianred")] = nColor(0.8039216,0.3607843,0.3607843,1);
	m_colors[NS("indigo")] = nColor(0.2941177,0,0.509804,1);
	m_colors[NS("ivory")] = nColor(1,1,0.9411765,1);
	m_colors[NS("khaki")] = nColor(0.9411765,0.9019608,0.5490196,1);
	m_colors[NS("lavender")] = nColor(0.9019608,0.9019608,0.9803922,1);
	m_colors[NS("lavenderblush")] = nColor(1,0.9411765,0.9607843,1);
	m_colors[NS("lawngreen")] = nColor(0.4862745,0.9882353,0,1);
	m_colors[NS("lemonchiffon")] = nColor(1,0.9803922,0.8039216,1);
	m_colors[NS("lightblue")] = nColor(0.6784314,0.8470588,0.9019608,1);
	m_colors[NS("lightcoral")] = nColor(0.9411765,0.5019608,0.5019608,1);
	m_colors[NS("lightcyan")] = nColor(0.8784314,1,1,1);
	m_colors[NS("lightgoldenrodyellow")] = nColor(0.9803922,0.9803922,0.8235294,1);
	m_colors[NS("lightgray")] = nColor(0.827451,0.827451,0.827451,1);
	m_colors[NS("lightgreen")] = nColor(0.5647059,0.9333333,0.5647059,1);
	m_colors[NS("lightpink")] = nColor(1,0.7137255,0.7568628,1);
	m_colors[NS("lightsalmon")] = nColor(1,0.627451,0.4784314,1);
	m_colors[NS("lightseagreen")] = nColor(0.1254902,0.6980392,0.6666667,1);
	m_colors[NS("lightskyblue")] = nColor(0.5294118,0.8078431,0.9803922,1);
	m_colors[NS("lightslategray")] = nColor(0.4666667,0.5333334,0.6,1);
	m_colors[NS("lightsteelblue")] = nColor(0.6901961,0.7686275,0.8705882,1);
	m_colors[NS("lightyellow")] = nColor(1,1,0.8784314,1);
	m_colors[NS("lime")] = nColor(0,1,0,1);
	m_colors[NS("limegreen")] = nColor(0.1960784,0.8039216,0.1960784,1);
	m_colors[NS("linen")] = nColor(0.9803922,0.9411765,0.9019608,1);
	m_colors[NS("magenta")] = nColor(1,0,1,1);
	m_colors[NS("maroon")] = nColor(0.5019608,0,0,1);
	m_colors[NS("mediumaquamarine")] = nColor(0.4,0.8039216,0.6666667,1);
	m_colors[NS("mediumblue")] = nColor(0,0,0.8039216,1);
	m_colors[NS("mediumorchid")] = nColor(0.7294118,0.3333333,0.827451,1);
	m_colors[NS("mediumpurple")] = nColor(0.5764706,0.4392157,0.8588235,1);
	m_colors[NS("mediumseagreen")] = nColor(0.2352941,0.7019608,0.4431373,1);
	m_colors[NS("mediumslateblue")] = nColor(0.4823529,0.4078431,0.9333333,1);
	m_colors[NS("mediumspringgreen")] = nColor(0,0.9803922,0.6039216,1);
	m_colors[NS("mediumturquoise")] = nColor(0.282353,0.8196079,0.8,1);
	m_colors[NS("mediumvioletred")] = nColor(0.7803922,0.08235294,0.5215687,1);
	m_colors[NS("midnightblue")] = nColor(0.09803922,0.09803922,0.4392157,1);
	m_colors[NS("mintcream")] = nColor(0.9607843,1,0.9803922,1);
	m_colors[NS("mistyrose")] = nColor(1,0.8941177,0.8823529,1);
	m_colors[NS("moccasin")] = nColor(1,0.8941177,0.7098039,1);
	m_colors[NS("navajowhite")] = nColor(1,0.8705882,0.6784314,1);
	m_colors[NS("navy")] = nColor(0,0,0.5019608,1);
	m_colors[NS("oldlace")] = nColor(0.9921569,0.9607843,0.9019608,1);
	m_colors[NS("olive")] = nColor(0.5019608,0.5019608,0,1);
	m_colors[NS("olivedrab")] = nColor(0.4196078,0.5568628,0.1372549,1);
	m_colors[NS("orange")] = nColor(1,0.6470588,0,1);
	m_colors[NS("orangered")] = nColor(1,0.2705882,0,1);
	m_colors[NS("orchid")] = nColor(0.854902,0.4392157,0.8392157,1);
	m_colors[NS("palegoldenrod")] = nColor(0.9333333,0.9098039,0.6666667,1);
	m_colors[NS("palegreen")] = nColor(0.5960785,0.9843137,0.5960785,1);
	m_colors[NS("paleturquoise")] = nColor(0.6862745,0.9333333,0.9333333,1);
	m_colors[NS("palevioletred")] = nColor(0.8588235,0.4392157,0.5764706,1);
	m_colors[NS("papayawhip")] = nColor(1,0.9372549,0.8352941,1);
	m_colors[NS("peachpuff")] = nColor(1,0.854902,0.7254902,1);
	m_colors[NS("peru")] = nColor(0.8039216,0.5215687,0.2470588,1);
	m_colors[NS("pink")] = nColor(1,0.7529412,0.7960784,1);
	m_colors[NS("plum")] = nColor(0.8666667,0.627451,0.8666667,1);
	m_colors[NS("powderblue")] = nColor(0.6901961,0.8784314,0.9019608,1);
	m_colors[NS("purple")] = nColor(0.5019608,0,0.5019608,1);
	m_colors[NS("red")] = nColor(1,0,0,1);
	m_colors[NS("rosybrown")] = nColor(0.7372549,0.5607843,0.5607843,1);
	m_colors[NS("royalblue")] = nColor(0.254902,0.4117647,0.8823529,1);
	m_colors[NS("saddlebrown")] = nColor(0.5450981,0.2705882,0.07450981,1);
	m_colors[NS("salmon")] = nColor(0.9803922,0.5019608,0.4470588,1);
	m_colors[NS("sandybrown")] = nColor(0.9568627,0.6431373,0.3764706,1);
	m_colors[NS("seagreen")] = nColor(0.1803922,0.5450981,0.3411765,1);
	m_colors[NS("seashell")] = nColor(1,0.9607843,0.9333333,1);
	m_colors[NS("sienna")] = nColor(0.627451,0.3215686,0.1764706,1);
	m_colors[NS("silver")] = nColor(0.7529412,0.7529412,0.7529412,1);
	m_colors[NS("skyblue")] = nColor(0.5294118,0.8078431,0.9215686,1);
	m_colors[NS("slateblue")] = nColor(0.4156863,0.3529412,0.8039216,1);
	m_colors[NS("slategray")] = nColor(0.4392157,0.5019608,0.5647059,1);
	m_colors[NS("snow")] = nColor(1,0.9803922,0.9803922,1);
	m_colors[NS("springgreen")] = nColor(0,1,0.4980392,1);
	m_colors[NS("steelblue")] = nColor(0.2745098,0.509804,0.7058824,1);
	m_colors[NS("tan")] = nColor(0.8235294,0.7058824,0.5490196,1);
	m_colors[NS("teal")] = nColor(0,0.5019608,0.5019608,1);
	m_colors[NS("thistle")] = nColor(0.8470588,0.7490196,0.8470588,1);
	m_colors[NS("tomato")] = nColor(1,0.3882353,0.2784314,1);
	m_colors[NS("transparent")] = nColor(0,0,0,0);
	m_colors[NS("turquoise")] = nColor(0.2509804,0.8784314,0.8156863,1);
	m_colors[NS("violet")] = nColor(0.9333333,0.509804,0.9333333,1);
	m_colors[NS("wheat")] = nColor(0.9607843,0.8705882,0.7019608,1);
	m_colors[NS("white")] = nColor(1,1,1,1);
	m_colors[NS("whitesmoke")] = nColor(0.9607843,0.9607843,0.9607843,1);
	m_colors[NS("yellow")] = nColor(1,1,0,1);
	m_colors[NS("yellowgreen")] = nColor(0.6039216,0.8039216,0.1960784,1);
}

ScriptInterpreter::ScriptInterpreter(void) :
	m_eval_open(NS('(')),
	m_eval_close(NS(')')),
	m_bad_command_msg(NS("Bad Command.")),
	m_max_lines(64),
	m_max_chars(150),
	m_echo(true)
{
	initColors();
	initExpressionEvaluation();

	AddCommand(NS("echo"), SCRIPT_LAMBDA
	{
		auto msg = nTrim(nToString(args,1));
		if (msg == NS(".")) engine->WriteLine();
		else if (nToLower(msg) == NS("off")) engine->SetEcho(false);
		else if (nToLower(msg) == NS("on")) engine->SetEcho(true);
		else
		{
			engine->WriteLine(nToString(args,1));
		}
	}, NS("echo [message] : prints a message\necho [off|on] toggles echo"));

	AddCommand(
		NS("help"), 
		SCRIPT_LAMBDA {
			nstring command = NS("help");
			if (ITR_SIZE(args) > 1) command = args[1];
			engine->PrintHelp(command);
		},
		AUTOCOMPLETE_LAMBDA {
			return engine->GetCommands();
		},
		NS("help [command] : shows the usage of a command"));

	AddCommand(
		NS("dv"),
		SCRIPT_LAMBDA {
			if (ITR_SIZE(args) > 2)
				engine->m_ram.Set(args[1], nToString(args, 2));
			else if (ITR_SIZE(args) == 2)
				engine->m_ram.Set(args[1], NS(""));
			else
				engine->WriteLine(NS("Not enough parameters."));
		},
		AUTOCOMPLETE_LAMBDA {
			return engine->m_ram.GetVars();
		},
		NS("dv [var] [value] : sets a dvar"));

	AddCommand(
		NS("clear"),
		SCRIPT_LAMBDA {
			engine->Clear();
		},
		NS("clear : clears the console output"));

	AddCommand(
		NS("loop"),
		SCRIPT_LAMBDA {
			int count = nToInt(args[1]);
			nstring command = nToString(args, 2);
			for (int i = 0; i < count; i++) engine->Run(command);
		},
		AUTOCOMPLETE_LAMBDA {
			if (ITR_SIZE(args) > 1) return engine->GetCommands();
			vector<nstring> result;
			return result;
		},
		NS("loop [count] [command] : executes a command for a specified number of times"));
	
	AddCommand(
		NS("assign"),
		SCRIPT_LAMBDA {
			if (ITR_SIZE(args) == 3)
			{
				try
				{
					engine->AddCommand(args[1], args[2]);
					engine->WriteLine(NS("OK"));
				}
				catch (exception e)
				{
					engine->WriteLine(NS("FAIL"));
				}
				return;
			}
			engine->WriteLine("Syntax error.");
		},
		AUTOCOMPLETE_LAMBDA {
			return engine->GetCommands();
		},
		NS("assign [new command] [existing command] : Assigns an alias for an existing command"));

	WriteLine(NS("Neat Script initialized. Private Build"));
}


ScriptInterpreter::~ScriptInterpreter(void)
{
}

void ScriptInterpreter::Run(nstring cmd)
{
	if (m_echo) WriteLine(NS("  > " + cmd));

	try
	{
		cmd = nReplaceAll(cmd, NS("$_"), NS("\n"));
		cmd = nReplaceAll(cmd, NS("$S"), NS(" "));
		auto lines = nSplit(cmd, NS('\n'));

		for (const nstring& line : lines)
		{
			auto lt = nTrim(line);
			if (lt.size() > 0)
				Run(nSplit(ParseParenthesis(lt), NS(' ')));
		}
	}
	catch (exception e)
	{
		WriteLine(NS("Error while parsing command: [EX HANDLING NOT IMPLEMENTED]"));
	}
}

void ScriptInterpreter::Run(const std::vector<nstring>& args)
{
	auto command = nToLower(args[0]);
	if (command == NS("rem")) return;
	auto itr = m_commands.find(command);
	if (itr == m_commands.end())
	{
		//No match in commands, try looking in buffered scripts
		auto itr = m_bufferedScripts.find(command);
		if (itr == m_bufferedScripts.end())
		{
			//No match anywhere, echo bad command message
			WriteLine(m_bad_command_msg);
			return;
		}
		else
		{
			//Script found, execute it.
			ExecuteBatch(itr->second);
			return;
		}
	}

	//Command found, execute it.
	itr->second(args, this);
}

void ScriptInterpreter::AddCommand(
			const nstring& key, const nScriptFunction& action, 
			const nstring& info)
{
	AddCommand(key, action, nullptr, info);
}

void ScriptInterpreter::AddCommand(const nstring& key, const nScriptFunction& action, 
			const nAutocompleteFunction& autoComplete, //=nullptr
			const nstring& info) //=NS(""
{
	auto keytr = nToLower(nTrim(key));
	if (keytr.size() == 0 || action == nullptr)
	{
		//Key or action is empty, stop.
		WriteLine(NS("AddCommand failed with key ") + keytr);
		return;
	}

	m_commands[keytr] = action;

	//remove old auto complete and info
	auto aci = m_autocompletes.find(keytr);
	if (aci != m_autocompletes.end()) m_autocompletes.erase(aci);
	auto ii = m_infos.find(keytr);
	if (ii != m_infos.end()) m_infos.erase(ii);

	if (autoComplete != nullptr)
		m_autocompletes[keytr] = autoComplete;
	if (!nIsWhiteSpace(info))
		m_infos[keytr] = info;
}

void ScriptInterpreter::AddCommand(
			const nstring& newAlias, const nstring& originalName){
	auto keytr = nToLower(nTrim(newAlias));
	auto dsttr = nToLower(nTrim(originalName));
	if (keytr.size() == 0 || dsttr.size() == 0)
	{
		//Key or action is empty, stop.
		WriteLine(NS("AddCommand failed with key ") + keytr);
		return;
	}

	auto dstitr = m_commands.find(dsttr);
	if (dstitr == m_commands.end()) //command not found
	{
		WriteLine(NS("AddCommand failed: Command not found."));
		return;
	}

	m_commands[keytr] = dstitr->second;
	auto cmpitr = m_autocompletes.find(dsttr);
	if (cmpitr != m_autocompletes.end())
		m_autocompletes[keytr] = cmpitr->second;

	auto infoitr = m_infos.find(dsttr);
	if (infoitr != m_infos.end())
		m_infos[keytr] = infoitr->second;
}

bool ScriptInterpreter::RemoveCommand(const nstring& key)
{
	auto keytr = nToLower(nTrim(key));
	if (keytr.size() == 0) return false; //Key is blank

	auto keyi = m_commands.find(keytr);
	if (keyi == m_commands.end()) return false; //Key not found

	m_commands.erase(keyi);
	return true;
}

void ScriptInterpreter::AutoComplete(nstring& input)
{
	//input = nTrim(input);
	auto args = nSplit(input, NS(' '));
	auto lastArg = *(args.end() - 1);
	auto lastArgTrim = nTrim(lastArg);
	auto oldLastArg = lastArg;
	
	auto command = nToLower(args[0]);
	shared_ptr<vector<nstring> > keys;
	if (args.size() <= 1)
		keys = make_shared<vector<nstring> >(GetCommands());
	else
	{
		auto itr = m_autocompletes.find(command);
		if (itr != m_autocompletes.end())
			keys = make_shared<vector<nstring> >(itr->second(args, this));
		else
			keys = make_shared<vector<nstring> >(GetCommands());
	}

	vector<nstring> s;
	if (keys->size() != 0)
		for (const nstring& item : *keys)
		{
			if (nStartsWith(item, lastArgTrim)) s.push_back(item);
		}
	if (s.size() == 0)
	{
		//No autocompletes available for current query
		WriteLine(NS("No results."));
		return;
	}
	if (s.size() == 1)
	{
		//Only one command available
		lastArg += nSubString(s[0], lastArgTrim.size()) + NS(" ");
	}
	else
	{
		{
			auto sample = s[0];
			int cindex = lastArgTrim.size();
			for (; cindex <= static_cast<int>(sample.size()); cindex++)
			{
				bool done = false;
				for (int i = 1; i < static_cast<int>(s.size()); i++)
				{
					if (!nStartsWith(s[i], nSubString(sample, 0, cindex)))
					{
						done = true;
						break;
					}
				}
				if (done) break;
			}

			lastArg = nSubString(sample, 0, cindex - 1);
			if (lastArg.size() < oldLastArg.size()) lastArg = oldLastArg;
		}
		for (int i = 0; i < static_cast<int>(s.size()); i++)
		{
			Write(s[i] + NS("     "));
			if (i > 0 && i + 5 == 0) WriteLine();
		}
		WriteLine();
	}

	if (lastArg != oldLastArg)
	{
		input = NS("");
		for (int i = 0; i < static_cast<int>(args.size()) - 1; i++)
			input += args[i] + NS(" ");
		input += lastArg;
	}
}

void ScriptInterpreter::PrintHelp(nstring command)
{
	command = nToLower(nTrim(command));
	auto itr = m_infos.find(command);
	if (itr != m_infos.end())
		WriteLine(itr->second);
	else
		WriteLine(NS("No help available for command ") + command);
}

void ScriptInterpreter::Write(const nstring& msg)
{
	nstring lastline;
	if (m_buffer.size() == 0) lastline = msg;
	else lastline = *(m_buffer.end() - 1) + msg;
	
	auto newlines = nSplit(lastline, NS('\n'));
	if (newlines.size() == 1)
	{
		//good. only one line. let's see how long it is.
		newlines = nSplit(newlines[0], m_max_chars);
		if (newlines.size() == 1)
		{
			//good. still one line. let's put it back.
			if (ITR_SIZE(m_buffer) == 0) m_buffer.push_back(newlines[0]);
			else *(m_buffer.end() - 1) = newlines[0];
			return;
		}
		else
		{
			//nope. no \ns, but lines are big.
			if (ITR_SIZE(m_buffer) == 0) m_buffer.push_back(newlines[0]);
			else
			{
				*(m_buffer.end() - 1) = newlines[0];
				for (auto itr = newlines.begin() + 1; itr != newlines.end(); itr++)
					m_buffer.push_back(*itr);
			}
			return;
		}
	}
	else
	{
		//first line's different
		auto firstlinecoll = nSplit(newlines[0], m_max_chars);
		if (firstlinecoll.size() == 1)
		{
			//first line's within limits.
			if (ITR_SIZE(m_buffer) == 0) m_buffer.push_back(firstlinecoll[0]);
			else *(m_buffer.end() - 1) = firstlinecoll[0];
		}
		else
		{
			if (ITR_SIZE(m_buffer) == 0) m_buffer.push_back(firstlinecoll[0]);
			else
			{
				*(m_buffer.end() - 1) = firstlinecoll[0];
				for (auto itr = firstlinecoll.begin(); itr != firstlinecoll.end(); itr++)
					m_buffer.push_back(*itr);
			}
		}
		//now let's do other lines
		for (auto l_itr = newlines.begin() + 1; l_itr != newlines.end(); l_itr++)
		{
			auto linecoll = nSplit(*l_itr, m_max_chars);
			for (auto itr = linecoll.begin(); itr != linecoll.end(); itr++)
				m_buffer.push_back(*itr);
		}
	}

	if (static_cast<int>(m_buffer.size()) > m_max_lines)
	{
		m_buffer.erase(m_buffer.begin(), m_buffer.end()-m_max_lines);
	}
}

void ScriptInterpreter::WriteLine(const nstring& msg)
{
	Write(msg + NS("\n"));
}

void ScriptInterpreter::Write(const float msg)
{
	Write(nToString(msg));
}

void ScriptInterpreter::Write(const double msg)
{
	Write(nToString(msg));
}

void ScriptInterpreter::Write(const FLOAT2 msg)
{
	Write(nToString(msg));
}

void ScriptInterpreter::Write(const FLOAT3 msg)
{
	Write(nToString(msg));
}

void ScriptInterpreter::Write(const FLOAT4 msg)
{
	Write(nToString(msg));
}

void ScriptInterpreter::Write(const bool msg)
{
	Write(nToString(msg));
}

void ScriptInterpreter::WriteLine(const float msg)
{
	WriteLine(nToString(msg));
}

void ScriptInterpreter::WriteLine(const double msg)
{
	WriteLine(nToString(msg));
}

void ScriptInterpreter::WriteLine(const FLOAT2 msg)
{
	WriteLine(nToString(msg));
}

void ScriptInterpreter::WriteLine(const FLOAT3 msg)
{
	WriteLine(nToString(msg));
}

void ScriptInterpreter::WriteLine(const FLOAT4 msg)
{
	WriteLine(nToString(msg));
}

void ScriptInterpreter::WriteLine(const bool msg)
{
	WriteLine(nToString(msg));
}

vector<nstring> ScriptInterpreter::GetMessages(void) const
{
	return m_buffer;
}

vector<nstring> ScriptInterpreter::GetMessages(const int& count, const int& offset) const
{
	if (static_cast<int>(m_buffer.size()) <= count - offset)
		return m_buffer;

	vector<nstring> messages;
	for (auto itr = m_buffer.end() - count - offset; itr != m_buffer.end() - offset; itr++)
		messages.push_back(*itr);

	return messages;
}

nstring ScriptInterpreter::ParseParenthesis(nstring& command) // const
{
	command = nTrim(command);
	if (command.size() < 1) return command;

	auto start = command.find(m_eval_open);
	if (start != command.npos)
	{
		auto end = start;
		for (auto i = start; i < command.size() && i >= 0 && i != command.npos; i++)
		{
			if (command[i] == m_eval_open)
			{
				start = i;
			}
			else if (command[i] == m_eval_close)
			{
				end = i;
				nstring left = nSubString(command, 0, start);
				nstring right = nSubString(command, end+1);
				nstring mid = EvaluateString(nSubString(command, start + 1, end - start - 1));
				command = left + m_ram.GetValue(mid) + right;
				start = command.find(m_eval_open);
				i = start;
			}
		}
	}

	return command;
}

FLOAT4 ScriptInterpreter::ParseColor(nstring color)
{
	color = nToLower(nTrim(color));

	try
	{
		//LOOK FOR COLOR NAME
		{
			auto itr = m_colors.find(color);
			if (itr != m_colors.end())
			{
				//color name exists
				return itr->second;
			}
		}

		//LOOK FOR COLOR VECTOR
		{
			auto itr = color.find(NS(','));
			if (itr != color.npos)
			{
				//comma found
				auto split = nSplit(color, NS(','));
				if (static_cast<int>(split.size()) == 4)
					return nToFloat4(color);
				else
				{
					FLOAT4 result = nToFloat4(color);
					result.w = 1;
					return result;
				}
			}
		}
	}
	catch (exception e)
	{
		
	}
	WriteLine(NS("Color ") + color + NS(" not recognized. returning red"));
	return FLOAT4(1,0,0,1);
}

nstring ScriptInterpreter::EvaluateString(nstring expr) //const
{
	//If expression is empty or too short, return the same.
	if (ITR_SIZE(expr) <= 1) return expr;

	//If there are parenthesis, evaluate them and come back.
	if (expr.find(m_eval_open)) expr = ParseParenthesis(expr);
	
	//Replace all operators with special characters
	for (int i = 0; i < ITR_SIZE(m_sortedOperatorsByLength); i++)
		expr = nReplaceAll(expr, m_sortedOperatorsByLength[i]->m_sign, (nchar)(i+ScriptInterpreter::OPR_SHIFT));

	//All minuses (dash chars) are now replaced with the subtract tag.
	//We should change those that mean negation and not subtraction.
	//Attention: Replacing just the first character does not work at all times.
	//For future reference, here are a couple of examples:
	//(-2^4) works fine by replacing the first character
	//(-2^-2) doesn't work fine, because it'll be evaluated as
	//Step 1: left: -2^ right: 2 op: -
	//Step 2: left: -2  right: [blank] op: ^
	shared_ptr<Operator> minusOperator = make_shared<Operator>(m_operators[minusOperatorIdx]);
	//The following block replaces just the first character
	if (expr[0] ==  minusOperator->m_tag)
	{
		expr.replace(expr.begin(), expr.begin() + 1, minusOperator->m_sign.begin(), minusOperator->m_sign.end());
	}
	//Now we have to search subtraction tags that are immediately after another operator.
	for (auto itr = expr.begin() + 1; itr != expr.end(); itr++)
	{
		if (*itr == minusOperator->m_tag) //subtraction tag found,
			if (static_cast<int>(*(itr-1)) >= ScriptInterpreter::OPR_SHIFT &&  //see if the previous char is an op
				static_cast<int>(*(itr-1)) < ScriptInterpreter::OPR_SHIFT + ITR_SIZE(m_operators))
			{
				//Change the subtraction tag back to the minus sign
				expr.replace(itr, itr+1, minusOperator->m_sign.begin(), minusOperator->m_sign.end());
			}
	}


	//Algorithm tries to find the farthest operator that has
	//the lowest priority, and then split the string recursively
	//to evaluate the string.
	
	//Examples for future reference
	//Example 1:	(2+3+4)
	//				LEFT: 2+3	RIGHT: 4	OP: +
	//				LEFT: 2		RIGHT: 3	OP: +	=	5
	//			<-	LEFT: 5		RIGHT: 4	=	9

	//Example 2:	(2*3+2/3*4+5)
	//				Op: +		LEFT: 2*3+2/3*4		RIGHT: 5
	//				Op: +		LEFT: 2*3			RIGHT: 2/3*4
	//				Op: *		LEFT: 2				RIGHT: 3		= 6
	//				Op: *		LEFT: 2/3			RIGHT: 4
	//				Op: /		LEFT: 2				RIGHT: 3		= 0.67
	//			<=	Op: +		LEFT: 6				RIGHT: 0.67		= 6.67
	//			<=	Op: +		LEFT: 6.67			RIGHT: 5		= 11.67 #
	
	Operator* selectedOperator = m_sortedOperatorsByOrder[0];
	int currentOrder = selectedOperator->m_order;
	int idx = -1;
	int minIdx = -1;
	//Greater number should be on the bottom and means higher priority. 
	//so start with 0 and go upto 5. (Sort must be in ascending order.)
	for (int i = 0; i < ITR_SIZE(m_sortedOperatorsByOrder); i++)
	{
		//We are working from the bottom to top, so find the last index.
		idx = static_cast<int>(expr.find_last_of(m_sortedOperatorsByOrder[i]->m_tag));

		if (m_sortedOperatorsByOrder[i]->m_order != currentOrder 
			&& minIdx >= 0 && minIdx < ITR_SIZE(expr))
		{
			//order is changed, if minIdx is valid, it means that we have one or more
			//low priority operators. minIdx points to the farthest low priority op.
			
			nstring left = selectedOperator->m_unary ? NS("") : EvaluateString(nSubString(expr, 0, minIdx));
			nstring right = EvaluateString(nSubString(expr, minIdx + 1));
			Write(NS("Left: ") + left +
				NS("    Right: ") + right + 
				NS("    Expr: ") + expr + 
				NS("    Op: ") + selectedOperator->m_sign);
			expr = selectedOperator->m_func(left, right);
			WriteLine(NS("  =  ") + expr);

			//reset values and start again.
			minIdx = -1;
			idx = -1;
			i = 0;
			selectedOperator = m_sortedOperatorsByOrder[0];
		}

		currentOrder = m_sortedOperatorsByOrder[i]->m_order;

		//Really find the farthest lower priority operator. 
		//minIdx is the position of the farthest operator found yet.
		if (idx >= 0 && idx > minIdx)
		{
			minIdx = idx;
			selectedOperator = m_sortedOperatorsByOrder[i];
		}
	}

	//THIS IS FOR THE LAST OPERATOR
	if (minIdx >= 0 && minIdx < ITR_SIZE(expr) && selectedOperator == ITR_LAST(m_sortedOperatorsByOrder))
	{
		//should be same as above.
		nstring left = selectedOperator->m_unary ? NS("") : EvaluateString(nSubString(expr, 0, minIdx));
		nstring right = EvaluateString(nSubString(expr, minIdx + 1));
		Write(NS("Left: ") + left + 
			NS("    Right: ") + right + 
			NS("    Expr: ") + expr + 
			NS("    Op: ") + selectedOperator->m_sign);
		expr = selectedOperator->m_func(left, right);
		WriteLine(NS("  =  ") + expr);
	}

	return expr;
}

void ScriptInterpreter::LoadBatch(nstring path)
{
	path = nTrim(path);
	LoadBatch(path, nToLower(nGetFileNameWithoutExt(path)), true);
}

void ScriptInterpreter::LoadBatch(nstring path, nstring name, const bool& call)
{
	path = nTrim(path);
	name = nToLower(nTrim(name));
	
	//This is weird. In the XNA code, I add names as keys, but look for paths as keys.
	//I'll port the same thing here, and I'll fix it if I run into any problems,
	//Since the xna code worked fine.

	if (call)
	{
		auto itr = m_bufferedScripts.find(path);
		if (itr != m_bufferedScripts.end())
		{
			ExecuteBatch(itr->second);
			return;
		}
	}

	if (!nFileExists(path))
	{
		WriteLine(NS("File ") + path + NS(" does not exist."));
		return;
	}

	auto batch = nReadAllLines(path);

	if (call) ExecuteBatch(batch);
	else m_bufferedScripts[name] = batch;
}

void b_goto(vector<nstring> args, ScriptInterpreter* engine)
{
	auto labelsItr = engine->m__labels.find(args[1]);
	if (labelsItr == engine->m__labels.end())
	{
		engine->WriteLine(NS("Label ") + args[1] + NS(" not found."));
		return;
	}

	engine->m__lc = labelsItr->second;
}

void b_end(vector<nstring> args, ScriptInterpreter* engine)
{
	engine->m__batchEnd = true;
}

void ScriptInterpreter::ExecuteBatch(const std::vector<nstring>& script)
{
	m__labelsStack.push(m__labels);
	m__labels.clear();
	AddCommand(NS("goto"), b_goto);
	AddCommand(NS("end"), b_end);

	//PASS I: Find Labels
	for (int i = 0; i < static_cast<int>(script.size()); i++)
	{
		auto trimmed = nTrim(script[i]);
		if (trimmed.size() != 0 && trimmed[0] == NS(':'))
			m__labels[nSubString(trimmed, 1)] = i;
	}

	//PASS II: Interpret
	m__lcStack.push(m__lc);
	for (m__lc = 0; m__lc < static_cast<int>(script.size()); m__lc++)
	{
		auto trimmed = nTrim(script[m__lc]);
		if (trimmed.size() != 0 && trimmed[0] != NS(':'))
		{
			try
			{
				Run(script[m__lc]);
				if (m__batchEnd)
				{
					m__batchEnd = false;
					break;
				}
			}
			catch (exception e)
			{
				WriteLine(NS("Error running [NOT IMPLEMENTED]"));
			}
		}
	}

	//Clean up
	if (static_cast<int>(m__lcStack.size()) > 0) 
	{
		m__lc = m__lcStack.top(); 
		m__lcStack.pop();
	}
	if (static_cast<int>(m__labelsStack.size()) > 0)
	{
		m__labels = m__labelsStack.top();
		m__labelsStack.pop();
	}
}

void ScriptInterpreter::SaveLog(nstring path)
{
	_OFSTREAM_ ofile(path = nTrim(path));
	if (ofile)
	{
		for (auto item : m_buffer)
			ofile << item << endl;
		ofile.close();
	}
	else
		WriteLine(NS("File ") + path + NS(" could not be opened."));
}

void ScriptInterpreter::Clear()
{
	m_buffer.clear();
}

vector<nstring> ScriptInterpreter::GetCommands(void)
{
	vector<nstring> commands;
	for (auto itr = m_commands.begin(); itr != m_commands.end(); itr++)
		commands.push_back(itr->first);
	return commands;
}

vector<nstring> ScriptInterpreter::GetColors(void)
{
	vector<nstring> colors;
	for (auto itr = m_colors.begin(); itr != m_colors.end(); itr++)
		colors.push_back(itr->first);
	return colors;
}

AUTOCOMPLETE_FUNC(Neat::nAutocompleteBoolean)
{
	vector<nstring> result;
	result.push_back(NS("false"));
	result.push_back(NS("true"));
	return result;
}