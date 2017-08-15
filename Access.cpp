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
	auto callback = new Callback(args[0].As<Function>());
	MessageBox(0, "1", "", MB_OK);
	vector<Drive_info> drives;
	AsyncQueueWorker(new Worker(callback, [&drives]()-> void
	{
		MessageBox(0, "2", "", MB_OK);

		vector<Drive_info> drives1;
		drives1 = move(get_drives());
		MessageBox(0, "2.5", "", MB_OK);
	}, [&drives](Nan::Callback* callback)-> void
	{
		auto driveArray = Nan::New<Array>();
		MessageBox(0, "3", "", MB_OK);
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

		const unsigned argc = 1;
		Local<Value> argv[argc] = { driveArray };

		callback->Call(1, argv);

	}));
	//args.GetReturnValue().Set(id);
}

void Access::method_list_files(const Nan::FunctionCallbackInfo<Value>& args)
{

}
