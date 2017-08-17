#include "Access.h"
#include "worker.h"
#include "tools.h"
#include "nantools.h"
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
	SetPrototypeMethod(tpl, "getDrives", get_drives);
	SetPrototypeMethod(tpl, "listFiles", list_files);

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

void Access::get_drives(const Nan::FunctionCallbackInfo<Value>& args)
{
	if (args[0]->IsUndefined())
		return;
	auto callback = new Callback(args[0].As<Function>());
	auto drives = make_shared<vector<Drive_info>>();
	AsyncQueueWorker(new Worker(callback, [drives]()-> void
	{
		*drives = move(::get_drives());
	}, [drives](Nan::Callback* callback)-> void
	{
		auto driveArray = Nan::New<Array>();
		int index{ 0 };
		for (auto it = drives->begin(); it < drives->end(); it++)
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

void Access::list_files(const Nan::FunctionCallbackInfo<Value>& args)
{
	if (args[1]->IsUndefined())
		return;
	auto directory{ move(get_normalized_path(get_string_value(args[0]))) };
	auto callback = new Callback(args[1].As<Function>());
	auto items = make_shared<vector<Item>>();
	AsyncQueueWorker(new Worker(callback, [directory, items]()-> void
	{
		*items = move(::list_files(directory));
	}, [directory, items](Nan::Callback* callback)-> void
	{
		auto itemArray = Nan::New<Array>();
		int index{ 0 };
		for (auto it = items->begin(); it < items->end(); it++)
		{
			Local<Object> obj = Nan::New<Object>();
			obj->Set(Nan::New("imageUrl").ToLocalChecked(),
				Nan::New(reinterpret_cast<const uint16_t*>(it->image_url.c_str()), static_cast<int>(it->image_url.length())).ToLocalChecked());
			obj->Set(Nan::New("name").ToLocalChecked(),
				Nan::New(reinterpret_cast<const uint16_t*>(it->name.c_str()), static_cast<int>(it->name.length())).ToLocalChecked());
			if (it->size != -1)
				obj->Set(Nan::New("fileSize").ToLocalChecked(), Nan::New<Number>(static_cast<double>(it->size)));
			obj->Set(Nan::New("dateTime").ToLocalChecked(),
				Nan::New(reinterpret_cast<const uint16_t*>(it->date_time.c_str()), static_cast<int>(it->date_time.length())).ToLocalChecked());
			obj->Set(Nan::New("kind").ToLocalChecked(), Nan::New<Number>(static_cast<int>(it->kind)));
			obj->Set(Nan::New("isHidden").ToLocalChecked(), Nan::New<Boolean>(it->is_hidden));
			obj->Set(Nan::New("isDirectory").ToLocalChecked(), Nan::New<Boolean>(it->is_directory));
			if (it->parent.length() > 0)
				obj->Set(Nan::New("parent").ToLocalChecked(),
					Nan::New(reinterpret_cast<const uint16_t*>(it->parent.c_str()), static_cast<int>(it->parent.length())).ToLocalChecked());

			itemArray->Set(index++, obj);
		}

		wstring current_directory{ move(directory) };
		Local<Object> itemsResult = Nan::New<Object>();
		itemsResult->Set(Nan::New("currentDirectory").ToLocalChecked(), 
			Nan::New(reinterpret_cast<const uint16_t*>(current_directory.c_str()), static_cast<int>(current_directory.length())).ToLocalChecked());
		itemsResult->Set(Nan::New("items").ToLocalChecked(), itemArray);

		const unsigned argc = 1;
		Local<Value> argv[argc] = { itemsResult };
		
		callback->Call(1, argv);
	}));
}
