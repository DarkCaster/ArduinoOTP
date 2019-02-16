#include "codegen_aggregator.h"

CodeGenAggregator::CodeGenAggregator(const CodeGenConfig* const codeGenCofigs, const size_t CSZ) :
  codeGenCofigs(codeGenCofigs),
  codeGenCofigsSz(CSZ)
{ }

CodeGenManager * CodeGenAggregator::GetManager(const ProfileType type)
{
	for(size_t i=0;i<codeGenCofigsSz;++i)
		if(codeGenCofigs[i].type==type)
			return &(codeGenCofigs[i].generator);
	return nullptr;
}
