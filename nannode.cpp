#include <nan.h>
#include "filesystem.h"
using namespace v8;

void method_get_drives(const Nan::FunctionCallbackInfo<Value>& args) {

	auto driveArray = Nan::New<Array>();

	auto drives = get_drives();
	int index{ 0 };
	for (auto it = drives.begin(); it < drives.end(); it++)
	{
		Local<Object> obj = Nan::New<Object>();
		obj->Set(Nan::New("name").ToLocalChecked(), 
			Nan::New(reinterpret_cast<const uint16_t*>(it->name.c_str()), static_cast<int>(it->name.length())).ToLocalChecked());
		obj->Set(Nan::New("description").ToLocalChecked(), 		
			Nan::New(reinterpret_cast<const uint16_t*>(it->description.c_str()), static_cast<int>(it->description.length())).ToLocalChecked());
		obj->Set(Nan::New("type").ToLocalChecked(), Nan::New<Number>(static_cast<int>(it->type)));
		obj->Set(Nan::New("size").ToLocalChecked(), Nan::New<Number>(static_cast<double>(it->size)));
		
		driveArray->Set(index++, obj);
	}
	args.GetReturnValue().Set(driveArray);
}

void Init(Local<Object> exports) {
	exports->Set(Nan::New("getDrives").ToLocalChecked(),
		Nan::New<FunctionTemplate>(method_get_drives)->GetFunction());
}

NODE_MODULE(filesystem, Init)
