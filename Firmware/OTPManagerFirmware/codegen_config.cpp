#include "codegen_manager.h"

CodeGenConfig::CodeGenConfig(uint8_t type, CodeGenManager& generator) :
  type(type),
  generator(generator)
{}
