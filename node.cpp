#include <node.h>
#include <v8.h>
#include "filesystem.h"
using namespace v8;

void Method(const FunctionCallbackInfo<Value>& args) {
	auto isolate = Isolate::GetCurrent();
	auto scope(isolate);

	auto drives = get_drives();

	args.GetReturnValue().Set(String::NewFromTwoByte(isolate, 
		reinterpret_cast<const uint16_t*>(drives[0].description.c_str())));
}

void Init(Handle<Object> exports) {
	auto isolate = Isolate::GetCurrent();
	exports->Set(String::NewFromUtf8(isolate, "hello"), FunctionTemplate::New(isolate, Method)->GetFunction());
}

NODE_MODULE(hello, Init)
