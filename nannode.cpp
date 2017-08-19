#include <nan.h>
#include <atomic>
#include "Access.h"
#include "filesystem.h"
#include "web_server.h"
using namespace v8;
using namespace Nan;
using namespace std;

Web_server web_server;

void create_object(const Nan::FunctionCallbackInfo<Value>& info) {
	info.GetReturnValue().Set(Access::NewInstance());
}

void show_hidden(const Nan::FunctionCallbackInfo<Value>& info) {
	showHidden = info[0].As<v8::Boolean>()->BooleanValue();
}

void Init(Local<Object> exports) 
{
	web_server.start();
	Access::Init(exports);
	exports->Set(Nan::New("createAccess").ToLocalChecked(), Nan::New<FunctionTemplate>(create_object)->GetFunction());
	exports->Set(Nan::New("showHidden").ToLocalChecked(), Nan::New<FunctionTemplate>(show_hidden)->GetFunction());
}

NODE_MODULE(filesystem, Init)


