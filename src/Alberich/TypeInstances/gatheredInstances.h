#pragma once

#include "InstanceIncludes.h"

// Types
#include "../LanguageRegister/BackendRegister.h"
#include "VOID.h"
#include "ARGS.h"
#include "BOOL.h"
#include "INT.h"
#include "DOUBLE.h"
#include "STRING.h"
#include "TENSOR_EXPRESSION.h"
#include "TENSOR_EXPRESSION_EQUATION.h"

#include "Operations.h"

bool setUpCore();
void setUpTypes();