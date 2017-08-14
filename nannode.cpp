#include <nan.h>
#include "filesystem.h"
using namespace v8;
using namespace std;

class PrimeWorker : public Nan::AsyncWorker
{
public:
	PrimeWorker(Nan::Callback *callback)
		: AsyncWorker(callback) {}
	void Execute();
	void HandleOKCallback();
};


void PrimeWorker::Execute() 
{
	Teste();
};

void PrimeWorker::HandleOKCallback() 
{
	Nan::HandleScope scope;

	wstring test{ L"Das kommt zurück!" };
	const unsigned argc = 1;
	Local<Value> argv[argc] = { Nan::New(reinterpret_cast<const uint16_t*>(test.c_str()), static_cast<int>(test.length())).ToLocalChecked() };

	//Local<Value> argv[] = { Null(), results };
	callback->Call(1, argv);
}

void method_get_drives(const Nan::FunctionCallbackInfo<Value>& args) {

	
	
	
	wstring test{ L"Das kommt zurück!" };

	auto callback = new Nan::Callback(args[0].As<Function>());
	Nan::AsyncQueueWorker(new PrimeWorker(callback));
	
	//Local<Function> cb = args[0].As<Function>();
	//const unsigned argc = 1;
	//Local<Value> argv[argc] = { Nan::New(reinterpret_cast<const uint16_t*>(test.c_str()), static_cast<int>(test.length())).ToLocalChecked() };
	//Nan::MakeCallback(Nan::GetCurrentContext()->Global(), cb, argc, argv);
	
	
	
	
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
