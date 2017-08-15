#include "Access.h"
#include "worker.h"
using namespace std;
using namespace Nan;
using namespace v8;

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
	SetPrototypeMethod(tpl, "getDrives", method_get_drives);

	constructor.Reset(tpl->GetFunction());
}

//Local<Object> Access::NewInstance(Local<Value> arg)
Local<Object> Access::NewInstance()
{
	Nan::EscapableHandleScope scope;

	//const unsigned argc = 1;
	//Local<Value> argv[argc] = { arg };
	Local<Function> cons = Nan::New<Function>(constructor);
	//Local<Object> instance = cons->NewInstance(argc, argv); // depri
	auto maybeInstance = Nan::NewInstance(cons, 0, nullptr);
	auto instance = maybeInstance.ToLocalChecked();
	return scope.Escape(instance);
}

void Access::New(const Nan::FunctionCallbackInfo<Value>& info) 
{
	auto obj = new Access();
	obj->Wrap(info.This());
	info.GetReturnValue().Set(info.This());
}

//void Access::GetValue(const Nan::FunctionCallbackInfo<Value>& info) {
//	auto obj = ObjectWrap::Unwrap<Access>(info.Holder());
//	info.GetReturnValue().Set(Nan::New(reinterpret_cast<const uint16_t*>(obj->value.c_str()), 
//		static_cast<int>(obj->value.length())).ToLocalChecked());
//}

void Access::method_get_drives(const Nan::FunctionCallbackInfo<Value>& args)
{
	auto id = atomic_fetch_add(&lastId, 1);
	auto callback = new Callback(args[0].As<Function>());
	AsyncQueueWorker(new Worker(callback, id));
	//args.GetReturnValue().Set(id);
}

atomic<int> Access::lastId;