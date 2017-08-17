#include <nan.h>
#include <atomic>
#include "Access.h"
#include "filesystem.h"
using namespace v8;
using namespace Nan;
using namespace std;

void create_object(const Nan::FunctionCallbackInfo<Value>& info) {
	info.GetReturnValue().Set(Access::NewInstance());
}

void show_hidden(const Nan::FunctionCallbackInfo<Value>& info) {
	showHidden = info[0].As<v8::Boolean>()->BooleanValue();
}

void Init(Local<Object> exports) 
{
	Access::Init(exports);
	exports->Set(Nan::New("createAccess").ToLocalChecked(), Nan::New<FunctionTemplate>(create_object)->GetFunction());
	exports->Set(Nan::New("showHidden").ToLocalChecked(), Nan::New<FunctionTemplate>(show_hidden)->GetFunction());
}

NODE_MODULE(filesystem, Init)


