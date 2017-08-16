#pragma once
#include <nan.h>
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

	static void get_drives(const Nan::FunctionCallbackInfo<Value>& args);
	static void list_files(const Nan::FunctionCallbackInfo<Value>& args);

	static Nan::Persistent<Function> constructor;
};

