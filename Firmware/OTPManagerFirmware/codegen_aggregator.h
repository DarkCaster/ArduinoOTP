#ifndef CODEGEN_AGGREGATOR_H
#define CODEGEN_AGGREGATOR_H

#include <Arduino.h>
#include "codegen_manager.h"

class CodeGenAggregator
{
	private:
		const CodeGenConfig * const codeGenCofigs;
		const size_t codeGenCofigsSz;
		CodeGenAggregator(const CodeGenConfig * const codeGenCofigs, const size_t CSZ);
	public:
		template<size_t CSZ> CodeGenAggregator(const CodeGenConfig (&codeGenCofigs)[CSZ]) :
		  CodeGenAggregator(codeGenCofigs, CSZ) { }
		template<size_t CSZ> CodeGenAggregator(const CodeGenConfig (&&)[CSZ]) = delete;

};

#endif
