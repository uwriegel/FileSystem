#pragma once
#include <nan.h>
#include <atomic>
#include <string>
using namespace Nan;
using namespace v8;
using namespace std;

class Access : public ObjectWrap 
{
public:
	static void Init(Local<Object> exports);
	static Local<Object> NewInstance();

private:
	Access();
	~Access();

	static void New(const Nan::FunctionCallbackInfo<Value>& info);

	static void method_get_drives(const Nan::FunctionCallbackInfo<Value>& args);

	static Nan::Persistent<Function> constructor;
//	wstring value{ L"Das ist aber schön" };

	static atomic<int> lastId;
};

