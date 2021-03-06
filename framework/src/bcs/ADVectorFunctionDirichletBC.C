//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ADVectorFunctionDirichletBC.h"
#include "Function.h"

registerADMooseObject("MooseApp", ADVectorFunctionDirichletBC);

defineADLegacyParams(ADVectorFunctionDirichletBC);

template <ComputeStage compute_stage>
InputParameters
ADVectorFunctionDirichletBC<compute_stage>::validParams()
{
  InputParameters params = ADVectorNodalBC<compute_stage>::validParams();
  params.addClassDescription(
      "Imposes the essential boundary condition $\\vec{u}=\\vec{g}$, where $\\vec{g}$ "
      "components are calculated with functions.");
  params.addParam<FunctionName>("function",
                                "The boundary condition vector function. This cannot be supplied "
                                "with the component parameters.");
  params.addParam<FunctionName>("function_x", 0, "The function for the x component");
  params.addParam<FunctionName>("function_y", 0, "The function for the y component");
  params.addParam<FunctionName>("function_z", 0, "The function for the z component");
  return params;
}

template <ComputeStage compute_stage>
ADVectorFunctionDirichletBC<compute_stage>::ADVectorFunctionDirichletBC(
    const InputParameters & parameters)
  : ADVectorNodalBC<compute_stage>(parameters),
    _function(isParamValid("function") ? &getFunction("function") : nullptr),
    _function_x(getFunction("function_x")),
    _function_y(getFunction("function_y")),
    _function_z(getFunction("function_z"))
{
  if (_function && parameters.isParamSetByUser("function_x"))
    paramError("function_x", "The 'function' and 'function_x' parameters cannot both be set.");
  if (_function && parameters.isParamSetByUser("function_y"))
    paramError("function_y", "The 'function' and 'function_y' parameters cannot both be set.");
  if (_function && parameters.isParamSetByUser("function_z"))
    paramError("function_z", "The 'function' and 'function_z' parameters cannot both be set.");
}

template <ComputeStage compute_stage>
ADRealVectorValue
ADVectorFunctionDirichletBC<compute_stage>::computeQpResidual()
{
  if (_function)
    _values = _function->vectorValue(_t, *_current_node);
  else
    _values = RealVectorValue(_function_x.value(_t, *_current_node),
                              _function_y.value(_t, *_current_node),
                              _function_z.value(_t, *_current_node));

  return _u - _values;
}
