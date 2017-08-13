#include <node.h>
#include <v8.h>
#include "filesystem.h"
using namespace v8;

void method_get_drives(const FunctionCallbackInfo<Value>& args) {
	auto isolate = Isolate::GetCurrent();
	auto scope(isolate);
	auto callback = Local<Function>::Cast(args[0]);

	auto drives = get_drives();
	for (auto it = drives.begin(); it < drives.end(); it++)
	{
		const unsigned argc = 1;
		Local<Value> argv[argc] = { String::NewFromTwoByte(isolate,
			reinterpret_cast<const uint16_t*>(it->description.c_str())) };
		callback->Call(isolate->GetCurrentContext()->Global(), argc, argv);
	}
}

void Init(Handle<Object> exports) {
	auto isolate = Isolate::GetCurrent();
	exports->Set(String::NewFromUtf8(isolate, "getDrives"), FunctionTemplate::New(isolate, method_get_drives)->GetFunction());
}

NODE_MODULE(filesystem, Init)
