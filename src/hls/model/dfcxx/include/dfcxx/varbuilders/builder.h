#ifndef DFCXX_VAR_BUILDER_H
#define DFCXX_VAR_BUILDER_H

#include "dfcxx/vars/vars.h"

namespace dfcxx {

class VarBuilder {
public:
  DFVariableImpl *
  buildStream(const std::string &name, IODirection direction, GraphHelper &helper,
              DFTypeImpl &type);

  DFVariableImpl *
  buildScalar(const std::string &name, IODirection direction, GraphHelper &helper,
              DFTypeImpl &type);

  DFVariableImpl *
  buildConstant(GraphHelper &helper, DFTypeImpl &type, ConstantTypeKind kind,
                ConstantValue value);

  DFVariable
  buildStream(const std::string &name, IODirection direction, GraphHelper &helper,
              const DFType &type);

  DFVariable
  buildScalar(const std::string &name, IODirection direction, GraphHelper &helper,
              const DFType &type);

  DFVariable
  buildConstant(GraphHelper &helper, const DFType &type, ConstantTypeKind kind,
                ConstantValue value);

  DFVariable
  buildMuxCopy(const DFVariable &var, GraphHelper &helper);
};

} // namespace dfcxx

#endif // DFCXX_VAR_BUILDER_H
