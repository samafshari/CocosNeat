#ifndef __SCRIPTINTERPRETER_H__
#define __SCRIPTINTERPRETER_H__

#include "NeatLib.h"
#include "NeatOperator.h"
#include "NeatRam.h"

#define SCRIPT_LAMBDA [](std::vector<nstring> args, Neat::ScriptInterpreter* engine)
#define AUTOCOMPLETE_LAMBDA [](std::vector<nstring> args, Neat::ScriptInterpreter* engine)

#define SCRIPT_FUNC(name) void (name)(std::vector<nstring> args, Neat::ScriptInterpreter* engine)
#define AUTOCOMPLETE_FUNC(name) std::vector<nstring> (name)(std::vector<nstring> args, Neat::ScriptInterpreter* engine)

#define AUTOCOMPLETE_COLORS AUTOCOMPLETE_LAMBDA { return engine->GetColors(); }
#define AUTOCOMPLETE_BOOL nAutocompleteBoolean

namespace Neat
{
	class ScriptInterpreter;

	typedef void (*nScriptFunction)(std::vector<nstring>, ScriptInterpreter*);
	typedef std::vector<nstring> (*nAutocompleteFunction)(std::vector<nstring>, ScriptInterpreter*);

	class ScriptInterpreter
	{
	private:
		//Consts
		nchar m_eval_open;
		nchar m_eval_close;
		nstring m_bad_command_msg;
		int m_max_lines;

		//Settings
		bool m_echo;
		std::map<nstring, nScriptFunction> m_commands;
		std::map<nstring, nAutocompleteFunction> m_autocompletes;
		std::map<nstring, nstring> m_infos;
		std::vector<nstring> m_buffer;

		std::map<nstring, FLOAT4> m_colors;

		std::vector<Operator> m_operators;
		std::vector<Operator* > m_sortedOperatorsByLength, m_sortedOperatorsByOrder;
		
		std::map<nstring, std::vector<nstring> > m_bufferedScripts;

		void initExpressionEvaluation(void);
		void sortOperators(void);
		void initColors(void);

		int minusOperatorIdx;

		static const int OPR_SHIFT;
	public:
		//batch vars
		int m__lc;
		std::map<nstring, int> m__labels;
		std::stack<std::map<nstring, int> > m__labelsStack;
		std::stack<int> m__lcStack;
		bool m__batchEnd;
		
		int m_max_chars;
		Ram m_ram;

		ScriptInterpreter(void);
		~ScriptInterpreter(void);

		virtual void Run(nstring cmd);
		virtual void Run(const std::vector<nstring>& args);

		void AddCommand(
			const nstring& key, const nScriptFunction& action, 
			const nstring& info);
		void AddCommand(
			const nstring& key, const nScriptFunction& action, 
			const nAutocompleteFunction& autoComplete = nullptr, 
			const nstring& info = NS(""));
		void AddCommand(
			const nstring& newAlias, const nstring& originalName);

		bool RemoveCommand(const nstring& key);

		void AutoComplete(nstring& input);
		void PrintHelp(nstring command);

		void Write(const nstring& msg = NS(""));
		void WriteLine(const nstring& msg = NS(""));
		void Write(const float msg);
		void Write(const double msg);
		void Write(const FLOAT2 msg);
		void Write(const FLOAT3 msg);
		void Write(const FLOAT4 msg);
		void Write(const bool msg);
		void WriteLine(const float msg);
		void WriteLine(const double msg);
		void WriteLine(const FLOAT2 msg);
		void WriteLine(const FLOAT3 msg);
		void WriteLine(const FLOAT4 msg);
		void WriteLine(const bool msg);
		std::vector<nstring> GetMessages(void) const;
		std::vector<nstring> GetMessages(const int& count, const int& offset) const;
		
		nstring ParseParenthesis(nstring& command);// const;
		FLOAT4 ParseColor(nstring color);
		nstring EvaluateString(nstring expr); //const;

		void LoadBatch(nstring path);
		void LoadBatch(nstring path, nstring name, const bool& call);
		void ExecuteBatch(const std::vector<nstring>& script);
		
		void SaveLog(nstring path);
		void Clear();

		inline void SetEcho(const bool& value) { m_echo = value; }

		std::vector<nstring> GetCommands(void);
		std::vector<nstring> GetColors(void);
	};

	AUTOCOMPLETE_FUNC(nAutocompleteBoolean);
}

#endif