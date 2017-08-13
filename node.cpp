#include <node.h>
#include <v8.h>
#include "filesystem.h"
using namespace v8;

void method_get_drives(const FunctionCallbackInfo<Value>& args) {
	auto isolate = Isolate::GetCurrent();
	auto scope(isolate);
	auto callback = Local<Function>::Cast(args[0]);

	auto driveArray = Array::New(isolate);

	auto drives = get_drives();
	int index{ 0 };
	for (auto it = drives.begin(); it < drives.end(); it++)
	{
		Local<Object> obj = Object::New(isolate);
		obj->Set(String::NewFromUtf8(isolate, "name"), String::NewFromTwoByte(isolate,
			reinterpret_cast<const uint16_t*>(it->name.c_str())));
		obj->Set(String::NewFromUtf8(isolate, "description"), String::NewFromTwoByte(isolate,
			reinterpret_cast<const uint16_t*>(it->description.c_str())));
		obj->Set(String::NewFromUtf8(isolate, "type"), Number::New(isolate, static_cast<int>(it->type)));

		driveArray->Set(index++, obj);
	}
	args.GetReturnValue().Set(driveArray);
}

void Init(Handle<Object> exports) {
	auto isolate = Isolate::GetCurrent();
	exports->Set(String::NewFromUtf8(isolate, "getDrives"), FunctionTemplate::New(isolate, method_get_drives)->GetFunction());
}

NODE_MODULE(filesystem, Init)
