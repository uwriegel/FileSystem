#include <nan.h>
#include <atomic>
#include "Access.h"
using namespace v8;
using namespace Nan;
using namespace std;

void create_object(const Nan::FunctionCallbackInfo<Value>& info) {
	info.GetReturnValue().Set(Access::NewInstance());
}

void Init(Local<Object> exports) 
{
	Access::Init(exports);
	exports->Set(Nan::New("createAccess").ToLocalChecked(), Nan::New<FunctionTemplate>(create_object)->GetFunction());
}

NODE_MODULE(filesystem, Init)


