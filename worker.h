#pragma once
#include <nan.h>
#include <functional>
#include "filesystem.h"
using namespace v8;
using namespace Nan;
using namespace std;

class Worker : public AsyncWorker
{
public:
	Worker(Callback *callback, function<void()> do_work, function<void(Callback* callback)> on_ok)
		: AsyncWorker(callback)
		, do_work(do_work)
		, on_ok(on_ok)
	{}
	void Execute();
	void HandleOKCallback();
private:
	function<void()> do_work;
	function<void(Callback* callback)> on_ok;
};

