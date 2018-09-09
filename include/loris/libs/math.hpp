/*
Copyright (c) 2014, Nicolas Brown
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of dragonscript nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#pragma once

#include "../assembly.hpp"
#include "../virtualmachine.hpp"

using namespace dragonscript;

namespace DSMathLib
{

Value NativeMod(VirtualMachine* vm,Object* self)
{
	return Value::CreateNumber(fmod(vm->GetArg(0).AsNumber(),vm->GetArg(1).AsNumber() ));
}

Value NativeFloor(VirtualMachine* vm,Object* self)
{
	return Value::CreateNumber(floor(vm->GetArg(0).AsNumber()));
}

Value NativeCeil(VirtualMachine* vm,Object* self)
{
	return Value::CreateNumber(ceil(vm->GetArg(0).AsNumber()));
}

Value NativeSin(VirtualMachine* vm,Object* self)
{
	return Value::CreateNumber(sin(vm->GetArg(0).AsNumber()));
}

Value NativeCos(VirtualMachine* vm,Object* self)
{
	return Value::CreateNumber(cos(vm->GetArg(0).AsNumber()));
}

Value NativeTan(VirtualMachine* vm,Object* self)
{
	return Value::CreateNumber(tan(vm->GetArg(0).AsNumber()));
}

Value NativeAsin(VirtualMachine* vm,Object* self)
{
	return Value::CreateNumber(asin(vm->GetArg(0).AsNumber()));
}

Value NativeAcos(VirtualMachine* vm,Object* self)
{
	return Value::CreateNumber(acos(vm->GetArg(0).AsNumber()));
}

Value NativeAtan(VirtualMachine* vm,Object* self)
{
	return Value::CreateNumber(atan(vm->GetArg(0).AsNumber()));
}

Value NativeExp10(VirtualMachine* vm,Object* self)
{
	return Value::CreateNumber(exp(vm->GetArg(0).AsNumber()*log(10)));
}

Value NativeExp2(VirtualMachine* vm,Object* self)
{
	return Value::CreateNumber(exp(vm->GetArg(0).AsNumber()*log(10)));
}

Value NativeLog10(VirtualMachine* vm,Object* self)
{
	return Value::CreateNumber(log(vm->GetArg(0).AsNumber())/log(10));
}

Value NativeLog2(VirtualMachine* vm,Object* self)
{
	return Value::CreateNumber(log(vm->GetArg(0).AsNumber())/log(2));
}

Value NativePow(VirtualMachine* vm,Object* self)
{
	return Value::CreateNumber(pow(vm->GetArg(0).AsNumber(),vm->GetArg(1).AsNumber() ));
}

Value NativeLog(VirtualMachine* vm,Object* self)
{
	return Value::CreateNumber(log(vm->GetArg(0).AsNumber()));
}

Value NativeExp(VirtualMachine* vm,Object* self)
{
	return Value::CreateNumber(exp(vm->GetArg(0).AsNumber()));
}


void Install(Assembly* lib)
{
	lib->AddNativeFunction("mod",NativeMod);
	lib->AddNativeFunction("floor",NativeFloor);
	lib->AddNativeFunction("ceil",NativeCeil);
	lib->AddNativeFunction("sin",NativeSin);
	lib->AddNativeFunction("cos",NativeCos);
	lib->AddNativeFunction("tan",NativeTan);
	lib->AddNativeFunction("asin",NativeAsin);
	lib->AddNativeFunction("acos",NativeAcos);
	lib->AddNativeFunction("atan",NativeAtan);
	lib->AddNativeFunction("log10",NativeLog10);
	lib->AddNativeFunction("log2",NativeLog2);
	lib->AddNativeFunction("exp10",NativeExp10);
	lib->AddNativeFunction("exp2",NativeExp2);
	lib->AddNativeFunction("pow",NativePow);
	lib->AddNativeFunction("exp",NativeExp);
	lib->AddNativeFunction("log",NativeLog);
}

}