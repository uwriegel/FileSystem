#pragma once
#include <nan.h>
#include "filesystem.h"
using namespace v8;
using namespace Nan;
using namespace std;

class Worker : public AsyncWorker
{
public:
	Worker(Callback *callback, int id)
		: AsyncWorker(callback)
		, id(id)
	{}
	void Execute();
	void HandleOKCallback();
private:
	vector<Drive_info> drives;
	int id;
};

