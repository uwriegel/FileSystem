#include "Access.h"

Nan::Persistent<Function> Access::constructor;

Access::Access() 
{
}

Access::~Access() 
{
}

void Access::Init(Local<Object> exports) 
{
	Nan::HandleScope scope;

	// Prepare constructor template
	auto tpl = Nan::New<FunctionTemplate>(New);
	tpl->SetClassName(Nan::New("Access").ToLocalChecked());
	tpl->InstanceTemplate()->SetInternalFieldCount(1);

	// Prototype
	SetPrototypeMethod(tpl, "getValue", GetValue);

	constructor.Reset(tpl->GetFunction());
	exports->Set(Nan::New("Access").ToLocalChecked(), tpl->GetFunction());
}

void Access::New(const Nan::FunctionCallbackInfo<v8::Value>& info) 
{
	if (info.IsConstructCall()) 
	{
		// Invoked as constructor: `new MyObject(...)`
		//double value = info[0]->IsUndefined() ? 0 : info[0]->NumberValue();
		//auto obj = new Access(value);
		auto obj = new Access();
		obj->Wrap(info.This());
		info.GetReturnValue().Set(info.This());
	}
	else 
	{
		// Invoked as plain function `MyObject(...)`, turn into construct call.
		//const int argc = 1;
		//Local<Value> argv[argc] = { info[0] };
		Local<Function> cons = Nan::New<Function>(constructor);
		//info.GetReturnValue().Set(cons->NewInstance(argc, argv));
		info.GetReturnValue().Set(cons->NewInstance());
	}
}

void Access::GetValue(const Nan::FunctionCallbackInfo<Value>& info) {
	auto obj = ObjectWrap::Unwrap<Access>(info.Holder());
	info.GetReturnValue().Set(Nan::New(reinterpret_cast<const uint16_t*>(obj->value.c_str()), 
		static_cast<int>(obj->value.length())).ToLocalChecked());
}

