#include "worker.h"

void Worker::Execute()
{
	drives = move(get_drives());
};

void Worker::HandleOKCallback()
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
