#include <nan.h>
#include <atomic>
#include "filesystem.h"
#include "Access.h"
using namespace v8;
using namespace Nan;
using namespace std;

class PrimeWorker : public AsyncWorker
{
public:
	PrimeWorker(Callback *callback, int id)
		: AsyncWorker(callback)
		, id(id)
		{}
	void Execute();
	void HandleOKCallback();
private:
	vector<Drive_info> drives;
	int id;
};

std::atomic<int> lastId;

void PrimeWorker::Execute() 
{
	drives = move(get_drives());
};

void PrimeWorker::HandleOKCallback() 
{
	auto driveArray = New<Array>();
		
	int index{ 0 };
	for (auto it = drives.begin(); it < drives.end(); it++)
	{
		Local<Object> obj = New<Object>();
		obj->Set(New("name").ToLocalChecked(),
			New(reinterpret_cast<const uint16_t*>(it->name.c_str()), static_cast<int>(it->name.length())).ToLocalChecked());
		obj->Set(New("description").ToLocalChecked(),
			New(reinterpret_cast<const uint16_t*>(it->description.c_str()), static_cast<int>(it->description.length())).ToLocalChecked());
		obj->Set(New("type").ToLocalChecked(), New<Number>(static_cast<int>(it->type)));
		obj->Set(New("size").ToLocalChecked(), New<Number>(static_cast<double>(it->size)));

		driveArray->Set(index++, obj);
	}

	const unsigned argc = 1;
	Local<Value> argv[argc] = { driveArray };

	callback->Call(1, argv);
}

void method_get_drives(const Nan::FunctionCallbackInfo<Value>& args) {
	auto id = atomic_fetch_add(&lastId, 1);
	auto callback = new Callback(args[0].As<Function>());
	AsyncQueueWorker(new PrimeWorker(callback, id));
	//args.GetReturnValue().Set(id);
}

void create_object(const Nan::FunctionCallbackInfo<Value>& info) {
	//info.GetReturnValue().Set(Access::NewInstance(info[0]));
	info.GetReturnValue().Set(Access::NewInstance());
}

void Init(Local<Object> exports) 
{
	Access::Init(exports);

	exports->Set(Nan::New("createAccess").ToLocalChecked(), Nan::New<FunctionTemplate>(create_object)->GetFunction());

	exports->Set(New("getDrives").ToLocalChecked(), New<FunctionTemplate>(method_get_drives)->GetFunction());
	
}

NODE_MODULE(filesystem, Init)


